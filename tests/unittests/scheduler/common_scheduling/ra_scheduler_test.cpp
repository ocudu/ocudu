// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "lib/scheduler/common_scheduling/csi_rs_scheduler.h"
#include "lib/scheduler/common_scheduling/ra_scheduler.h"
#include "lib/scheduler/common_scheduling/sib1_scheduler.h"
#include "sub_scheduler_test_environment.h"
#include "tests/test_doubles/scheduler/cell_config_builder_profiles.h"
#include "tests/test_doubles/scheduler/scheduler_config_helper.h"
#include "tests/test_doubles/scheduler/scheduler_result_finder.h"
#include "tests/test_doubles/utils/test_rng.h"
#include "tests/unittests/scheduler/test_utils/dummy_test_components.h"
#include "tests/unittests/scheduler/test_utils/indication_generators.h"
#include "tests/unittests/scheduler/test_utils/scheduler_test_suite.h"
#include "ocudu/ran/prach/prach_time_mapping.h"
#include "ocudu/ran/prach/ra_helper.h"
#include "ocudu/scheduler/config/time_domain_resource_helper.h"
#include <algorithm>
#include <gtest/gtest.h>

using namespace ocudu;
using namespace cell_config_builder_profiles;

namespace {

class ra_scheduler_setup : public sub_scheduler_test_environment
{
  static constexpr unsigned CRNTI_RANGE = to_value(rnti_t::MAX_CRNTI) - to_value(rnti_t::MIN_CRNTI);

public:
  ra_scheduler_setup(const sched_cell_configuration_request_message& req, bool sched_csi, bool sched_sib1) :
    ra_scheduler_setup({}, req, sched_csi, sched_sib1)
  {
  }
  ra_scheduler_setup(const scheduler_expert_config&                  sched_cfg_,
                     const sched_cell_configuration_request_message& req,
                     bool                                            sched_csi,
                     bool                                            sched_sib1) :
    sub_scheduler_test_environment(sched_cfg_, req)
  {
    if (sched_csi) {
      csi_rs_sch.emplace(cell_cfg);
    }
    if (sched_sib1) {
      sib1_sch.emplace(cell_cfg, *pdcch_alloc, units::bytes{108});
    }

    rnti_count = test_rng::uniform_int<unsigned>(0, CRNTI_RANGE);
    rnti_inc   = test_rng::uniform_int<unsigned>(1, 5);

    // Run slot once so that the resource grid gets initialized with the initial slot.
    this->run_slot();
    for (unsigned i = 0; i != res_grid.max_dl_slot_alloc_delay - 1; ++i) {
      if (csi_rs_sch.has_value()) {
        csi_rs_sch->run_slot(res_grid[i]);
      }
      if (sib1_sch.has_value()) {
        sib1_sch->run_slot(res_grid[i]);
      }
    }
  }
  ~ra_scheduler_setup() override { this->flush_events(); }

  void do_run_slot() override
  {
    if (csi_rs_sch.has_value()) {
      csi_rs_sch->run_slot(res_grid[res_grid.max_dl_slot_alloc_delay]);
    }
    if (sib1_sch.has_value()) {
      sib1_sch->run_slot(res_grid[res_grid.max_dl_slot_alloc_delay]);
    }
    ra_sch.run_slot(res_grid);
    ASSERT_NO_FATAL_FAILURE(tracker.on_new_result(res_grid[0].slot, res_grid[0].result));
  }

  void handle_rach_indication(rach_indication_message ind)
  {
    // Advance the simulator to a slot that has a valid PRACH occasion. The ra_scheduler only
    // prereserves MsgA PUSCH for slots whose corresponding PRACH slot is a valid occasion per
    // the configured PRACH config index; injecting a PRACH at an arbitrary slot would miss the
    // prereservation and fail the sanity check in handle_msga_occasion.
    const prach_helper::preamble_slot_mapping prach_mapper{
        cell_cfg.band(),
        cell_cfg.init_bwp.ul.cfg().scs,
        cell_cfg.init_bwp.ul.rach_common()->rach_cfg_generic.prach_config_index};
    run_slot_until([this, &prach_mapper]() { return prach_mapper.has_prach_occasion(next_slot_rx()); });
    ind.slot_rx = next_slot_rx();
    ra_sch.handle_rach_indication(ind);
    tracker.on_new_rach_ind(ind);
  }

  void handle_crc_indication(const ul_crc_indication& crc)
  {
    ra_sch.handle_crc_indication(crc);
    tracker.on_crc_indication(crc);
  }

  rach_indication_message::preamble create_random_preamble()
  {
    const auto next_rnti = rnti_count + to_value(rnti_t::MIN_CRNTI);

    rach_indication_message::preamble preamble =
        test_helper::create_preamble(test_rng::uniform_int<unsigned>(0, 63), to_rnti(next_rnti));
    preamble.time_advance =
        phy_time_unit::from_seconds(std::uniform_real_distribution<double>{0, 2005e-6}(test_rng::tls_gen()));

    rnti_count = (rnti_count + rnti_inc) % CRNTI_RANGE;
    return preamble;
  }

  rach_indication_message create_rach_indication(unsigned nof_preambles)
  {
    std::vector<rach_indication_message::preamble> preambles;
    for (unsigned i = 0; i != nof_preambles; ++i) {
      preambles.push_back(create_random_preamble());
    }
    return test_helper::create_rach_indication(next_slot_rx(), preambles);
  }

  bool grants_scheduled_in_next_slots(unsigned nof_slots_to_check)
  {
    for (unsigned i = 0; i != nof_slots_to_check; ++i) {
      this->run_slot();

      auto& dl_res = res_grid[0].result.dl;
      auto& ul_res = res_grid[0].result.ul;

      const bool has_ra_dl_pdcch =
          std::any_of(dl_res.dl_pdcchs.begin(), dl_res.dl_pdcchs.end(), [](const pdcch_dl_information& pdcch) {
            return pdcch.dci.type() == dci_dl_rnti_config_type::ra_f1_0;
          });
      const bool has_msg3_retx_pdcch =
          std::any_of(dl_res.ul_pdcchs.begin(), dl_res.ul_pdcchs.end(), [](const pdcch_ul_information& pdcch) {
            return pdcch.dci.type() == dci_ul_rnti_config_type::tc_rnti_f0_0;
          });
      const bool has_msg3_pusch =
          std::any_of(ul_res.puschs.begin(), ul_res.puschs.end(), [](const ul_sched_info& pusch) {
            return pusch.context.ue_index == INVALID_DU_UE_INDEX;
          });

      if (not dl_res.rar_grants.empty() or has_ra_dl_pdcch or has_msg3_retx_pdcch or has_msg3_pusch) {
        return true;
      }
    }
    return false;
  }

  void handle_crc_for_pending_puschs(bool crc)
  {
    const auto& puschs = this->res_grid[0].result.ul.puschs;
    for (const ul_sched_info& pusch : puschs) {
      if (pusch.context.ue_index == INVALID_DU_UE_INDEX) {
        // Msg3 PUSCH. "rapid" is a MsgA-only field; if set here, the PHY will wrongly apply the MsgA
        // descrambling sequence (TS38.211 6.3.1.1) to a regular Msg3 PUSCH and fail to decode it.
        ASSERT_FALSE(pusch.context.rapid.has_value()) << "Msg3 PUSCH context.rapid must not be set";
      }
    }
    if (not puschs.empty()) {
      handle_crc_indication(test_helper::create_crc_indication(this->res_grid[0].slot, puschs, crc));
    }
  }

