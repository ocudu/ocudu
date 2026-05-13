// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "lib/scheduler/common_scheduling/prach_scheduler.h"
#include "sub_scheduler_test_environment.h"
#include "tests/test_doubles/scheduler/cell_config_builder_profiles.h"
#include "tests/test_doubles/scheduler/scheduler_config_helper.h"
#include "tests/unittests/scheduler/test_utils/scheduler_test_suite.h"
#include "ocudu/ran/prach/prach_frequency_mapping.h"
#include "ocudu/ran/prach/prach_preamble_information.h"
#include "ocudu/scheduler/config/scheduler_cell_config_validator.h"
#include "ocudu/scheduler/config/scheduler_expert_config_factory.h"
#include <gtest/gtest.h>

using namespace ocudu;

namespace prach_test {

struct prach_test_params {
  subcarrier_spacing scs;
  nr_band            band;
  unsigned           prach_cfg_index;
};

// Dummy function overload of template <typename T> void testing::internal::PrintTo(const T& value, ::std::ostream* os).
// This prevents valgrind from complaining about uninitialized variables.
void PrintTo(const prach_test_params& value, ::std::ostream* os)
{
  return;
}

static sched_cell_configuration_request_message
make_custom_sched_cell_configuration_request(const prach_test_params test_params)
{
  cell_config_builder_params params = cell_config_builder_profiles::create(test_params.band);
  // We disable CSI-RS so it is simpler to enable more UL slots to test different PRACH configs.
  params.csi_rs_enabled = false;
  params.scs_common     = test_params.scs;
  if (band_helper::get_duplex_mode(test_params.band) == duplex_mode::TDD) {
    auto& tdd_cfg                              = params.tdd_ul_dl_cfg_common.emplace();
    tdd_cfg.pattern1.dl_ul_tx_period_nof_slots = 10;
    tdd_cfg.ref_scs                            = params.scs_common;
    tdd_cfg.pattern1.nof_dl_slots              = 2;
    tdd_cfg.pattern1.nof_dl_symbols            = 6;
    tdd_cfg.pattern1.nof_ul_slots              = 8;
    tdd_cfg.pattern1.nof_ul_symbols            = 0;
  }

  sched_cell_configuration_request_message sched_req =
      sched_config_helper::make_default_sched_cell_configuration_request(params);

  sched_req.ran.ul_cfg_common.init_ul_bwp.rach_cfg_common.value().rach_cfg_generic.prach_config_index =
      test_params.prach_cfg_index;

  return sched_req;
}
} // namespace prach_test

using namespace prach_test;

class prach_scheduler_test : public sub_scheduler_test_environment, public ::testing::TestWithParam<prach_test_params>
{
  // Returns the cell config for the given params if it passes scheduler validation, or the default cell config
  // otherwise. Tests with unsupported configurations are skipped in SetUp().
  static sched_cell_configuration_request_message safe_cell_config(const prach_test_params& p)
  {
    const auto msg = make_custom_sched_cell_configuration_request(p);
    if (not config_validators::validate_sched_cell_configuration_request_message(
                msg, config_helpers::make_default_scheduler_expert_config())
                .has_value()) {
      return sched_config_helper::make_default_sched_cell_configuration_request();
    }
    return msg;
  }

protected:
  prach_scheduler_test() :
    sub_scheduler_test_environment(safe_cell_config(GetParam())),
    prach_sch(cell_cfg),
    prach_cfg(prach_configuration_get(
        band_helper::get_freq_range(cell_cfg.band()),
        cell_cfg.paired_spectrum() ? duplex_mode::FDD : duplex_mode::TDD,
        cell_cfg.params.ul_cfg_common.init_ul_bwp.rach_cfg_common->rach_cfg_generic.prach_config_index)),
    prach_repetition_period(prach_cfg.x * NOF_SUBFRAMES_PER_FRAME * get_nof_slots_per_subframe(cell_cfg.scs_common()))
  {
    prach_symbols_slots_duration prach_duration_info =
        get_prach_duration_info(prach_cfg, cell_cfg.params.ul_cfg_common.init_ul_bwp.generic_params.scs);
    nof_symbols        = prach_duration_info.nof_symbols;
    prach_length_slots = prach_duration_info.prach_length_slots;
  }

  void SetUp() override
  {
    const auto msg = make_custom_sched_cell_configuration_request(GetParam());
    const auto err = config_validators::validate_sched_cell_configuration_request_message(
        msg, config_helpers::make_default_scheduler_expert_config());
    if (not err.has_value()) {
      GTEST_SKIP() << fmt::format(
          "Unsupported PRACH configuration index {} - skipping. Cause: {}", GetParam().prach_cfg_index, err.error());
    }
  }

  void do_run_slot() override { prach_sch.run_slot(res_grid); }

