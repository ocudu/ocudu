// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "lib/scheduler/common_scheduling/prs_scheduler.h"
#include "sub_scheduler_test_environment.h"
#include "tests/test_doubles/scheduler/scheduler_config_helper.h"
#include "ocudu/ran/prs/prs.h"
#include <gtest/gtest.h>

using namespace ocudu;

namespace {

constexpr uint16_t start_prb      = 0;
constexpr uint16_t bandwidth_prbs = 24;
constexpr uint8_t  symbol_offset  = 0;
constexpr uint16_t sequence_id    = 123;

/// Builds a PRS resource set with a single resource, no repetitions and no muting.
prs_resource_set make_prs_resource_set(unsigned periodicity_slots, unsigned slot_offset)
{
  prs_resource_set res_set{};
  res_set.start_prb         = start_prb;
  res_set.bandwidth_prbs    = bandwidth_prbs;
  res_set.comb_size         = prs_comb_size::two;
  res_set.periodicity_slots = periodicity_slots;
  res_set.slot_offset       = slot_offset;
  res_set.repetition_factor = prs_repetition_factor::one;
  res_set.time_gap          = prs_time_gap::one;
  res_set.nof_symbols       = prs_num_symbols::two;
  res_set.power_offset_db   = 0;
  res_set.resources.push_back(
      prs_resource{.sequence_id = sequence_id, .re_offset = 0, .slot_offset = 0, .symbol_offset = symbol_offset});
  return res_set;
}

/// Builds a muting pattern of the given size out of a bitmap, where bit i of the bitmap is repetition/instance i.
bounded_bitset<prs_constants::VALID_MUTING_PATTERN_SIZES.back()> make_muting_pattern(unsigned size, uint32_t bitmap)
{
  bounded_bitset<prs_constants::VALID_MUTING_PATTERN_SIZES.back()> pattern(size);
  for (unsigned i = 0; i != size; ++i) {
    pattern.set(i, ((bitmap >> i) & 1U) != 0);
  }
  return pattern;
}

/// Test bench that runs a \c prs_scheduler over the resource grid of a cell with the given DL-PRS configuration.
class prs_test_bench : public sub_scheduler_test_environment
{
public:
  explicit prs_test_bench(prs_config prs_cfg) :
    sub_scheduler_test_environment(make_cell_req(std::move(prs_cfg))), prs_sch(cell_cfg)
  {
  }

  void do_run_slot() override { prs_sch.run_slot(res_grid); }

  /// DL-PRS PDUs scheduled in the slot that was last run.
  span<const prs_info> last_prs_pdus() const { return last_sched_result().dl.prs; }

  /// Slot that was last run, as a count of slots since SFN 0.
  unsigned last_slot_count() const { return last_slot_tx().count(); }

private:
  static sched_cell_configuration_request_message make_cell_req(prs_config prs_cfg)
  {
    sched_cell_configuration_request_message req = sched_config_helper::make_default_sched_cell_configuration_request();
    req.ran.prs_cfg                              = std::move(prs_cfg);
    return req;
  }

  prs_scheduler prs_sch;
};

} // namespace

TEST(prs_scheduler_test, no_resource_set_configured_schedules_nothing)
{
  prs_test_bench bench{prs_config{}};

  for (unsigned i = 0; i != 40; ++i) {
    bench.run_slot();
    ASSERT_TRUE(bench.last_prs_pdus().empty()) << "DL-PRS scheduled with no resource set configured";
  }
}

TEST(prs_scheduler_test, resource_is_scheduled_on_the_slots_of_its_period_and_offset)
{
  constexpr unsigned period      = 10;
  constexpr unsigned slot_offset = 4;

  prs_config cfg;
  cfg.resource_sets.push_back(make_prs_resource_set(period, slot_offset));
  prs_test_bench bench{cfg};

  // Run for long enough that the resource grid ring wraps around several times, so that PDUs left over from a previous
  // use of a ring slot would show up as an extra, or a spurious, DL-PRS transmission.
  for (unsigned i = 0; i != 40 * period; ++i) {
    bench.run_slot();

    const bool is_prs_slot = (bench.last_slot_count() % period) == slot_offset;
    ASSERT_EQ(bench.last_prs_pdus().size(), is_prs_slot ? 1U : 0U)
        << "Unexpected number of DL-PRS PDUs in slot " << bench.last_slot_count();
  }
}