  ra_scheduler                      ra_sch{cell_cfg, *pdcch_alloc, pucch_alloc, ev_logger, metrics_hdlr};
  std::optional<csi_rs_scheduler>   csi_rs_sch;
  std::optional<sib1_scheduler>     sib1_sch;
  test_helper::ra_scheduler_tracker tracker{cell_cfg};
  unsigned                          rnti_count = 0;
  unsigned                          rnti_inc   = 1;
};

struct test_params {
  frequency_range                        fr;
  unsigned                               min_k;
  std::optional<tdd_ul_dl_config_common> tdd_cfg;
  bool                                   sched_csi_rs = false;
  bool                                   sched_sib1   = false;
};

void PrintTo(const test_params& p, std::ostream* os)
{
  *os << "fr=" << to_string(p.fr) << " min_k=" << p.min_k << " tdd=" << (p.tdd_cfg.has_value() ? "yes" : "no")
      << " csi_rs=" << p.sched_csi_rs << " sib1=" << p.sched_sib1;
}

/// Test suite common to different FRs, duplex modes, k values.
class ra_scheduler_common_test : public ra_scheduler_setup, public ::testing::TestWithParam<test_params>
{
public:
  ra_scheduler_common_test() :
    ra_scheduler_setup(get_sched_req(GetParam()), GetParam().sched_csi_rs, GetParam().sched_sib1)
  {
  }

  static sched_cell_configuration_request_message get_sched_req(const test_params& t_params)
  {
    cell_config_builder_params builder_params =
        create(t_params.tdd_cfg.has_value() ? duplex_mode::TDD : duplex_mode::FDD, t_params.fr);
    builder_params.min_k2               = t_params.min_k;
    builder_params.min_k1               = t_params.min_k;
    builder_params.tdd_ul_dl_cfg_common = t_params.tdd_cfg;
    return sched_config_helper::make_default_sched_cell_configuration_request(builder_params);
  }
};

/// This test verifies that the cell resource grid remains empty when no RACH indications arrive to the RA scheduler.
TEST_P(ra_scheduler_common_test, when_no_rach_indication_received_then_no_rar_allocated)
{
  ASSERT_FALSE(grants_scheduled_in_next_slots(10));
}

TEST_P(ra_scheduler_common_test,
       when_rach_indication_with_single_preamble_received_then_one_rar_and_one_msg3_are_allocated)
{
  // Forward single RACH occasion with multiple preambles.
  rach_indication_message one_rach = create_rach_indication(1);
  handle_rach_indication(one_rach);

  for (unsigned slot_count = 0, max_slot_count = 1000; slot_count < max_slot_count and tracker.nof_msg3_acked() == 0;
       ++slot_count) {
    run_slot();

    ASSERT_EQ(this->res_grid[0].result.dl.ue_grants.size(), 0);
    ASSERT_EQ(this->res_grid[0].result.dl.ul_pdcchs.size(), 0);
    handle_crc_for_pending_puschs(true);
  }

  ASSERT_FALSE(grants_scheduled_in_next_slots(10));
  ASSERT_EQ(tracker.nof_ra_dl_pdcchs(), 1);
  ASSERT_EQ(tracker.nof_rars(), 1);
  ASSERT_EQ(tracker.nof_msg3_acked(), 1);
}

/// This test verifies the correct scheduling of a RAR and Msg3 when multiple RACH Preambles are received, all in a
/// single RACH occasion.
/// The scheduler is expected to allocate one or more RARs (all with the same RA-RNTI), and containing one or multiple
/// Msg3s.
TEST_P(ra_scheduler_common_test,
       when_rach_indication_with_multiple_preambles_received_then_multiple_msg3s_are_allocated)
{
  // Forward single RACH occasion with multiple preambles.
  const unsigned          nof_preambles = test_rng::uniform_int<unsigned>(2, 8);
  rach_indication_message one_rach      = create_rach_indication(nof_preambles);
  handle_rach_indication(one_rach);

  unsigned         max_msg3_per_rar = 0;
  std::set<rnti_t> ra_rntis;
  for (unsigned slot_count = 0, max_slots = 1000; slot_count != max_slots and tracker.nof_msg3_acked() < nof_preambles;
       ++slot_count) {
    run_slot();

    const auto& rars = this->res_grid[0].result.dl.rar_grants;
    for (const rar_information& rar : rars) {
      ra_rntis.emplace(rar.pdsch_cfg.rnti);
      max_msg3_per_rar = std::max<unsigned>(max_msg3_per_rar, rar.grants.size());
    }

    handle_crc_for_pending_puschs(true);
  }

  ASSERT_FALSE(grants_scheduled_in_next_slots(10));
  ASSERT_EQ(ra_rntis.size(), 1) << "Only one RA-RNTI was expected";
  ASSERT_GT(max_msg3_per_rar, 1) << "The RA scheduler should try to schedule multiple Msg3s per RAR";
  ASSERT_EQ(tracker.nof_ra_dl_pdcchs(), tracker.nof_rars());
  ASSERT_LE(tracker.nof_rars(), one_rach.occasions[0].preambles.size());
  ASSERT_EQ(tracker.nof_msg3_newtxs(), one_rach.occasions[0].preambles.size());
  ASSERT_EQ(tracker.nof_msg3_acked(), tracker.nof_msg3_newtxs());
}

/// This test verifies the correct scheduling of a RAR and Msg3 when multiple RACH Preambles are received, each in a
/// different PRACH occasion.
/// The scheduler is expected to allocate several RARs (with different RA-RNTIs), each composed by one Msg3.
TEST_P(ra_scheduler_common_test, when_rach_indication_with_multiple_occasions_received_then_multiple_rars_are_allocated)
{
  auto                    nof_occasions = test_rng::uniform_int<unsigned>(1, MAX_PRACH_OCCASIONS_PER_SLOT);
  rach_indication_message rach_ind      = create_rach_indication(0);
  for (unsigned i = 0; i != nof_occasions; ++i) {
    rach_ind.occasions.emplace_back();
    rach_ind.occasions.back().start_symbol    = 0;
    rach_ind.occasions.back().frequency_index = i;
    rach_ind.occasions.back().preambles.emplace_back(create_random_preamble());
  }
  handle_rach_indication(rach_ind);

  for (unsigned slot_count = 0, max_slots = 1000; slot_count != max_slots and tracker.nof_msg3_acked() < nof_occasions;
       ++slot_count) {
    run_slot();

    handle_crc_for_pending_puschs(true);
  }

  ASSERT_FALSE(grants_scheduled_in_next_slots(10));
  ASSERT_EQ(tracker.nof_ra_dl_pdcchs(), tracker.nof_rars());
  ASSERT_LE(tracker.nof_rars(), nof_occasions);
  ASSERT_EQ(tracker.nof_msg3_newtxs(), tracker.nof_rars());
  ASSERT_EQ(tracker.nof_msg3_acked(), tracker.nof_msg3_newtxs());
}

TEST_P(ra_scheduler_common_test, when_crc_is_ko_then_msg3_retx_is_scheduled)
{
  // Forward single RACH occasion with multiple preambles.
  const unsigned          nof_preambles = test_rng::uniform_int<unsigned>(2, 8);
  rach_indication_message one_rach      = create_rach_indication(nof_preambles);
  handle_rach_indication(one_rach);

  // Run until Msg3s get NACKed.
  for (unsigned slot_count = 0, max_slots = 1000; slot_count != max_slots and tracker.nof_msg3_newtxs() < nof_preambles;
       ++slot_count) {
    run_slot();
    handle_crc_for_pending_puschs(false);
  }

  // ACK following Msg3s.
  ASSERT_EQ(tracker.nof_msg3_acked(), 0);
  for (unsigned slot_count = 0, max_slots = 1000; slot_count != max_slots and tracker.nof_msg3_acked() < nof_preambles;
       ++slot_count) {
    run_slot();
    handle_crc_for_pending_puschs(true);
  }

  ASSERT_FALSE(grants_scheduled_in_next_slots(20));
  ASSERT_EQ(tracker.nof_msg3_acked(), nof_preambles);
  ASSERT_EQ(tracker.nof_msg3_newtxs(), nof_preambles);
  ASSERT_GE(tracker.nof_msg3_retxs(), nof_preambles);
}

using tdd_fr1_30khz = tdd_pattern_profile_fr1_30khz;
INSTANTIATE_TEST_SUITE_P(
    ra_scheduler,
    ra_scheduler_common_test,
    ::testing::Values(
        // FR1, FDD.
        test_params{frequency_range::FR1, 2},
        test_params{frequency_range::FR1, 4},
        test_params{frequency_range::FR1, 4, std::nullopt, true},
        // FR1, TDD.
        test_params{frequency_range::FR1, 2, create_tdd_pattern(tdd_fr1_30khz::DDDDDDDSUU)},
        test_params{frequency_range::FR1, 4, create_tdd_pattern(tdd_fr1_30khz::DDDDDDDSUU)},
        test_params{frequency_range::FR1, 2, create_tdd_pattern(tdd_fr1_30khz::DDDSU)},
        test_params{frequency_range::FR1, 1, create_tdd_pattern(tdd_fr1_30khz::DSUU)},
        test_params{frequency_range::FR1, 2, create_tdd_pattern(tdd_fr1_30khz::DDDDDDDSUU), true},
        test_params{frequency_range::FR1, 2, create_tdd_pattern(tdd_fr1_30khz::DDDDDDDSUU), true, true},
        test_params{frequency_range::FR1, 2, create_tdd_pattern(tdd_pattern_profile_fr1_30khz::DDDSU), true, true},
        // FR2, TDD.
        test_params{frequency_range::FR2, 1, create_tdd_pattern(tdd_pattern_profile_fr2_120khz::DDDSU)},
        test_params{frequency_range::FR2, 1, create_tdd_pattern(tdd_pattern_profile_fr2_120khz::DDDSU), true}));

class ra_scheduler_failed_rar_test : public ra_scheduler_setup, public ::testing::TestWithParam<test_params>
{
public:
  ra_scheduler_failed_rar_test() :
    ra_scheduler_setup(get_sched_req(GetParam()), GetParam().sched_csi_rs, GetParam().sched_sib1)
  {
  }