  bool is_prach_slot() const
  {
    const slot_point sl = res_grid[0].slot;
    bool             prach_occasion_sfn =
        std::any_of(prach_cfg.y.begin(), prach_cfg.y.end(), [&](uint8_t y) { return sl.sfn() % prach_cfg.x == y; });
    if (!prach_occasion_sfn) {
      return false;
    }

    const subcarrier_spacing scs_ref = band_helper::get_freq_range(cell_cfg.band()) == frequency_range::FR2
                                           ? subcarrier_spacing::kHz60
                                           : subcarrier_spacing::kHz15;
    const unsigned scs_ratio = pow2(to_numerology_value(cell_cfg.params.ul_cfg_common.init_ul_bwp.generic_params.scs) -
                                    to_numerology_value(scs_ref));

    if (std::find(prach_cfg.slots.begin(), prach_cfg.slots.end(), sl.slot_index() / scs_ratio) ==
        prach_cfg.slots.end()) {
      return false;
    }

    if (is_long_preamble(prach_cfg.format)) {
      // With long Format PRACH, the starting_symbol refers to the SCS 15kHz. We need to map this starting symbol into
      // the slot of the SCS used by the system to know whether this is the slot with the PRACH opportunity.
      const unsigned start_slot_offset =
          prach_cfg.starting_symbol *
          pow2(to_numerology_value(cell_cfg.params.ul_cfg_common.init_ul_bwp.generic_params.scs)) /
          NOF_OFDM_SYM_PER_SLOT_NORMAL_CP;
      if (sl.subframe_slot_index() != start_slot_offset) {
        return false;
      }
    } else {
      // With short Format PRACH, with 15kHz SCS, there is only 1 slot per subframe. This slot contrains a burst PRACH
      // occasions. With 30kHz SCS, there are 2 slots per subframe; depending on whether the "number of PRACH slots
      // within a subframe" 1 or 2 (as per Tables 6.3.3.2-2 and 6.3.3.2-3, TS 38.211), the occasions (and the
      // transmission in one of the occasions) are expected in the second slot of the subframe (if number of PRACH
      // slots within a subframe = 1) or in both slots (number of PRACH slots within a subframe = 2).
      if ((prach_cfg.nof_prach_slots_within_subframe == 1) && (scs_ratio == 2)) {
        return sl.subframe_slot_index() % 2 == 1;
      }

      if (scs_ratio == 1) {
        return sl.subframe_slot_index() % 2 == 0;
      }
    }

    return true;
  }

  grant_info get_prach_grant(const prach_occasion_info& occasion, unsigned prach_slot_idx) const
  {
    // The information we need are not related to whether it is the last PRACH occasion.
    constexpr bool                   is_last_prach_occasion = false;
    const prach_preamble_information info =
        is_long_preamble(prach_cfg.format)
            ? get_prach_preamble_long_info(prach_cfg.format)
            : get_prach_preamble_short_info(
                  prach_cfg.format,
                  to_ra_subcarrier_spacing(cell_cfg.params.ul_cfg_common.init_ul_bwp.generic_params.scs),
                  is_last_prach_occasion);
    // Compute the grant PRBs
    const unsigned prach_nof_prbs =
        prach_frequency_mapping_get(info.scs, cell_cfg.params.ul_cfg_common.init_ul_bwp.generic_params.scs).nof_rb_ra;
    const uint8_t prb_start =
        cell_cfg.params.ul_cfg_common.init_ul_bwp.rach_cfg_common->rach_cfg_generic.msg1_frequency_start +
        occasion.index_fd_ra * prach_nof_prbs;
    const prb_interval prach_prbs{prb_start, prb_start + prach_nof_prbs};
    const crb_interval crbs = prb_to_crb(cell_cfg.params.ul_cfg_common.init_ul_bwp.generic_params, prach_prbs);

    if (is_long_preamble(prach_cfg.format)) {
      // Compute the grant symbols.
      const unsigned starting_symbol_pusch_scs =
          (occasion.start_symbol *
           (1U << to_numerology_value(cell_cfg.params.ul_cfg_common.init_ul_bwp.generic_params.scs))) %
          NOF_OFDM_SYM_PER_SLOT_NORMAL_CP;
      const ofdm_symbol_range prach_symbols{prach_slot_idx == 0 ? starting_symbol_pusch_scs : 0,
                                            prach_slot_idx < prach_length_slots - 1
                                                ? NOF_OFDM_SYM_PER_SLOT_NORMAL_CP
                                                : (starting_symbol_pusch_scs + nof_symbols) %
                                                      NOF_OFDM_SYM_PER_SLOT_NORMAL_CP};

      return grant_info{cell_cfg.params.ul_cfg_common.init_ul_bwp.generic_params.scs, prach_symbols, crbs};
    }
    const unsigned          starting_symbol_pusch_scs = occasion.start_symbol;
    const ofdm_symbol_range prach_symbols{starting_symbol_pusch_scs,
                                          starting_symbol_pusch_scs +
                                              prach_cfg.duration * prach_cfg.nof_occasions_within_slot};

    return grant_info{cell_cfg.params.ul_cfg_common.init_ul_bwp.generic_params.scs, prach_symbols, crbs};
  }

  static bool
  test_res_grid_has_re_set(const cell_resource_allocator& cell_res_grid, grant_info grant, unsigned tx_delay)
  {
    return cell_res_grid[tx_delay].ul_res_grid.all_set(grant);
  }

  prach_scheduler           prach_sch;
  const prach_configuration prach_cfg;
  const unsigned            prach_repetition_period;
  // Helper variables.
  unsigned prach_length_slots{1};
  unsigned nof_symbols{0};
};