TEST(prs_scheduler_test, scheduled_pdu_matches_the_resource_configuration)
{
  constexpr unsigned period      = 10;
  constexpr unsigned slot_offset = 0;

  prs_config cfg;
  cfg.resource_sets.push_back(make_prs_resource_set(period, slot_offset));
  prs_test_bench bench{cfg};

  ASSERT_TRUE(bench.run_slot_until([&bench]() { return not bench.last_prs_pdus().empty(); }, 2 * period));

  const prs_info& pdu = bench.last_prs_pdus().front();
  ASSERT_EQ(pdu.n_id_prs, sequence_id);
  ASSERT_EQ(pdu.comb_size, prs_comb_size::two);
  ASSERT_EQ(pdu.comb_offset, 0);
  ASSERT_EQ(pdu.nof_symbols, prs_num_symbols::two);
  ASSERT_EQ(pdu.symbols, ofdm_symbol_range(symbol_offset, symbol_offset + 2));
  ASSERT_EQ(pdu.crbs, crb_interval(start_prb, start_prb + bandwidth_prbs));
  ASSERT_EQ(pdu.power_offset_db, 0);
  ASSERT_EQ(pdu.scs, bench.cell_cfg.params.dl_cfg_common.init_dl_bwp.generic_params.scs);
}

TEST(prs_scheduler_test, scheduled_resource_is_reserved_in_the_resource_grid)
{
  constexpr unsigned period      = 10;
  constexpr unsigned slot_offset = 0;

  prs_config cfg;
  cfg.resource_sets.push_back(make_prs_resource_set(period, slot_offset));
  prs_test_bench bench{cfg};

  ASSERT_TRUE(bench.run_slot_until([&bench]() { return not bench.last_prs_pdus().empty(); }, 2 * period));

  // The resource grid is RB-granular, so the whole DL-PRS bandwidth is reserved for the symbols of the resource.
  const prs_info& pdu = bench.last_prs_pdus().front();
  ASSERT_TRUE(bench.res_grid[0].dl_res_grid.all_set(grant_info{pdu.scs, pdu.symbols, pdu.crbs}));
}

TEST(prs_scheduler_test, every_repetition_of_a_resource_is_scheduled)
{
  constexpr unsigned period      = 20;
  constexpr unsigned slot_offset = 1;
  constexpr unsigned time_gap    = 2;
  constexpr unsigned nof_reps    = 4;

  prs_resource_set res_set  = make_prs_resource_set(period, slot_offset);
  res_set.repetition_factor = prs_repetition_factor::four;
  res_set.time_gap          = prs_time_gap::two;

  prs_config cfg;
  cfg.resource_sets.push_back(res_set);
  prs_test_bench bench{cfg};

  for (unsigned i = 0; i != 3 * period; ++i) {
    bench.run_slot();

    // The repetitions occur every time_gap slots, starting at the slot offset of the set.
    const unsigned phase       = bench.last_slot_count() % period;
    const bool     is_prs_slot = phase >= slot_offset and ((phase - slot_offset) % time_gap) == 0 and
                             ((phase - slot_offset) / time_gap) < nof_reps;
    ASSERT_EQ(bench.last_prs_pdus().size(), is_prs_slot ? 1U : 0U)
        << "Unexpected number of DL-PRS PDUs in slot " << bench.last_slot_count();
  }
}

TEST(prs_scheduler_test, repetitions_muted_by_muting_option2_are_not_scheduled)
{
  constexpr unsigned period      = 20;
  constexpr unsigned slot_offset = 0;
  constexpr unsigned time_gap    = 2;
  // Bit i of the pattern is repetition i, so repetitions 0 and 2 are transmitted and 1 and 3 are muted.
  constexpr uint32_t muting_bitmap = 0b0101;

  prs_resource_set res_set  = make_prs_resource_set(period, slot_offset);
  res_set.repetition_factor = prs_repetition_factor::four;
  res_set.time_gap          = prs_time_gap::two;
  res_set.muting_option2.emplace(prs_muting_option2{.muting_pattern = make_muting_pattern(4, muting_bitmap)});

  prs_config cfg;
  cfg.resource_sets.push_back(res_set);
  prs_test_bench bench{cfg};

  for (unsigned i = 0; i != 3 * period; ++i) {
    bench.run_slot();

    const unsigned phase       = bench.last_slot_count() % period;
    const unsigned rep_idx     = phase / time_gap;
    const bool     is_prs_slot = (phase % time_gap) == 0 and rep_idx < 4 and ((muting_bitmap >> rep_idx) & 1U) != 0;
    ASSERT_EQ(bench.last_prs_pdus().size(), is_prs_slot ? 1U : 0U)
        << "Unexpected number of DL-PRS PDUs in slot " << bench.last_slot_count();
  }
}