  static sched_cell_configuration_request_message get_sched_req(const test_params& t_params)
  {
    cell_config_builder_params builder_params =
        create(t_params.tdd_cfg.has_value() ? duplex_mode::TDD : duplex_mode::FDD, t_params.fr);
    builder_params.min_k2               = t_params.min_k;
    builder_params.min_k1               = t_params.min_k;
    builder_params.tdd_ul_dl_cfg_common = t_params.tdd_cfg;
    return sched_config_helper::make_default_sched_cell_configuration_request(builder_params);
  }

  void fill_dl_rbs(unsigned lookahead_slots)
  {
    if (cell_cfg.is_dl_enabled(res_grid[lookahead_slots].slot)) {
      const grant_info grant{
          cell_cfg.scs_common(),
          ofdm_symbol_range{cell_cfg.params.dl_cfg_common.init_dl_bwp.pdcch_common.coreset0->duration(),
                            NOF_OFDM_SYM_PER_SLOT_NORMAL_CP},
          crb_interval{0, cell_cfg.nof_dl_prbs}};
      res_grid[lookahead_slots].dl_res_grid.fill(grant);
    }
  }
};

TEST_P(ra_scheduler_failed_rar_test, when_no_dl_rbs_available_then_rar_is_not_scheduled)
{
  // Enqueue RACH indication.
  handle_rach_indication(create_rach_indication(test_rng::uniform_int<unsigned>(1, MAX_PREAMBLES_PER_PRACH_OCCASION)));

  // Mark all DL RBs of the first lookahead_dl_rbs slots as busy.
  const unsigned lookahead_dl_rbs = 16;
  for (unsigned i = 0; i != lookahead_dl_rbs; ++i) {
    fill_dl_rbs(i);
  }

  // Keep marking DL RBs as busy until RAR expiry.
  for (unsigned i = 0, nof_slots = 100; i != nof_slots and tracker.has_pending_ra(); ++i) {
    fill_dl_rbs(lookahead_dl_rbs);
    run_slot();
  }

  ASSERT_FALSE(grants_scheduled_in_next_slots(lookahead_dl_rbs * 2))
      << "After RAR window expiry, nothing should be scheduled";
  ASSERT_EQ(tracker.nof_ra_dl_pdcchs(), 0);
  ASSERT_EQ(tracker.nof_rars(), 0);
}

TEST_P(ra_scheduler_failed_rar_test, when_no_ul_rbs_available_then_rar_is_not_scheduled)
{
  // Enqueue RACH indication.
  handle_rach_indication(create_rach_indication(test_rng::uniform_int<unsigned>(1, MAX_PREAMBLES_PER_PRACH_OCCASION)));

  // Mark all UL RBs of the first lookahead_ul_rbs slots as busy.
  const unsigned   lookahead_ul_rbs = res_grid.max_ul_slot_alloc_delay;
  const grant_info marked_res{cell_cfg.scs_common(),
                              ofdm_symbol_range{0, NOF_OFDM_SYM_PER_SLOT_NORMAL_CP},
                              crb_interval{0, cell_cfg.nof_ul_prbs}};
  for (unsigned i = 0; i != lookahead_ul_rbs; ++i) {
    if (cell_cfg.is_ul_enabled(res_grid[i].slot)) {
      res_grid[i].ul_res_grid.fill(marked_res);
    }
  }

  // Keep marking UL RBs as busy until RAR expiry.
  for (unsigned i = 0, nof_slots = 100; i != nof_slots and tracker.has_pending_ra(); ++i) {
    if (cell_cfg.is_ul_enabled(res_grid[lookahead_ul_rbs].slot)) {
      res_grid[lookahead_ul_rbs].ul_res_grid.fill(marked_res);
    }
    run_slot();
  }

  ASSERT_FALSE(grants_scheduled_in_next_slots(lookahead_ul_rbs * 2))
      << "After RAR window expiry, nothing should be scheduled";
  ASSERT_EQ(tracker.nof_ra_dl_pdcchs(), 0);
  ASSERT_EQ(tracker.nof_rars(), 0);
}

TEST_P(ra_scheduler_failed_rar_test, when_rbs_available_then_new_rars_can_be_scheduled)
{
  // Enqueue first RACH (which is going to fail to be allocated).
  auto first_rach = create_rach_indication(test_rng::uniform_int<unsigned>(1, MAX_PREAMBLES_PER_PRACH_OCCASION));
  handle_rach_indication(first_rach);

  // Let RAR window expire.
  const unsigned lookahead_dl_rbs = 16;
  for (unsigned i = 0; i != lookahead_dl_rbs; ++i) {
    fill_dl_rbs(i);
  }
  for (unsigned i = 0, nof_slots = 100; i != nof_slots and tracker.has_pending_ra(); ++i) {
    fill_dl_rbs(lookahead_dl_rbs);
    run_slot();
  }
  ASSERT_FALSE(grants_scheduled_in_next_slots(lookahead_dl_rbs));
  ASSERT_EQ(tracker.nof_rars(), 0);

  // New RACH is scheduled, but this time it should be possible to schedule new RARs.
  auto second_rach = create_rach_indication(test_rng::uniform_int<unsigned>(1, 8));
  handle_rach_indication(second_rach);
  for (unsigned i = 0, nof_slots = 100; i != nof_slots and tracker.has_pending_ra(); ++i) {
    run_slot();
    handle_crc_for_pending_puschs(true);
  }
  ASSERT_GT(tracker.nof_rars(), 0);
  ASSERT_EQ(tracker.nof_rars(), tracker.nof_ra_dl_pdcchs());
  ASSERT_EQ(tracker.nof_msg3_newtxs(), second_rach.occasions[0].preambles.size());
  ASSERT_EQ(tracker.nof_msg3_newtxs(), tracker.nof_msg3_acked());
}

INSTANTIATE_TEST_SUITE_P(ra_scheduler,
                         ra_scheduler_failed_rar_test,
                         ::testing::Values(
                             // FR1, FDD.
                             test_params{frequency_range::FR1, 2},
                             // FR1, TDD.
                             test_params{frequency_range::FR1, 2, create_tdd_pattern(tdd_fr1_30khz::DDDDDDDSUU)},
                             test_params{frequency_range::FR1, 2, create_tdd_pattern(tdd_fr1_30khz::DDDSU)}));

/// Test class for congestion-control-driven Backoff Indicator (BI) scheduling. Preamble SNR/count thresholds are set
/// via the scheduler_ra_expert_config passed to the constructor.
class ra_scheduler_backoff_indicator_test : public ra_scheduler_setup, public ::testing::Test
{
public:
  ra_scheduler_backoff_indicator_test(std::optional<float> snr_threshold_dB,
                                      unsigned             max_preambles = MAX_PREAMBLES_PER_PRACH_OCCASION,
                                      unsigned             duration_ms   = 40) :
    ra_scheduler_setup(make_sched_cfg(snr_threshold_dB, max_preambles, duration_ms), get_sched_req(), false, false)
  {
  }