TEST_P(prach_scheduler_test, prach_sched_allocates_in_prach_configured_slots)
{
  unsigned prach_counter = 0;
  for (unsigned i = 0; i != prach_repetition_period; ++i) {
    run_slot();
    unsigned nof_prachs = res_grid[0].result.ul.prachs.size();
    if (is_prach_slot()) {
      ASSERT_GE(1, nof_prachs);
    } else {
      ASSERT_EQ(0, nof_prachs);
    }
    prach_counter += nof_prachs;
  }
  ASSERT_GT(prach_counter, 0);
}

TEST_P(prach_scheduler_test, prach_sched_allocates_in_sched_grid)
{
  for (unsigned i = 0; i != prach_repetition_period * 2; ++i) {
    run_slot();
    if (is_prach_slot()) {
      ASSERT_GE(1, res_grid[0].result.ul.prachs.size());
      for (const auto& prach_pdu : res_grid[0].result.ul.prachs) {
        // For long PRACHs, we have 1 PRACH PDU and up to 8 grid grants allocated per PRACH occasion; these are
        // allocated by the scheduler at the slot where the PRACH pramble starts.
        // For short PRACHs, we have 1 or 2 PRACH PDU and grid grant allocated per burst of PRACH occasions; the
        // occasion extents over 1 or 2 slots, each slot containing 1 PRACH PDU and 1 grid grant.
        for (unsigned prach_slot_idx = 0; prach_slot_idx < prach_length_slots; ++prach_slot_idx) {
          // Note that prach_slot_idx is only used for long PRACH. For short PRACHs, the function below return the
          // same grant regardless of the prach_slot_idx; this means the operation gets repeated twice.
          const grant_info grant = get_prach_grant(prach_pdu, prach_slot_idx);
          test_res_grid_has_re_set(res_grid, grant, 0);
        }
      }
    }
  }
}

INSTANTIATE_TEST_SUITE_P(
    prach_scheduler_fdd_15kHz,
    prach_scheduler_test,
    testing::Values(
        // Format 0.
        prach_test_params{.scs = ocudu::subcarrier_spacing::kHz15, .band = ocudu::nr_band::n3, .prach_cfg_index = 0},
        prach_test_params{.scs = ocudu::subcarrier_spacing::kHz15, .band = ocudu::nr_band::n3, .prach_cfg_index = 15},
        prach_test_params{.scs = ocudu::subcarrier_spacing::kHz15, .band = ocudu::nr_band::n3, .prach_cfg_index = 26},
        prach_test_params{.scs = ocudu::subcarrier_spacing::kHz15, .band = ocudu::nr_band::n3, .prach_cfg_index = 27},
        // Format 1.
        prach_test_params{.scs = ocudu::subcarrier_spacing::kHz15, .band = ocudu::nr_band::n3, .prach_cfg_index = 34},
        prach_test_params{.scs = ocudu::subcarrier_spacing::kHz15, .band = ocudu::nr_band::n3, .prach_cfg_index = 44},
        prach_test_params{.scs = ocudu::subcarrier_spacing::kHz15, .band = ocudu::nr_band::n3, .prach_cfg_index = 51},
        prach_test_params{.scs = ocudu::subcarrier_spacing::kHz15, .band = ocudu::nr_band::n3, .prach_cfg_index = 52},
        // Format 2.
        prach_test_params{.scs = ocudu::subcarrier_spacing::kHz15, .band = ocudu::nr_band::n3, .prach_cfg_index = 55},
        // Format 3.
        prach_test_params{.scs = ocudu::subcarrier_spacing::kHz15, .band = ocudu::nr_band::n3, .prach_cfg_index = 73},
        prach_test_params{.scs = ocudu::subcarrier_spacing::kHz15, .band = ocudu::nr_band::n3, .prach_cfg_index = 86},
        // Format A1.
        prach_test_params{.scs = ocudu::subcarrier_spacing::kHz15, .band = ocudu::nr_band::n3, .prach_cfg_index = 87},
        prach_test_params{.scs = ocudu::subcarrier_spacing::kHz15, .band = ocudu::nr_band::n3, .prach_cfg_index = 88},
        prach_test_params{.scs = ocudu::subcarrier_spacing::kHz15, .band = ocudu::nr_band::n3, .prach_cfg_index = 99},
        prach_test_params{.scs = ocudu::subcarrier_spacing::kHz15, .band = ocudu::nr_band::n3, .prach_cfg_index = 100},
        prach_test_params{.scs = ocudu::subcarrier_spacing::kHz15, .band = ocudu::nr_band::n3, .prach_cfg_index = 106},
        prach_test_params{.scs = ocudu::subcarrier_spacing::kHz15, .band = ocudu::nr_band::n3, .prach_cfg_index = 107},
        // Format A3.
        prach_test_params{.scs = ocudu::subcarrier_spacing::kHz15, .band = ocudu::nr_band::n3, .prach_cfg_index = 147},
        prach_test_params{.scs = ocudu::subcarrier_spacing::kHz15, .band = ocudu::nr_band::n3, .prach_cfg_index = 153},
        prach_test_params{.scs = ocudu::subcarrier_spacing::kHz15, .band = ocudu::nr_band::n3, .prach_cfg_index = 156},
        prach_test_params{.scs = ocudu::subcarrier_spacing::kHz15, .band = ocudu::nr_band::n3, .prach_cfg_index = 159},
        prach_test_params{.scs = ocudu::subcarrier_spacing::kHz15, .band = ocudu::nr_band::n3, .prach_cfg_index = 164},
        prach_test_params{.scs = ocudu::subcarrier_spacing::kHz15, .band = ocudu::nr_band::n3, .prach_cfg_index = 165},
        // Format B4.
        prach_test_params{.scs = ocudu::subcarrier_spacing::kHz15, .band = ocudu::nr_band::n3, .prach_cfg_index = 198},
        prach_test_params{.scs = ocudu::subcarrier_spacing::kHz15, .band = ocudu::nr_band::n3, .prach_cfg_index = 205},
        prach_test_params{.scs = ocudu::subcarrier_spacing::kHz15, .band = ocudu::nr_band::n3, .prach_cfg_index = 208},
        prach_test_params{.scs = ocudu::subcarrier_spacing::kHz15, .band = ocudu::nr_band::n3, .prach_cfg_index = 212},
        prach_test_params{.scs = ocudu::subcarrier_spacing::kHz15, .band = ocudu::nr_band::n3, .prach_cfg_index = 216},
        prach_test_params{.scs = ocudu::subcarrier_spacing::kHz15, .band = ocudu::nr_band::n3, .prach_cfg_index = 218},
        // Format C0.
        prach_test_params{.scs = ocudu::subcarrier_spacing::kHz15, .band = ocudu::nr_band::n3, .prach_cfg_index = 219},
        prach_test_params{.scs = ocudu::subcarrier_spacing::kHz15, .band = ocudu::nr_band::n3, .prach_cfg_index = 225},
        prach_test_params{.scs = ocudu::subcarrier_spacing::kHz15, .band = ocudu::nr_band::n3, .prach_cfg_index = 234},
        // Format C2.
        prach_test_params{.scs = ocudu::subcarrier_spacing::kHz15, .band = ocudu::nr_band::n3, .prach_cfg_index = 236},
        prach_test_params{.scs = ocudu::subcarrier_spacing::kHz15, .band = ocudu::nr_band::n3, .prach_cfg_index = 242},
        prach_test_params{.scs = ocudu::subcarrier_spacing::kHz15, .band = ocudu::nr_band::n3, .prach_cfg_index = 253}),
    [](const testing::TestParamInfo<prach_scheduler_test::ParamType>& info_) {
      return fmt::format("fdd_scs_{}_prach_cfg_idx_{}", to_string(info_.param.scs), info_.param.prach_cfg_index);
    });