TEST(prs_scheduler_test, instances_muted_by_muting_option1_are_not_scheduled)
{
  constexpr unsigned period      = 10;
  constexpr unsigned slot_offset = 3;
  // Bit i of the pattern is resource set instance i, so only even instances are transmitted.
  constexpr unsigned pattern_size  = 2;
  constexpr uint32_t muting_bitmap = 0b01;

  prs_resource_set res_set = make_prs_resource_set(period, slot_offset);
  res_set.muting_option1.emplace(
      prs_muting_option1{.muting_pattern               = make_muting_pattern(pattern_size, muting_bitmap),
                         .muting_bit_repetition_factor = prs_muting_bit_repetition_factor::one});

  prs_config cfg;
  cfg.resource_sets.push_back(res_set);
  prs_test_bench bench{cfg};

  for (unsigned i = 0; i != 6 * period; ++i) {
    bench.run_slot();

    const unsigned instance = bench.last_slot_count() / period;
    const bool     is_prs_slot =
        (bench.last_slot_count() % period) == slot_offset and ((muting_bitmap >> (instance % pattern_size)) & 1U) != 0;
    ASSERT_EQ(bench.last_prs_pdus().size(), is_prs_slot ? 1U : 0U)
        << "Unexpected number of DL-PRS PDUs in slot " << bench.last_slot_count();
  }
}

TEST(prs_scheduler_test, resources_multiplexed_on_the_comb_are_scheduled_on_the_same_slot)
{
  constexpr unsigned period      = 10;
  constexpr unsigned slot_offset = 2;

  // Two resources on the same slot and symbols, interleaved on the frequency-domain comb.
  prs_resource_set res_set = make_prs_resource_set(period, slot_offset);
  res_set.resources.push_back(
      prs_resource{.sequence_id = sequence_id + 1, .re_offset = 1, .slot_offset = 0, .symbol_offset = symbol_offset});

  prs_config cfg;
  cfg.resource_sets.push_back(res_set);
  prs_test_bench bench{cfg};

  ASSERT_TRUE(bench.run_slot_until([&bench]() { return not bench.last_prs_pdus().empty(); }, 2 * period));

  ASSERT_EQ(bench.last_prs_pdus().size(), 2U);
  ASSERT_EQ(bench.last_prs_pdus()[0].comb_offset, 0);
  ASSERT_EQ(bench.last_prs_pdus()[1].comb_offset, 1);
  ASSERT_EQ(bench.last_prs_pdus()[0].n_id_prs, sequence_id);
  ASSERT_EQ(bench.last_prs_pdus()[1].n_id_prs, sequence_id + 1);
}

TEST(prs_scheduler_test, resources_with_different_slot_offsets_are_scheduled_on_different_slots)
{
  constexpr unsigned period          = 10;
  constexpr unsigned set_slot_offset = 1;
  constexpr unsigned res_slot_offset = 3;

  prs_resource_set res_set = make_prs_resource_set(period, set_slot_offset);
  res_set.resources.push_back(prs_resource{
      .sequence_id = sequence_id + 1, .re_offset = 0, .slot_offset = res_slot_offset, .symbol_offset = symbol_offset});

  prs_config cfg;
  cfg.resource_sets.push_back(res_set);
  prs_test_bench bench{cfg};

  for (unsigned i = 0; i != 3 * period; ++i) {
    bench.run_slot();

    const unsigned phase = bench.last_slot_count() % period;
    if (phase == set_slot_offset) {
      ASSERT_EQ(bench.last_prs_pdus().size(), 1U);
      ASSERT_EQ(bench.last_prs_pdus()[0].n_id_prs, sequence_id);
    } else if (phase == set_slot_offset + res_slot_offset) {
      ASSERT_EQ(bench.last_prs_pdus().size(), 1U);
      ASSERT_EQ(bench.last_prs_pdus()[0].n_id_prs, sequence_id + 1);
    } else {
      ASSERT_TRUE(bench.last_prs_pdus().empty()) << "Unexpected DL-PRS PDU in slot " << bench.last_slot_count();
    }
  }
}