  static scheduler_expert_config
  make_sched_cfg(std::optional<float> snr_threshold_dB, unsigned max_preambles, unsigned duration_ms)
  {
    scheduler_expert_config cfg               = config_helpers::make_default_scheduler_expert_config();
    cfg.ra.backoff_indicator_snr_threshold_dB = snr_threshold_dB;
    cfg.ra.backoff_indicator_max_preambles    = max_preambles;
    cfg.ra.backoff_indicator_duration         = std::chrono::milliseconds{duration_ms};
    return cfg;
  }

  static sched_cell_configuration_request_message get_sched_req()
  {
    cell_config_builder_params builder_params = create(duplex_mode::FDD, frequency_range::FR1);
    builder_params.min_k1                     = 2;
    builder_params.min_k2                     = 2;
    return sched_config_helper::make_default_sched_cell_configuration_request(builder_params);
  }

  /// Creates a random preamble with an explicit SNR value.
  rach_indication_message::preamble create_preamble_with_snr(float snr_dB)
  {
    rach_indication_message::preamble p = create_random_preamble();
    p.snr_dB                            = snr_dB;
    return p;
  }
};

class ra_scheduler_snr_backoff_test : public ra_scheduler_backoff_indicator_test
{
public:
  ra_scheduler_snr_backoff_test() : ra_scheduler_backoff_indicator_test(/* snr_threshold_dB */ -5.0F) {}
};

class ra_scheduler_count_backoff_test : public ra_scheduler_backoff_indicator_test
{
public:
  ra_scheduler_count_backoff_test() :
    ra_scheduler_backoff_indicator_test(/* snr_threshold_dB */ std::nullopt, /* max_preambles */ 2U)
  {
  }
};

class ra_scheduler_backoff_only_test : public ra_scheduler_backoff_indicator_test
{
public:
  ra_scheduler_backoff_only_test() : ra_scheduler_backoff_indicator_test(/* snr_threshold_dB */ -5.0F) {}
};

class ra_scheduler_backoff_duration_test : public ra_scheduler_backoff_indicator_test
{
public:
  ra_scheduler_backoff_duration_test() :
    ra_scheduler_backoff_indicator_test(
        /* snr_threshold_dB */ -5.0F,
        /* max_preambles */ MAX_PREAMBLES_PER_PRACH_OCCASION,
        /* duration_ms */ 80U)
  {
  }
};

/// This test verifies that a preamble with SNR below the configured threshold is excluded from the RAR/Msg3 grant,
/// while a preamble with SNR above the threshold is still granted, and that the RAR carries a Backoff Indicator.
TEST_F(ra_scheduler_snr_backoff_test, weak_preamble_excluded_and_backoff_indicator_set)
{
  rach_indication_message::preamble strong = create_preamble_with_snr(0.0F);
  rach_indication_message::preamble weak   = create_preamble_with_snr(-10.0F);

  handle_rach_indication(test_helper::create_rach_indication(next_slot_rx(), {strong, weak}));

  const bool found = run_slot_until([this]() { return not res_grid[0].result.dl.rar_grants.empty(); });
  ASSERT_TRUE(found);

  const rar_information& rar = res_grid[0].result.dl.rar_grants.front();
  ASSERT_TRUE(rar.backoff_indicator.has_value());
  ASSERT_EQ(rar.grants.size(), 1U);
  EXPECT_EQ(rar.grants.front().temp_crnti, strong.tc_rnti);
}

/// This test verifies that, when more preambles are detected in an occasion than the configured maximum, only the
/// strongest-SNR preambles are granted a Msg3, and that the RAR carries a Backoff Indicator.
TEST_F(ra_scheduler_count_backoff_test, excess_preambles_excluded_and_backoff_indicator_set)
{
  const float                                    snrs[] = {-2.0F, 10.0F, 3.0F, -8.0F};
  std::vector<rach_indication_message::preamble> preambles;
  for (float snr : snrs) {
    preambles.push_back(create_preamble_with_snr(snr));
  }

  handle_rach_indication(test_helper::create_rach_indication(next_slot_rx(), preambles));

  bool bi_seen = false;
  for (unsigned slot_count = 0, max_slots = 1000; slot_count != max_slots and tracker.has_pending_ra(); ++slot_count) {
    run_slot();
    for (const rar_information& rar : res_grid[0].result.dl.rar_grants) {
      bi_seen |= rar.backoff_indicator.has_value();
    }
    handle_crc_for_pending_puschs(true);
  }

  EXPECT_TRUE(bi_seen);
  ASSERT_EQ(tracker.nof_msg3_newtxs(), 2U) << "Only the 2 strongest preambles should have been granted a Msg3";
}

/// This test verifies that, when every detected preamble in an occasion is excluded by congestion control, the RA
/// scheduler still transmits a RAR containing only a Backoff Indicator subheader (no RAPID subPDUs), and no Msg3 is
/// granted.
TEST_F(ra_scheduler_backoff_only_test, all_preambles_below_threshold_yields_backoff_only_rar)
{
  rach_indication_message::preamble weak1 = create_preamble_with_snr(-10.0F);
  rach_indication_message::preamble weak2 = create_preamble_with_snr(-20.0F);

  handle_rach_indication(test_helper::create_rach_indication(next_slot_rx(), {weak1, weak2}));

  const rar_information* backoff_rar = nullptr;
  const bool             found       = run_slot_until([this, &backoff_rar]() {
    for (const rar_information& rar : res_grid[0].result.dl.rar_grants) {
      if (rar.backoff_indicator.has_value()) {
        backoff_rar = &rar;
        return true;
      }
    }
    return false;
  });

  ASSERT_TRUE(found);
  ASSERT_NE(backoff_rar, nullptr);
  EXPECT_TRUE(backoff_rar->grants.empty());
  ASSERT_EQ(tracker.nof_msg3_newtxs(), 0U) << "No preamble should have been granted a Msg3";
}

/// This test verifies that the configured Backoff Indicator duration, in ms, is mapped to its index in TS38.321,
/// Table 7.2-1. 80ms is table index 6.
TEST_F(ra_scheduler_backoff_duration_test, duration_is_mapped_to_table_index)
{
  rach_indication_message::preamble weak = create_preamble_with_snr(-10.0F);

  handle_rach_indication(test_helper::create_rach_indication(next_slot_rx(), {weak}));

  const rar_information* backoff_rar = nullptr;
  const bool             found       = run_slot_until([this, &backoff_rar]() {
    for (const rar_information& rar : res_grid[0].result.dl.rar_grants) {
      if (rar.backoff_indicator.has_value()) {
        backoff_rar = &rar;
        return true;
      }
    }
    return false;
  });

  ASSERT_TRUE(found);
  ASSERT_NE(backoff_rar, nullptr);
  EXPECT_EQ(*backoff_rar->backoff_indicator, 6U);
}

struct two_step_test_params {
  /// MsgA PUSCH TD offset.
  uint8_t                                td_offset;
  std::optional<tdd_ul_dl_config_common> tdd_cfg;
};

void PrintTo(const two_step_test_params& p, std::ostream* os)
{
  *os << "td_offset=" << +p.td_offset << " tdd=" << (p.tdd_cfg.has_value() ? "yes" : "no");
}

/// Test class for 2-step RACH (MsgA/MsgB) procedures.
///
/// The test cell is configured with a mix of 4-step (preamble IDs [0, 60)) and 2-step CB (preamble IDs [60, 64))
/// preambles on shared RACH occasions.  \c td_offset is parameterized and must be >= \c delay_tx_rx_slots (default=2)
/// so that the MsgA PUSCH slot (prach_slot_rx + td_offset) falls within the allocatable resource grid.
class ra_scheduler_two_step_rach_test : public ra_scheduler_setup, public ::testing::TestWithParam<two_step_test_params>
{
  static constexpr unsigned MSGA_PREAMBLE_OFFSET = 60;

public:
  ra_scheduler_two_step_rach_test() :
    ra_scheduler_setup(make_two_step_rach_req(GetParam()), /*sched_csi=*/false, /*sched_sib1=*/false)
  {
  }