INSTANTIATE_TEST_SUITE_P(
    prach_scheduler_fdd_30kHz,
    prach_scheduler_test,
    testing::Values(
        // Format 0.
        prach_test_params{.scs = ocudu::subcarrier_spacing::kHz30, .band = ocudu::nr_band::n3, .prach_cfg_index = 0},
        prach_test_params{.scs = ocudu::subcarrier_spacing::kHz30, .band = ocudu::nr_band::n3, .prach_cfg_index = 15},
        prach_test_params{.scs = ocudu::subcarrier_spacing::kHz30, .band = ocudu::nr_band::n3, .prach_cfg_index = 26},
        prach_test_params{.scs = ocudu::subcarrier_spacing::kHz30, .band = ocudu::nr_band::n3, .prach_cfg_index = 27},
        // Format 1.
        prach_test_params{.scs = ocudu::subcarrier_spacing::kHz30, .band = ocudu::nr_band::n3, .prach_cfg_index = 34},
        prach_test_params{.scs = ocudu::subcarrier_spacing::kHz30, .band = ocudu::nr_band::n3, .prach_cfg_index = 44},
        prach_test_params{.scs = ocudu::subcarrier_spacing::kHz30, .band = ocudu::nr_band::n3, .prach_cfg_index = 51},
        prach_test_params{.scs = ocudu::subcarrier_spacing::kHz30, .band = ocudu::nr_band::n3, .prach_cfg_index = 52},
        // Format 2.
        prach_test_params{.scs = ocudu::subcarrier_spacing::kHz30, .band = ocudu::nr_band::n3, .prach_cfg_index = 55},
        // Format 3.
        prach_test_params{.scs = ocudu::subcarrier_spacing::kHz30, .band = ocudu::nr_band::n3, .prach_cfg_index = 73},
        prach_test_params{.scs = ocudu::subcarrier_spacing::kHz30, .band = ocudu::nr_band::n3, .prach_cfg_index = 86},
        // Format A1.
        prach_test_params{.scs = ocudu::subcarrier_spacing::kHz30, .band = ocudu::nr_band::n3, .prach_cfg_index = 87},
        prach_test_params{.scs = ocudu::subcarrier_spacing::kHz30, .band = ocudu::nr_band::n3, .prach_cfg_index = 88},
        prach_test_params{.scs = ocudu::subcarrier_spacing::kHz30, .band = ocudu::nr_band::n3, .prach_cfg_index = 99},
        prach_test_params{.scs = ocudu::subcarrier_spacing::kHz30, .band = ocudu::nr_band::n3, .prach_cfg_index = 100},
        prach_test_params{.scs = ocudu::subcarrier_spacing::kHz30, .band = ocudu::nr_band::n3, .prach_cfg_index = 106},
        prach_test_params{.scs = ocudu::subcarrier_spacing::kHz30, .band = ocudu::nr_band::n3, .prach_cfg_index = 107},
        // Format A3.
        prach_test_params{.scs = ocudu::subcarrier_spacing::kHz30, .band = ocudu::nr_band::n3, .prach_cfg_index = 147},
        prach_test_params{.scs = ocudu::subcarrier_spacing::kHz30, .band = ocudu::nr_band::n3, .prach_cfg_index = 153},
        prach_test_params{.scs = ocudu::subcarrier_spacing::kHz30, .band = ocudu::nr_band::n3, .prach_cfg_index = 156},
        prach_test_params{.scs = ocudu::subcarrier_spacing::kHz30, .band = ocudu::nr_band::n3, .prach_cfg_index = 159},
        prach_test_params{.scs = ocudu::subcarrier_spacing::kHz30, .band = ocudu::nr_band::n3, .prach_cfg_index = 164},
        prach_test_params{.scs = ocudu::subcarrier_spacing::kHz30, .band = ocudu::nr_band::n3, .prach_cfg_index = 165},
        // Format B4.
        prach_test_params{.scs = ocudu::subcarrier_spacing::kHz30, .band = ocudu::nr_band::n3, .prach_cfg_index = 198},
        prach_test_params{.scs = ocudu::subcarrier_spacing::kHz30, .band = ocudu::nr_band::n3, .prach_cfg_index = 205},
        prach_test_params{.scs = ocudu::subcarrier_spacing::kHz30, .band = ocudu::nr_band::n3, .prach_cfg_index = 208},
        prach_test_params{.scs = ocudu::subcarrier_spacing::kHz30, .band = ocudu::nr_band::n3, .prach_cfg_index = 212},
        prach_test_params{.scs = ocudu::subcarrier_spacing::kHz30, .band = ocudu::nr_band::n3, .prach_cfg_index = 216},
        prach_test_params{.scs = ocudu::subcarrier_spacing::kHz30, .band = ocudu::nr_band::n3, .prach_cfg_index = 218},
        // Format C0.
        prach_test_params{.scs = ocudu::subcarrier_spacing::kHz30, .band = ocudu::nr_band::n3, .prach_cfg_index = 219},
        prach_test_params{.scs = ocudu::subcarrier_spacing::kHz30, .band = ocudu::nr_band::n3, .prach_cfg_index = 225},
        prach_test_params{.scs = ocudu::subcarrier_spacing::kHz30, .band = ocudu::nr_band::n3, .prach_cfg_index = 234},
        // Format C2.
        prach_test_params{.scs = ocudu::subcarrier_spacing::kHz30, .band = ocudu::nr_band::n3, .prach_cfg_index = 236},
        prach_test_params{.scs = ocudu::subcarrier_spacing::kHz30, .band = ocudu::nr_band::n3, .prach_cfg_index = 242},
        prach_test_params{.scs = ocudu::subcarrier_spacing::kHz30, .band = ocudu::nr_band::n3, .prach_cfg_index = 253}),
    [](const testing::TestParamInfo<prach_scheduler_test::ParamType>& info_) {
      return fmt::format("fdd_scs_{}_prach_cfg_idx_{}", to_string(info_.param.scs), info_.param.prach_cfg_index);
    });