  static sched_cell_configuration_request_message make_two_step_rach_req(const two_step_test_params& params)
  {
    cell_config_builder_params builder_params =
        create(params.tdd_cfg.has_value() ? duplex_mode::TDD : duplex_mode::FDD, frequency_range::FR1);
    builder_params.min_k1               = 2;
    builder_params.min_k2               = 2;
    builder_params.tdd_ul_dl_cfg_common = params.tdd_cfg;
    auto  req  = sched_config_helper::make_default_sched_cell_configuration_request(builder_params);
    auto& rach = *req.ran.ul_cfg_common.init_ul_bwp.rach_cfg_common;
    // Reserve preamble IDs [60, 64) for 2-step CB RACH.
    rach.nof_cb_preambles_per_ssb = MSGA_PREAMBLE_OFFSET;
    rach.two_step_rach_cfg.emplace();
    rach.two_step_rach_cfg->pusch.td_offset = params.td_offset;
    return req;
  }

  static rach_indication_message::preamble make_msga_preamble(unsigned preamble_idx, rnti_t tc_rnti)
  {
    return test_helper::create_preamble(MSGA_PREAMBLE_OFFSET + preamble_idx, tc_rnti);
  }

  rach_indication_message
  create_msga_rach_indication(std::initializer_list<rach_indication_message::preamble> preambles) const
  {
    return test_helper::create_rach_indication(next_slot_rx(), {preambles.begin(), preambles.end()});
  }

  /// \brief Builds and forwards a MsgA RACH indication, and records the actual PRACH slot it lands on.
  ///
  /// \c handle_rach_indication() (from \c ra_scheduler_setup) advances the simulator to the next slot with a valid
  /// PRACH occasion and overwrites the indication's slot_rx accordingly, so the slot passed to
  /// \c create_msga_rach_indication is not necessarily the one actually used. \c last_prach_slot_rx is captured
  /// right after, while \c next_slot_rx() still reflects that same (unadvanced) slot.
  void send_msga_rach(std::initializer_list<rach_indication_message::preamble> preambles)
  {
    handle_rach_indication(create_msga_rach_indication(preambles));
    last_prach_slot_rx = next_slot_rx();
  }

  /// \brief Inject a CRC result for a MsgA PUSCH by preamble index (as passed to \c make_msga_preamble).
  ///
  /// The MsgA PUSCH grant is scheduled and decoded using the (shared, per-occasion) RA-RNTI, not the preamble's
  /// TC-RNTI (TS 38.211, 6.3.1.1). The RA-RNTI is recomputed from the original PRACH occasion (rather than looked
  /// up in the current resource grid) so this also works after the grid has slid past the MsgA PUSCH slot.
  void send_msga_crc(unsigned preamble_idx, bool success)
  {
    const rnti_t ra_rnti = test_helper::compute_ra_rnti(cell_cfg,
                                                        last_prach_slot_rx,
                                                        /*start_symbol=*/0,
                                                        /*frequency_index=*/0);

    ul_crc_indication crc_ind;
    crc_ind.cell_index = cell_cfg.cell_index;
    crc_ind.sl_rx      = res_grid[0].slot;
    auto& pdu          = crc_ind.crcs.emplace_back();
    pdu.rnti           = ra_rnti;
    pdu.ue_index       = INVALID_DU_UE_INDEX;
    pdu.harq_id        = to_harq_id(0);
    pdu.rapid          = static_cast<uint8_t>(MSGA_PREAMBLE_OFFSET + preamble_idx);
    pdu.tb_crc_success = success;
    handle_crc_indication(crc_ind);
  }