INSTANTIATE_TEST_SUITE_P(
    prach_scheduler_tdd_15kHz,
    prach_scheduler_test,
    testing::Values(
        // Format 0.
        prach_test_params{.scs = ocudu::subcarrier_spacing::kHz15, .band = ocudu::nr_band::n41, .prach_cfg_index = 0},
        prach_test_params{.scs = ocudu::subcarrier_spacing::kHz15, .band = ocudu::nr_band::n41, .prach_cfg_index = 9},
        prach_test_params{.scs = ocudu::subcarrier_spacing::kHz15, .band = ocudu::nr_band::n41, .prach_cfg_index = 12},
        prach_test_params{.scs = ocudu::subcarrier_spacing::kHz15, .band = ocudu::nr_band::n41, .prach_cfg_index = 14},
        prach_test_params{.scs = ocudu::subcarrier_spacing::kHz15, .band = ocudu::nr_band::n41, .prach_cfg_index = 17},
        prach_test_params{.scs = ocudu::subcarrier_spacing::kHz15, .band = ocudu::nr_band::n41, .prach_cfg_index = 25},
        // Format 1.
        prach_test_params{.scs = ocudu::subcarrier_spacing::kHz15, .band = ocudu::nr_band::n41, .prach_cfg_index = 29},
        prach_test_params{.scs = ocudu::subcarrier_spacing::kHz15, .band = ocudu::nr_band::n41, .prach_cfg_index = 33},
        // Format 2.
        prach_test_params{.scs = ocudu::subcarrier_spacing::kHz15, .band = ocudu::nr_band::n41, .prach_cfg_index = 34},
        prach_test_params{.scs = ocudu::subcarrier_spacing::kHz15, .band = ocudu::nr_band::n41, .prach_cfg_index = 38},
        // Format 3.
        prach_test_params{.scs = ocudu::subcarrier_spacing::kHz15, .band = ocudu::nr_band::n41, .prach_cfg_index = 40},
        prach_test_params{.scs = ocudu::subcarrier_spacing::kHz15, .band = ocudu::nr_band::n41, .prach_cfg_index = 57},
        prach_test_params{.scs = ocudu::subcarrier_spacing::kHz15, .band = ocudu::nr_band::n41, .prach_cfg_index = 64},
        // Format A1.
        prach_test_params{.scs = ocudu::subcarrier_spacing::kHz15, .band = ocudu::nr_band::n41, .prach_cfg_index = 67},
        prach_test_params{.scs = ocudu::subcarrier_spacing::kHz15, .band = ocudu::nr_band::n41, .prach_cfg_index = 69},
        prach_test_params{.scs = ocudu::subcarrier_spacing::kHz15, .band = ocudu::nr_band::n41, .prach_cfg_index = 71},
        prach_test_params{.scs = ocudu::subcarrier_spacing::kHz15, .band = ocudu::nr_band::n41, .prach_cfg_index = 73},
        prach_test_params{.scs = ocudu::subcarrier_spacing::kHz15, .band = ocudu::nr_band::n41, .prach_cfg_index = 86},
        // Format A3.
        prach_test_params{.scs = ocudu::subcarrier_spacing::kHz15, .band = ocudu::nr_band::n41, .prach_cfg_index = 110},
        prach_test_params{.scs = ocudu::subcarrier_spacing::kHz15, .band = ocudu::nr_band::n41, .prach_cfg_index = 112},
        prach_test_params{.scs = ocudu::subcarrier_spacing::kHz15, .band = ocudu::nr_band::n41, .prach_cfg_index = 113},
        prach_test_params{.scs = ocudu::subcarrier_spacing::kHz15, .band = ocudu::nr_band::n41, .prach_cfg_index = 127},
        prach_test_params{.scs = ocudu::subcarrier_spacing::kHz15, .band = ocudu::nr_band::n41, .prach_cfg_index = 132},
        // Format B4.
        prach_test_params{.scs = ocudu::subcarrier_spacing::kHz15, .band = ocudu::nr_band::n41, .prach_cfg_index = 145},
        prach_test_params{.scs = ocudu::subcarrier_spacing::kHz15, .band = ocudu::nr_band::n41, .prach_cfg_index = 147},
        prach_test_params{.scs = ocudu::subcarrier_spacing::kHz15, .band = ocudu::nr_band::n41, .prach_cfg_index = 152},
        prach_test_params{.scs = ocudu::subcarrier_spacing::kHz15, .band = ocudu::nr_band::n41, .prach_cfg_index = 162},
        prach_test_params{.scs = ocudu::subcarrier_spacing::kHz15, .band = ocudu::nr_band::n41, .prach_cfg_index = 168},
        // Format C0.
        prach_test_params{.scs = ocudu::subcarrier_spacing::kHz15, .band = ocudu::nr_band::n41, .prach_cfg_index = 169},
        prach_test_params{.scs = ocudu::subcarrier_spacing::kHz15, .band = ocudu::nr_band::n41, .prach_cfg_index = 176},
        prach_test_params{.scs = ocudu::subcarrier_spacing::kHz15, .band = ocudu::nr_band::n41, .prach_cfg_index = 178},
        prach_test_params{.scs = ocudu::subcarrier_spacing::kHz15, .band = ocudu::nr_band::n41, .prach_cfg_index = 183},
        prach_test_params{.scs = ocudu::subcarrier_spacing::kHz15, .band = ocudu::nr_band::n41, .prach_cfg_index = 187},
        // Format C2.
        prach_test_params{.scs = ocudu::subcarrier_spacing::kHz15, .band = ocudu::nr_band::n41, .prach_cfg_index = 189},
        prach_test_params{.scs = ocudu::subcarrier_spacing::kHz15, .band = ocudu::nr_band::n41, .prach_cfg_index = 193},
        prach_test_params{.scs = ocudu::subcarrier_spacing::kHz15, .band = ocudu::nr_band::n41, .prach_cfg_index = 198},
        prach_test_params{.scs = ocudu::subcarrier_spacing::kHz15, .band = ocudu::nr_band::n41, .prach_cfg_index = 206},
        prach_test_params{.scs             = ocudu::subcarrier_spacing::kHz15,
                          .band            = ocudu::nr_band::n41,
                          .prach_cfg_index = 210}),
    [](const testing::TestParamInfo<prach_scheduler_test::ParamType>& info_) {
      return fmt::format("tdd_scs_{}_prach_cfg_idx_{}", to_string(info_.param.scs), info_.param.prach_cfg_index);
    });

INSTANTIATE_TEST_SUITE_P(
    prach_scheduler_tdd_30kHz,
    prach_scheduler_test,
    testing::Values(
        // Format 0.
        prach_test_params{.scs = ocudu::subcarrier_spacing::kHz30, .band = ocudu::nr_band::n41, .prach_cfg_index = 0},
        prach_test_params{.scs = ocudu::subcarrier_spacing::kHz30, .band = ocudu::nr_band::n41, .prach_cfg_index = 9},
        prach_test_params{.scs = ocudu::subcarrier_spacing::kHz30, .band = ocudu::nr_band::n41, .prach_cfg_index = 12},
        prach_test_params{.scs = ocudu::subcarrier_spacing::kHz30, .band = ocudu::nr_band::n41, .prach_cfg_index = 14},
        prach_test_params{.scs = ocudu::subcarrier_spacing::kHz30, .band = ocudu::nr_band::n41, .prach_cfg_index = 16},
        prach_test_params{.scs = ocudu::subcarrier_spacing::kHz30, .band = ocudu::nr_band::n41, .prach_cfg_index = 17},
        prach_test_params{.scs = ocudu::subcarrier_spacing::kHz30, .band = ocudu::nr_band::n41, .prach_cfg_index = 26},
        // Format 1.
        prach_test_params{.scs = ocudu::subcarrier_spacing::kHz30, .band = ocudu::nr_band::n41, .prach_cfg_index = 29},
        prach_test_params{.scs = ocudu::subcarrier_spacing::kHz30, .band = ocudu::nr_band::n41, .prach_cfg_index = 33},
        // Format 2.
        prach_test_params{.scs = ocudu::subcarrier_spacing::kHz30, .band = ocudu::nr_band::n41, .prach_cfg_index = 34},
        prach_test_params{.scs = ocudu::subcarrier_spacing::kHz30, .band = ocudu::nr_band::n41, .prach_cfg_index = 38},
        // Format 3.
        prach_test_params{.scs = ocudu::subcarrier_spacing::kHz30, .band = ocudu::nr_band::n41, .prach_cfg_index = 40},
        prach_test_params{.scs = ocudu::subcarrier_spacing::kHz30, .band = ocudu::nr_band::n41, .prach_cfg_index = 55},
        prach_test_params{.scs = ocudu::subcarrier_spacing::kHz30, .band = ocudu::nr_band::n41, .prach_cfg_index = 56},
        prach_test_params{.scs = ocudu::subcarrier_spacing::kHz30, .band = ocudu::nr_band::n41, .prach_cfg_index = 65},
        // Format A1.
        prach_test_params{.scs = ocudu::subcarrier_spacing::kHz30, .band = ocudu::nr_band::n41, .prach_cfg_index = 67},
        prach_test_params{.scs = ocudu::subcarrier_spacing::kHz30, .band = ocudu::nr_band::n41, .prach_cfg_index = 69},
        prach_test_params{.scs = ocudu::subcarrier_spacing::kHz30, .band = ocudu::nr_band::n41, .prach_cfg_index = 71},
        prach_test_params{.scs = ocudu::subcarrier_spacing::kHz30, .band = ocudu::nr_band::n41, .prach_cfg_index = 73},
        prach_test_params{.scs = ocudu::subcarrier_spacing::kHz30, .band = ocudu::nr_band::n41, .prach_cfg_index = 86},
        // Format A3.
        prach_test_params{.scs = ocudu::subcarrier_spacing::kHz30, .band = ocudu::nr_band::n41, .prach_cfg_index = 110},
        prach_test_params{.scs = ocudu::subcarrier_spacing::kHz30, .band = ocudu::nr_band::n41, .prach_cfg_index = 112},
        prach_test_params{.scs = ocudu::subcarrier_spacing::kHz30, .band = ocudu::nr_band::n41, .prach_cfg_index = 113},
        prach_test_params{.scs = ocudu::subcarrier_spacing::kHz30, .band = ocudu::nr_band::n41, .prach_cfg_index = 127},
        prach_test_params{.scs = ocudu::subcarrier_spacing::kHz30, .band = ocudu::nr_band::n41, .prach_cfg_index = 132},
        // Format B4.
        prach_test_params{.scs = ocudu::subcarrier_spacing::kHz30, .band = ocudu::nr_band::n41, .prach_cfg_index = 145},
        prach_test_params{.scs = ocudu::subcarrier_spacing::kHz30, .band = ocudu::nr_band::n41, .prach_cfg_index = 147},
        prach_test_params{.scs = ocudu::subcarrier_spacing::kHz30, .band = ocudu::nr_band::n41, .prach_cfg_index = 152},
        prach_test_params{.scs = ocudu::subcarrier_spacing::kHz30, .band = ocudu::nr_band::n41, .prach_cfg_index = 162},
        prach_test_params{.scs = ocudu::subcarrier_spacing::kHz30, .band = ocudu::nr_band::n41, .prach_cfg_index = 168},
        // Format C0.
        prach_test_params{.scs = ocudu::subcarrier_spacing::kHz30, .band = ocudu::nr_band::n41, .prach_cfg_index = 169},
        prach_test_params{.scs = ocudu::subcarrier_spacing::kHz30, .band = ocudu::nr_band::n41, .prach_cfg_index = 176},
        prach_test_params{.scs = ocudu::subcarrier_spacing::kHz30, .band = ocudu::nr_band::n41, .prach_cfg_index = 178},
        prach_test_params{.scs = ocudu::subcarrier_spacing::kHz30, .band = ocudu::nr_band::n41, .prach_cfg_index = 183},
        prach_test_params{.scs = ocudu::subcarrier_spacing::kHz30, .band = ocudu::nr_band::n41, .prach_cfg_index = 187},
        // Format C2.
        prach_test_params{.scs = ocudu::subcarrier_spacing::kHz30, .band = ocudu::nr_band::n41, .prach_cfg_index = 189},
        prach_test_params{.scs = ocudu::subcarrier_spacing::kHz30, .band = ocudu::nr_band::n41, .prach_cfg_index = 193},
        prach_test_params{.scs = ocudu::subcarrier_spacing::kHz30, .band = ocudu::nr_band::n41, .prach_cfg_index = 198},
        prach_test_params{.scs = ocudu::subcarrier_spacing::kHz30, .band = ocudu::nr_band::n41, .prach_cfg_index = 206},
        prach_test_params{.scs             = ocudu::subcarrier_spacing::kHz30,
                          .band            = ocudu::nr_band::n41,
                          .prach_cfg_index = 210}),
    [](const testing::TestParamInfo<prach_scheduler_test::ParamType>& info_) {
      return fmt::format("tdd_scs_{}_prach_cfg_idx_{}", to_string(info_.param.scs), info_.param.prach_cfg_index);
    });