  /// PRACH slot of the last MsgA RACH indication created via \c create_msga_rach_indication.
  slot_point last_prach_slot_rx;

  /// Returns true if the current slot result contains at least one MsgB grant of either type.
  bool has_msgb_grant() const
  {
    return std::any_of(res_grid[0].result.dl.rar_grants.begin(),
                       res_grid[0].result.dl.rar_grants.end(),
                       [](const rar_information& rar) {
                         return std::any_of(rar.grants.begin(), rar.grants.end(), [](const rar_ul_grant& g) {
                           return std::holds_alternative<rar_ul_grant::two_step_success_info>(g.type) or
                                  std::holds_alternative<rar_ul_grant::two_step_fallback_info>(g.type);
                         });
                       });
  }

  /// Returns true if there is a Msg3 PUSCH grant (created by FallbackRAR) for the given TC-RNTI anywhere in the
  /// lookahead portion of the resource grid.
  bool has_msg3_pusch_for(rnti_t tc_rnti) const
  {
    for (unsigned i = 0; i <= res_grid.max_ul_slot_alloc_delay; ++i) {
      for (const auto& pusch : res_grid[i].result.ul.puschs) {
        if (pusch.context.ue_index == INVALID_DU_UE_INDEX and pusch.context.msg3_delay.has_value() and
            pusch.pusch_cfg.rnti == tc_rnti) {
          return true;
        }
      }
    }
    return false;
  }

  /// Returns the pre-reserved MsgA PUSCH grant spanning all FDM occasions.
  grant_info get_msga_pusch_grant() const
  {
    const auto&    rach_cfg  = *cell_cfg.params.ul_cfg_common.init_ul_bwp.rach_cfg_common;
    const auto&    msga      = rach_cfg.two_step_rach_cfg->pusch;
    const auto&    ul_bwp    = cell_cfg.params.ul_cfg_common.init_ul_bwp;
    const auto&    td_alloc  = ul_bwp.pusch_cfg_common->pusch_td_alloc_list[msga.pusch_td_res_index];
    const unsigned crb_start = ul_bwp.generic_params.crbs.start() + msga.prb_start;
    return grant_info{ul_bwp.generic_params.scs,
                      td_alloc.symbols,
                      crb_interval{crb_start, crb_start + msga.po_fdm * msga.nof_prbs_per_msgA_po}};
  }