INSTANTIATE_TEST_SUITE_P(
    prach_scheduler_tdd_120kHz_fr2,
    prach_scheduler_test,
    testing::Values(
        // Format A1.
        prach_test_params{.scs = ocudu::subcarrier_spacing::kHz120, .band = ocudu::nr_band::n261, .prach_cfg_index = 1},
        prach_test_params{.scs = ocudu::subcarrier_spacing::kHz120, .band = ocudu::nr_band::n261, .prach_cfg_index = 6},
        prach_test_params{.scs             = ocudu::subcarrier_spacing::kHz120,
                          .band            = ocudu::nr_band::n261,
                          .prach_cfg_index = 11},
        prach_test_params{.scs             = ocudu::subcarrier_spacing::kHz120,
                          .band            = ocudu::nr_band::n261,
                          .prach_cfg_index = 20},
        prach_test_params{.scs             = ocudu::subcarrier_spacing::kHz120,
                          .band            = ocudu::nr_band::n261,
                          .prach_cfg_index = 27},
        // Format A2.
        prach_test_params{.scs             = ocudu::subcarrier_spacing::kHz120,
                          .band            = ocudu::nr_band::n261,
                          .prach_cfg_index = 32},
        prach_test_params{.scs             = ocudu::subcarrier_spacing::kHz120,
                          .band            = ocudu::nr_band::n261,
                          .prach_cfg_index = 39},
        prach_test_params{.scs             = ocudu::subcarrier_spacing::kHz120,
                          .band            = ocudu::nr_band::n261,
                          .prach_cfg_index = 45},
        prach_test_params{.scs             = ocudu::subcarrier_spacing::kHz120,
                          .band            = ocudu::nr_band::n261,
                          .prach_cfg_index = 53},
        prach_test_params{.scs             = ocudu::subcarrier_spacing::kHz120,
                          .band            = ocudu::nr_band::n261,
                          .prach_cfg_index = 58},
        // Format B4.
        prach_test_params{.scs             = ocudu::subcarrier_spacing::kHz120,
                          .band            = ocudu::nr_band::n261,
                          .prach_cfg_index = 115},
        prach_test_params{.scs             = ocudu::subcarrier_spacing::kHz120,
                          .band            = ocudu::nr_band::n261,
                          .prach_cfg_index = 121},
        prach_test_params{.scs             = ocudu::subcarrier_spacing::kHz120,
                          .band            = ocudu::nr_band::n261,
                          .prach_cfg_index = 132},
        prach_test_params{.scs             = ocudu::subcarrier_spacing::kHz120,
                          .band            = ocudu::nr_band::n261,
                          .prach_cfg_index = 137},
        prach_test_params{.scs             = ocudu::subcarrier_spacing::kHz120,
                          .band            = ocudu::nr_band::n261,
                          .prach_cfg_index = 143},
        // Format C0.
        prach_test_params{.scs             = ocudu::subcarrier_spacing::kHz120,
                          .band            = ocudu::nr_band::n261,
                          .prach_cfg_index = 145},
        prach_test_params{.scs             = ocudu::subcarrier_spacing::kHz120,
                          .band            = ocudu::nr_band::n261,
                          .prach_cfg_index = 151},
        prach_test_params{.scs             = ocudu::subcarrier_spacing::kHz120,
                          .band            = ocudu::nr_band::n261,
                          .prach_cfg_index = 162},
        prach_test_params{.scs             = ocudu::subcarrier_spacing::kHz120,
                          .band            = ocudu::nr_band::n261,
                          .prach_cfg_index = 167},
        prach_test_params{.scs             = ocudu::subcarrier_spacing::kHz120,
                          .band            = ocudu::nr_band::n261,
                          .prach_cfg_index = 171}),
    [](const testing::TestParamInfo<prach_scheduler_test::ParamType>& info_) {
      return fmt::format("tdd_fr2_scs_{}_prach_cfg_idx_{}", to_string(info_.param.scs), info_.param.prach_cfg_index);
    });