  /// Scans the lookahead grid and returns the nearest slot where the MsgA PUSCH grant is pre-reserved.
  std::optional<slot_point> find_msga_preresv_slot(const grant_info& grant) const
  {
    for (unsigned i = 1; i <= res_grid.max_ul_slot_alloc_delay; ++i) {
      if (res_grid[i].ul_res_grid.collides(grant)) {
        return res_grid[i].slot;
      }
    }
    return std::nullopt;
  }
};

TEST_P(ra_scheduler_two_step_rach_test, when_two_step_rach_enqueued_then_msga_pusch_is_scheduled)
{
  // Event: Enqueue RACH indication with two-step RACH preamble.
  const rnti_t tc_rnti  = to_rnti(to_value(rnti_t::MIN_CRNTI));
  auto         rach_ind = create_msga_rach_indication({make_msga_preamble(0, tc_rnti)});
  handle_rach_indication(rach_ind);

  // Test Case: MsgA PUSCH is scheduled in the slot td_offset after PRACH.
  ASSERT_TRUE(run_slot_until([this]() { return tracker.nof_msga_puschs() > 0; }, 10));
  ASSERT_EQ(tracker.nof_msga_puschs(), 1);
}

/// When MsgA PUSCH is decoded successfully (CRC=OK), the scheduler must respond with a SuccessRAR and must
/// not allocate a Msg3 PUSCH.
TEST_P(ra_scheduler_two_step_rach_test, when_msga_crc_ok_then_msgb_with_success_rar_scheduled_and_no_msg3)
{
  // Event: Enqueue RACH indication with two-step RACH preamble.
  const rnti_t tc_rnti = to_rnti(to_value(rnti_t::MIN_CRNTI));
  send_msga_rach({make_msga_preamble(0, tc_rnti)});

  // Event: MsgA PUSCH scheduled and forward CRC=OK.
  ASSERT_TRUE(run_slot_until([this]() { return not res_grid[0].result.ul.puschs.empty(); }));
  send_msga_crc(0, true);

  // Test: MsgB with SuccessRAR scheduled.
  ASSERT_TRUE(run_slot_until([this]() { return tracker.nof_success_rars() > 0; }));
  ASSERT_EQ(tracker.nof_success_rars(), 1);
  ASSERT_EQ(tracker.nof_fallback_rars(), 0);

  // Test: No Msg3 scheduled.
  ASSERT_FALSE(run_slot_until([this]() { return tracker.nof_msg3_newtxs() > 0; }));
  ASSERT_EQ(tracker.nof_msg3_newtxs(), 0) << "SuccessRAR must not allocate a Msg3 PUSCH";
}

/// When MsgA PUSCH decoding fails (CRC=KO), the scheduler must respond with a FallbackRAR and allocate a
/// Msg3 PUSCH for the UE to fall back to the 4-step procedure.
TEST_P(ra_scheduler_two_step_rach_test, when_msga_crc_ko_then_fallback_rar_and_msg3_scheduled)
{
  const rnti_t tc_rnti = to_rnti(to_value(rnti_t::MIN_CRNTI));
  send_msga_rach({make_msga_preamble(0, tc_rnti)});
  run_slot();

  // Event: MsgA PUSCH scheduled and forward CRC=KO.
  ASSERT_TRUE(run_slot_until([this]() { return not res_grid[0].result.ul.puschs.empty(); }));
  send_msga_crc(0, false);

  ASSERT_TRUE(run_slot_until([this]() { return tracker.nof_fallback_rars() > 0; }));
  ASSERT_EQ(tracker.nof_fallback_rars(), 1);
  ASSERT_EQ(tracker.nof_success_rars(), 0) << "No SuccessRAR expected for CRC=KO preamble";

  std::optional<bool> msg3_rapid_set;
  ASSERT_TRUE(run_slot_until([this, &msg3_rapid_set]() {
    const auto& puschs = res_grid[0].result.ul.puschs;
    auto        it     = std::find_if(
        puschs.begin(), puschs.end(), [](const ul_sched_info& p) { return p.context.msg3_delay.has_value(); });
    if (it != puschs.end()) {
      msg3_rapid_set = it->context.rapid.has_value();
    }
    return tracker.nof_msg3_newtxs() > 0;
  }));
  ASSERT_EQ(tracker.nof_msg3_newtxs(), 1) << "FallbackRAR must allocate a Msg3 PUSCH";
  ASSERT_TRUE(msg3_rapid_set.has_value()) << "Msg3 PUSCH was never observed in the resource grid";
  // "rapid" is a MsgA-only field; if set here, the PHY will wrongly apply the MsgA descrambling sequence
  // (TS38.211 6.3.1.1) to this regular Msg3 PUSCH and fail to decode it.
  ASSERT_FALSE(*msg3_rapid_set) << "FallbackRAR Msg3 PUSCH context.rapid must not be set";
}

/// While the MsgA PUSCH CRC indication has not yet arrived, MsgB scheduling must be postponed to allow
/// time for the CRC to be received.  Once the CRC arrives the MsgB must be scheduled promptly.
TEST_P(ra_scheduler_two_step_rach_test, when_crc_pending_then_msgb_scheduling_is_postponed)
{
  const rnti_t tc_rnti = to_rnti(to_value(rnti_t::MIN_CRNTI));
  send_msga_rach({make_msga_preamble(0, tc_rnti)});
  run_slot();

  // Run several slots without a CRC indication; MsgB must not be scheduled.
  ASSERT_FALSE(run_slot_until([this]() { return not res_grid[0].result.dl.rar_grants.empty(); }, 5));

  // CRC=OK arrives; MsgB must now be scheduled.
  send_msga_crc(0, true);
  ASSERT_TRUE(run_slot_until([this]() { return tracker.nof_success_rars() > 0; }));
  ASSERT_EQ(tracker.nof_success_rars(), 1);
}

/// When multiple MsgA preambles arrive in the same PRACH occasion and their CRC outcomes differ, the scheduler
/// must send a SuccessRAR for the decoded preamble and a FallbackRAR (with Msg3) for the failed one, both
/// within the same MsgB response.
TEST_P(ra_scheduler_two_step_rach_test, when_mixed_crc_outcomes_both_rar_types_scheduled_together)
{
  const rnti_t tc_rnti_ok = to_rnti(to_value(rnti_t::MIN_CRNTI));
  const rnti_t tc_rnti_ko = to_rnti(to_value(rnti_t::MIN_CRNTI) + 1);
  send_msga_rach({make_msga_preamble(0, tc_rnti_ok), make_msga_preamble(1, tc_rnti_ko)});

  ASSERT_TRUE(run_slot_until([this]() { return not res_grid[0].result.ul.puschs.empty(); }));
  send_msga_crc(0, true);
  send_msga_crc(1, false);

  ASSERT_TRUE(run_slot_until([this]() { return tracker.nof_success_rars() > 0 and tracker.nof_fallback_rars() > 0; }));
  ASSERT_EQ(tracker.nof_success_rars(), 1);
  ASSERT_EQ(tracker.nof_fallback_rars(), 1);

  ASSERT_TRUE(run_slot_until([this]() { return not res_grid[0].result.ul.puschs.empty(); }));
  ASSERT_EQ(res_grid[0].result.ul.puschs.size(), 1);
  ASSERT_EQ(res_grid[0].result.ul.puschs[0].pusch_cfg.rnti, tc_rnti_ko);
  ASSERT_EQ(tracker.nof_msg3_newtxs(), 1) << "FallbackRAR preamble must have a Msg3 PUSCH";
}

/// The MsgA PUSCH resources must be blocked in the UL resource grid before any PRACH preamble is detected,
/// so that the UE PUSCH scheduler cannot steal them.
TEST_P(ra_scheduler_two_step_rach_test, msga_pusch_rbs_are_pre_reserved_before_preamble_detection)
{
  const grant_info grant = get_msga_pusch_grant();

  // No RACH indication has been sent. The scheduler must still pre-reserve the MsgA PUSCH resources.
  ASSERT_TRUE(run_slot_until([&]() { return find_msga_preresv_slot(grant).has_value(); }, 400))
      << "MsgA PUSCH pre-reservation not found within lookahead grid";
}

INSTANTIATE_TEST_SUITE_P(two_step_rach,
                         ra_scheduler_two_step_rach_test,
                         ::testing::Values(two_step_test_params{2},
                                           two_step_test_params{4},
                                           two_step_test_params{10, create_tdd_pattern(tdd_fr1_30khz::DDDDDDDSUU)},
                                           two_step_test_params{4, create_tdd_pattern(tdd_fr1_30khz::DSUU)}));

/// Test fixture for CFRA-specific RA scheduler behaviour.
///
/// The cell is configured with 4 dedicated CFRA preambles (IDs [60, 64)), leaving 60 preambles for CBRA.
class ra_scheduler_cfra_test : public ra_scheduler_setup, public ::testing::Test
{
  static constexpr unsigned NOF_CB_PREAMBLES = 60;

public:
  ra_scheduler_cfra_test() : ra_scheduler_setup(make_cfra_sched_req(), false, false) {}

  static sched_cell_configuration_request_message make_cfra_sched_req()
  {
    cell_config_builder_params bparams;
    auto                       req = sched_config_helper::make_default_sched_cell_configuration_request(bparams);
    req.ran.ul_cfg_common.init_ul_bwp.rach_cfg_common->nof_cb_preambles_per_ssb = NOF_CB_PREAMBLES;
    return req;
  }

  rach_indication_message create_cfra_rach_indication(rnti_t tc_rnti) const
  {
    const unsigned cfra_preamble_id =
        cell_cfg.params.ul_cfg_common.init_ul_bwp.rach_cfg_common->nof_cb_preambles_per_ssb;
    auto preamble = test_helper::create_preamble(cfra_preamble_id, tc_rnti);
    return test_helper::create_rach_indication(next_slot_rx(), {preamble});
  }

  void send_cfra_crc(rnti_t tc_rnti, bool success)
  {
    ul_crc_indication crc_ind;
    crc_ind.cell_index = cell_cfg.cell_index;
    crc_ind.sl_rx      = res_grid[0].slot;
    auto& pdu          = crc_ind.crcs.emplace_back();
    pdu.rnti           = tc_rnti;
    pdu.ue_index       = cfra_ue_index;
    pdu.harq_id        = to_harq_id(0);
    pdu.tb_crc_success = success;
    handle_crc_indication(crc_ind);
  }

  const du_ue_index_t cfra_ue_index = to_du_ue_index(5);
};

/// Verify that a Msg3 CRC with a valid UE index (CFRA path) is accepted by the RA scheduler.
TEST_F(ra_scheduler_cfra_test, cfra_msg3_crc_with_valid_ue_index_is_accepted)
{
  const rnti_t tc_rnti = to_rnti(0x4601);
  ra_sch.handle_cfra_mapping_update(cfra_ue_index, tc_rnti);
  handle_rach_indication(create_cfra_rach_indication(tc_rnti));

  for (unsigned slot_count = 0, max_slots = 1000; slot_count < max_slots and tracker.nof_msg3_acked() == 0;
       ++slot_count) {
    run_slot();
    if (not res_grid[0].result.ul.puschs.empty()) {
      send_cfra_crc(tc_rnti, true);
    }
  }

  ASSERT_EQ(tracker.nof_msg3_acked(), 1);
}

/// Verify the Msg3 retransmission flow for a CFRA UE: CRC KO triggers retx, CRC OK completes the procedure.
TEST_F(ra_scheduler_cfra_test, cfra_msg3_crc_ko_causes_retx_then_ok_completes)
{
  const rnti_t tc_rnti = to_rnti(0x4601);
  ra_sch.handle_cfra_mapping_update(cfra_ue_index, tc_rnti);
  handle_rach_indication(create_cfra_rach_indication(tc_rnti));

  // NACK the first Msg3 new-tx.
  for (unsigned slot_count = 0, max_slots = 1000; slot_count < max_slots and tracker.nof_msg3_newtxs() == 0;
       ++slot_count) {
    run_slot();
    if (not res_grid[0].result.ul.puschs.empty()) {
      send_cfra_crc(tc_rnti, false);
    }
  }
  ASSERT_GE(tracker.nof_msg3_newtxs(), 1);
  ASSERT_EQ(tracker.nof_msg3_acked(), 0);

  // ACK the retx.
  for (unsigned slot_count = 0, max_slots = 1000; slot_count < max_slots and tracker.nof_msg3_acked() == 0;
       ++slot_count) {
    run_slot();
    if (not res_grid[0].result.ul.puschs.empty()) {
      send_cfra_crc(tc_rnti, true);
    }
  }

  ASSERT_GE(tracker.nof_msg3_retxs(), 1);
  ASSERT_EQ(tracker.nof_msg3_acked(), 1);
}

/// Verify that, when a NACKed Msg3 HARQ is never rescheduled for retransmission (starved until the retx timeout
/// fires), its pending_msg3s ring entry is released -- so a later PRACH reusing the same TC-RNTI is accepted
/// instead of being rejected as "already under use".
TEST_F(ra_scheduler_cfra_test, when_msg3_retx_starves_then_tc_rnti_is_released_for_reuse)
{
  const rnti_t tc_rnti = to_rnti(0x4601);
  ra_sch.handle_cfra_mapping_update(cfra_ue_index, tc_rnti);
  handle_rach_indication(create_cfra_rach_indication(tc_rnti));

  // NACK the first Msg3 new-tx, putting its HARQ into pending_retx state.
  for (unsigned slot_count = 0, max_slots = 1000; slot_count < max_slots and tracker.nof_msg3_newtxs() == 0;
       ++slot_count) {
    run_slot();
    if (not res_grid[0].result.ul.puschs.empty()) {
      send_cfra_crc(tc_rnti, false);
    }
  }
  ASSERT_GE(tracker.nof_msg3_newtxs(), 1);

  // Block all UL resources so the scheduler can never grant the Msg3 retransmission, until the retx timeout fires
  // and the HARQ (and its pending_msg3s ring entry) is force-discarded.
  const grant_info marked_res{cell_cfg.scs_common(),
                              ofdm_symbol_range{0, NOF_OFDM_SYM_PER_SLOT_NORMAL_CP},
                              crb_interval{0, cell_cfg.nof_ul_prbs}};
  const unsigned   lookahead_ul_rbs = res_grid.max_ul_slot_alloc_delay;
  for (unsigned i = 0; i != lookahead_ul_rbs; ++i) {
    if (cell_cfg.is_ul_enabled(res_grid[i].slot)) {
      res_grid[i].ul_res_grid.fill(marked_res);
    }
  }
  const unsigned nof_retxs_before_block = tracker.nof_msg3_retxs();
  for (unsigned i = 0, max_slots = 500; i != max_slots; ++i) {
    if (cell_cfg.is_ul_enabled(res_grid[lookahead_ul_rbs].slot)) {
      res_grid[lookahead_ul_rbs].ul_res_grid.fill(marked_res);
    }
    run_slot();
  }
  ASSERT_EQ(tracker.nof_msg3_retxs(), nof_retxs_before_block)
      << "No retx grant should have been possible while UL resources were blocked";

  // Drain the lookahead window of already-blocked slots (without refilling) before probing with a new preamble.
  for (unsigned i = 0; i != lookahead_ul_rbs; ++i) {
    run_slot();
  }

  // A new preamble reusing the SAME TC-RNTI must now be accepted. If pending_msg3s still held a stale entry for
  // this TC-RNTI, ra_scheduler's ring-collision check would silently drop this preamble instead.
  ra_sch.handle_cfra_mapping_update(cfra_ue_index, tc_rnti);
  handle_rach_indication(create_cfra_rach_indication(tc_rnti));
  for (unsigned slot_count = 0, max_slots = 1000; slot_count < max_slots and tracker.nof_msg3_newtxs() < 2;
       ++slot_count) {
    run_slot();
  }
  ASSERT_GE(tracker.nof_msg3_newtxs(), 2)
      << "TC-RNTI reuse after the retx timeout was rejected -- pending_msg3s leaked";
}

/// Verify that a CRC with a valid but unregistered UE index is filtered by the RA scheduler.
///
/// Because the RNTI is not in pending_cfra_ues for that ue_index, is_ra_crc() returns false and
/// the Msg3 HARQ is not freed.  A retransmission must therefore be scheduled.
TEST_F(ra_scheduler_cfra_test, non_cfra_crc_with_valid_ue_index_is_filtered)
{
  const rnti_t tc_rnti            = to_rnti(0x4601);
  const auto   unrelated_ue_index = to_du_ue_index(10);
  ra_sch.handle_cfra_mapping_update(cfra_ue_index, tc_rnti);
  handle_rach_indication(create_cfra_rach_indication(tc_rnti));

  // Wait for the Msg3 new-tx.
  for (unsigned slot_count = 0, max_slots = 1000; slot_count < max_slots and tracker.nof_msg3_newtxs() == 0;
       ++slot_count) {
    run_slot();
  }
  ASSERT_GE(tracker.nof_msg3_newtxs(), 1) << "Msg3 new-tx was not scheduled";

  // Send a CRC with a valid but unregistered UE index directly to the RA scheduler, bypassing the tracker.
  // pending_cfra_ues[unrelated_ue_index] is INVALID_RNTI, so is_ra_crc() must reject it.
  {
    ul_crc_indication bad_crc;
    bad_crc.cell_index = cell_cfg.cell_index;
    bad_crc.sl_rx      = res_grid[0].slot;
    auto& pdu          = bad_crc.crcs.emplace_back();
    pdu.rnti           = tc_rnti;
    pdu.ue_index       = unrelated_ue_index;
    pdu.harq_id        = to_harq_id(0);
    pdu.tb_crc_success = true;
    ra_sch.handle_crc_indication(bad_crc);
  }
  // Send a valid NACK to give the HARQ real feedback and trigger a retransmission.
  // If the bad CRC above was wrongly accepted, the HARQ would already be freed and no retx would appear.
  send_cfra_crc(tc_rnti, false);

  // The HARQ must not have been freed by the filtered CRC — a retransmission must be scheduled.
  for (unsigned i = 0, max_slots = 1000; i < max_slots and tracker.nof_msg3_retxs() == 0; ++i) {
    run_slot();
  }
  ASSERT_GE(tracker.nof_msg3_retxs(), 1) << "Filtered CRC must not free the HARQ — retx expected";
}

} // namespace
