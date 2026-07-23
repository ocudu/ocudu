// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

/// \file
/// \brief Unit test for scheduler using different TDD patterns.

#include "test_utils/indication_generators.h"
#include "test_utils/scheduler_test_simulator.h"
#include "tests/test_doubles/scheduler/cell_config_builder_profiles.h"
#include "tests/test_doubles/scheduler/scheduler_config_helper.h"
#include "tests/test_doubles/utils/test_rng.h"
#include "ocudu/du/du_update_config_helpers.h"
#include "ocudu/ran/tdd/tdd_ul_dl_config_formatters.h"
#include "ocudu/scheduler/config/time_domain_resource_helper.h"
#include "ocudu/scheduler/rrm/pucch_resource_manager.h"
#include "ocudu/scheduler/rrm/srs_resource_manager_factory.h"
#include "ocudu/support/enum_utils.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace cell_config_builder_profiles;

namespace {

// Test parameters for both single-UE and multi-UE TDD tests.
struct common_tdd_tester_params {
  enum class coreset_type { auto_derived, dur2 };

  bool                    csi_rs_enabled = true;
  tdd_ul_dl_config_common tdd_cfg;
  unsigned                min_k = 4;
  bs_channel_bandwidth    bw    = bs_channel_bandwidth::MHz20;
  // CORESET profile. "dur2" forces CORESET#0 index 13 (duration 2 symbols) and reduces the SS#1/SS#2 PDCCH candidate
  // counts to keep the monitored candidates per slot within limits; "auto_derived" leaves the auto-derived defaults
  // untouched.
  coreset_type cs_type = coreset_type::auto_derived;
  // PUCCH Format 1 multiplexing for the dedicated SR/HARQ resources.
  pucch_nof_cyclic_shifts f1_nof_cyc_shifts = pucch_nof_cyclic_shifts::twelve;
  bool                    f1_occ_supported  = true;
  // When set, every UE of the cell gets a periodic SRS resource with this periodicity, in slots.
  std::optional<srs_periodicity> srs_period;
};

// Default scheduler expert config for the TDD testers: remove the PRACH guardbands so PUSCH can fill the UL slot, and
// raise the per-slot PUCCH / UL-grant ceilings so many UEs can be served in the single UL slot.
scheduler_expert_config default_expert_cfg()
{
  auto expert_cfg                        = config_helpers::make_default_scheduler_expert_config();
  expert_cfg.ra.nof_prach_guardbands_rbs = 0;
  expert_cfg.ue.max_pucchs_per_slot      = 120;
  expert_cfg.ue.max_ul_grants_per_slot   = 140;
  return expert_cfg;
}

class base_scheduler_tdd_tester : public scheduler_test_simulator
{
protected:
  explicit base_scheduler_tdd_tester(const common_tdd_tester_params& testparams) :
    scheduler_test_simulator(scheduler_test_sim_config{.sched_cfg = default_expert_cfg(),
                                                       .max_scs   = testparams.tdd_cfg.ref_scs,
                                                       .auto_uci  = true,
                                                       .auto_crc  = true})
  {
    ocudu_assert(testparams.tdd_cfg.ref_scs == subcarrier_spacing::kHz30, "Only 30kHz SCS is supported in this test");
    params = cell_config_builder_profiles::create(duplex_mode::TDD, frequency_range::FR1, testparams.bw);
    if (testparams.cs_type == common_tdd_tester_params::coreset_type::dur2) {
      params.cs0_index             = 13;
      params.max_coreset0_duration = 2;
    }
    params.csi_rs_enabled       = testparams.csi_rs_enabled;
    params.tdd_ul_dl_cfg_common = testparams.tdd_cfg;
    params.min_k1               = testparams.min_k;
    params.min_k2               = testparams.min_k;

    // Add Cell. Provision ample PUCCH resources (as in a real deployment) so tests can host many UEs, each with a
    // distinct PUCCH config, on the (single) UL slot.
    auto                          cell_req = sched_config_helper::make_default_sched_cell_configuration_request(params);
    pucch_resource_builder_params pucch_params;
    pucch_params.res_set_size             = 8;
    pucch_params.nof_cell_res_set_configs = 2;
    pucch_params.nof_cell_sr_resources    = 90;
    pucch_params.nof_cell_csi_resources   = 90;
    auto& f1                              = pucch_params.f0_or_f1_params.emplace<pucch_f1_params>();
    f1.nof_cyc_shifts                     = testparams.f1_nof_cyc_shifts;
    f1.occ_supported                      = testparams.f1_occ_supported;
    // Increase PUCCH Format 2 code rate to support DL-heavy TDD configurations.
    std::get<pucch_f2_params>(pucch_params.f2_or_f3_or_f4_params).max_code_rate = max_pucch_code_rate::dot_35;
    cell_req.ran.init_bwp.pucch.resources                                       = pucch_params;
    // A duration-2 CORESET#0 doubles the PDCCH candidate cost per aggregation level, so reduce the common SS#1
    // (RA/paging) and dedicated SS#2 candidate counts to keep the monitored candidates per slot within limits.
    if (testparams.cs_type == common_tdd_tester_params::coreset_type::dur2) {
      for (auto& ss : cell_req.ran.dl_cfg_common.init_dl_bwp.pdcch_common.search_spaces) {
        if (ss.get_id() == to_search_space_id(1)) {
          ss.set_non_ss0_nof_candidates({0, 0, 8, 0, 0});
        }
      }
      if (cell_req.ran.init_bwp.pdcch_cfg.has_value()) {
        for (auto& ss : cell_req.ran.init_bwp.pdcch_cfg->search_spaces) {
          if (ss.get_id() == to_search_space_id(2)) {
            ss.set_non_ss0_nof_candidates({0, 2, 2, 0, 0});
          }
        }
      }
    }
    // Place PRACH clear of the PUCCH resource pool (as the DU does), so a large PUCCH pool does not overlap the PRACH
    // occasion.
    cell_req.ran.ul_cfg_common.init_ul_bwp.rach_cfg_common->rach_cfg_generic.msg1_frequency_start =
        config_helpers::compute_prach_frequency_start(
            pucch_params, cell_req.ran.ul_cfg_common.init_ul_bwp.generic_params.crbs.length(), false);
    const bool srs_enabled = testparams.srs_period.has_value();
    if (srs_enabled) {
      cell_req.ran.init_bwp.srs_cfg.srs_type_enabled       = srs_type::periodic;
      cell_req.ran.init_bwp.srs_cfg.srs_period_prohib_time = *testparams.srs_period;
      // Regenerate the common PUSCH time-domain-resource table with SRS awareness, adding a shortened candidate per
      // slot so Msg3/RAR PUSCH can still be scheduled once SRS occupies the tail of the slot.
      cell_req.ran.ul_cfg_common.init_ul_bwp.pusch_cfg_common->pusch_td_alloc_list =
          time_domain_resource_helper::generate_dedicated_pusch_td_res_list(
              cell_req.ran.tdd_cfg,
              cell_req.ran.ul_cfg_common.init_ul_bwp.generic_params.cp,
              params.min_k2,
              cell_req.ran.init_bwp.srs_cfg.max_nof_symbols.value(),
              cell_req.ran.init_bwp.srs_cfg.nof_symbols);
    }
    this->add_cell(cell_req);

    // Setup PUCCH and SRS resource builders.
    pucch_builder.add_cell(to_du_cell_index(0), cell_cfg().params);
    if (srs_enabled) {
      srs_builder = create_srs_resource_manager(cell_cfg().params);
      srs_builder->add_cell(to_du_cell_index(0), cell_cfg().params);
    }
  }

  // Build a UE creation request, allocating a distinct dedicated PUCCH config (SR/CSI/HARQ) and, if enabled, a
  // distinct periodic SRS occasion from the cell resource pools, so many UEs can coexist on the single UL slot (as a
  // real DU does).
  sched_ue_creation_request_message
  build_ue_request(du_ue_index_t idx, rnti_t rnti, std::initializer_list<lcid_t> lcids)
  {
    auto ue_cfg     = sched_config_helper::create_default_sched_ue_creation_request(cell_cfg().params, lcids);
    ue_cfg.ue_index = idx;
    ue_cfg.crnti    = rnti;
    report_fatal_error_if_not(
        pucch_builder.alloc_resources((*ue_cfg.cfg.cells)[0]), "Failed to allocate PUCCH resources for UE {}", idx);
    if (srs_builder != nullptr) {
      // The SRS resource manager expects the SRS config to be unset before allocating, mirroring the DU manager's own
      // reset_serv_cell_cfg() call ahead of its own srs_res_mng->alloc_resources() call.
      (*ue_cfg.cfg.cells)[0].serv_cell_cfg.ul_config->init_ul_bwp.srs_cfg.reset();
      report_fatal_error_if_not(
          srs_builder->alloc_resources((*ue_cfg.cfg.cells)[0]), "Failed to allocate SRS resources for UE {}", idx);
    }
    return ue_cfg;
  }

  cell_config_builder_params params;
  // Builds a distinct per-UE dedicated PUCCH config from the cell pool for every UE created by these tests.
  pucch_resource_manager pucch_builder{120};
  // Builds a distinct per-UE periodic SRS occasion from the cell pool. Null when SRS is disabled for the test.
  std::unique_ptr<srs_resource_manager> srs_builder;
};

// ------------------------------------ single-UE case --------------------------------------------

struct singleue_tdd_test_params {
  bool                    csi_rs_enabled;
  tdd_ul_dl_config_common tdd_cfg;
  unsigned                min_k = 4;
};

/// \brief Base fixture for the single-UE TDD tests. Builds a default MHz20 cell from \ref singleue_tdd_test_params and
/// provides a helper to build the (single) UE's creation request.
class base_scheduler_single_ue_tdd_tester : public base_scheduler_tdd_tester
{
protected:
  explicit base_scheduler_single_ue_tdd_tester(const singleue_tdd_test_params& testparams) :
    base_scheduler_tdd_tester(common_tdd_tester_params{.csi_rs_enabled = testparams.csi_rs_enabled,
                                                       .tdd_cfg        = testparams.tdd_cfg,
                                                       .min_k          = testparams.min_k})
  {
    this->add_ue(build_ue_request(ue_idx, ue_rnti, {ue_drb_lcid}));
  }

  const du_ue_index_t ue_idx      = to_du_ue_index(0);
  const rnti_t        ue_rnti     = to_rnti(0x4601);
  const lcid_t        ue_drb_lcid = LCID_MIN_DRB;
};

/// Formatter for test params.
void PrintTo(const singleue_tdd_test_params& value, ::std::ostream* os)
{
  *os << fmt::format(
      "csi={} tdd={} min_k={}", value.csi_rs_enabled ? "enabled" : "disabled", value.tdd_cfg, value.min_k);
}

class scheduler_dl_tdd_tester : public base_scheduler_single_ue_tdd_tester,
                                public ::testing::TestWithParam<singleue_tdd_test_params>
{
public:
  scheduler_dl_tdd_tester() : base_scheduler_single_ue_tdd_tester(GetParam())
  {
    // Enqueue enough bytes for continuous DL tx.
    dl_buffer_state_indication_message dl_buf_st{ue_idx, ue_drb_lcid, 10000000};
    this->push_dl_buffer_state(dl_buf_st);
  }
};

TEST_P(scheduler_dl_tdd_tester, all_dl_slots_are_scheduled)
{
  static constexpr unsigned MAX_COUNT = 1000;
  for (unsigned count = 0; count != MAX_COUNT; ++count) {
    this->run_slot();

    // For every DL slot.
    if (cell_cfg(to_du_cell_index(0)).is_dl_enabled(this->last_result_slot())) {
      // Ensure UE PDSCH allocations are made.
      ASSERT_FALSE(this->last_sched_result(to_du_cell_index(0))->dl.ue_grants.empty()) << fmt::format(
          "The UE configuration is leading to slot {} not having DL UE grant scheduled", this->last_result_slot());
    }
  }
}

class scheduler_ul_tdd_tester : public base_scheduler_single_ue_tdd_tester,
                                public ::testing::TestWithParam<singleue_tdd_test_params>
{
public:
  scheduler_ul_tdd_tester() : base_scheduler_single_ue_tdd_tester(GetParam())
  {
    // Enqueue enough bytes for continuous UL tx.
    ul_bsr_indication_message bsr{
        to_du_cell_index(0), ue_idx, ue_rnti, bsr_format::SHORT_BSR, {ul_bsr_lcg_report{uint_to_lcg_id(0), 10000000}}};
    this->push_bsr(bsr);

    // Run some slots to ensure that there is space for PDCCH to be scheduled.
    unsigned tdd_period = nof_slots_per_tdd_period(*cell_cfg(to_du_cell_index(0)).params.tdd_cfg);
    for (unsigned i = 0; i != 2 * tdd_period; ++i) {
      run_slot();
    }
  }
};

TEST_P(scheduler_ul_tdd_tester, all_ul_slots_are_scheduled)
{
  static constexpr unsigned MAX_COUNT = 1000;
  for (unsigned count = 0; count != MAX_COUNT; ++count) {
    this->run_slot();

    // For every UL slot.
    // Note: Skip special slots in test for now.
    if (cell_cfg().is_fully_ul_enabled(this->last_result_slot())) {
      // Ensure UE PUSCH allocations are made.
      ASSERT_FALSE(this->last_sched_result()->ul.puschs.empty()) << fmt::format(
          "The UE configuration is leading to slot {} not having UL UE grant scheduled", this->last_result_slot());
    }
  }
}

INSTANTIATE_TEST_SUITE_P(
    scheduler_tdd_test,
    scheduler_dl_tdd_tester,
    testing::Values(
        // clang-format off
  // csi_enabled, {ref_scs, pattern1={slot_period, DL_slots, DL_symbols, UL_slots, UL_symbols}, pattern2={...}, min_k}
  singleue_tdd_test_params{true,  {subcarrier_spacing::kHz30, {10, 6, 5, 3, 4}}}, // DDDDDDSUUU
  // > TS 38.101-4, Table A.1.2-2: TDD UL-DL configuration for SCS 30kHz.
  singleue_tdd_test_params{true, create_tdd_pattern(tdd_pattern_profile_fr1_30khz::DDDDDDDSUU)},
  singleue_tdd_test_params{true, create_tdd_pattern(tdd_pattern_profile_fr1_30khz::DDDDDDDSUU), 2},
  singleue_tdd_test_params{true, create_tdd_pattern(tdd_pattern_profile_fr1_30khz::DDDSU)},
  singleue_tdd_test_params{true, create_tdd_pattern(tdd_pattern_profile_fr1_30khz::DDDSU), 2},
  singleue_tdd_test_params{true, create_tdd_pattern(tdd_pattern_profile_fr1_30khz::DDDSUDDSUU)},
  // CSI disabled: the DL burst's HARQ-ACKs can only reach the two UL slots after the special slot, and a periodic CSI
  // on one of them exhausts the shared Format 2 resource, leaving the hardest DL slot without a HARQ-ACK opportunity.
  singleue_tdd_test_params{false, create_tdd_pattern(tdd_pattern_profile_fr1_30khz::DDDSUUDDDD)},
  singleue_tdd_test_params{true, create_tdd_pattern(tdd_pattern_profile_fr1_30khz::DSUU)},
  singleue_tdd_test_params{true, create_tdd_pattern(tdd_pattern_profile_fr1_30khz::DSSU)},
  // > Other DL-heavy patterns.
  singleue_tdd_test_params{false, {subcarrier_spacing::kHz30, {10, 8, 0, 1, 0}}},
  singleue_tdd_test_params{false, {subcarrier_spacing::kHz30, {10, 8, 0, 1, 0}}, 1},
  // >> DDDSUUUUDD
  singleue_tdd_test_params{true, {subcarrier_spacing::kHz30, {8,  3, 6, 4, 4}, tdd_ul_dl_pattern{2, 2, 0, 0, 0}}},
  singleue_tdd_test_params{true,  {subcarrier_spacing::kHz30, {4,  2, 9, 1, 0}}},
  singleue_tdd_test_params{true,  {subcarrier_spacing::kHz30, {4,  2, 9, 1, 0}}, 1},
  singleue_tdd_test_params{true,  {subcarrier_spacing::kHz30, {10, 6, 13, 3, 0}}, 4} // DDDDDDSUUU, with 13 DL symbols in special slot
// Note: The params below lead to a failure due to "Not enough space in PUCCH". However, I don't think there is no valid
// k1 candidate list that accommodates all DL slots.
  //singleue_tdd_test_params{true, {subcarrier_spacing::kHz30, {10, 8, 5, 1, 4}}}
));
// clang-format on

INSTANTIATE_TEST_SUITE_P(
    scheduler_tdd_test,
    scheduler_ul_tdd_tester,
    testing::Values(
        // clang-format off
  // csi_enabled, {ref_scs, pattern1={slot_period, DL_slots, DL_symbols, UL_slots, UL_symbols}, pattern2={...}}
  singleue_tdd_test_params{true,  {subcarrier_spacing::kHz30, {10, 6, 5, 3, 4}}}, // DDDDDDSUUU
  // > TS 38.101-4, Table A.1.2-2: TDD UL-DL configuration for SCS 30kHz.
  singleue_tdd_test_params{true, create_tdd_pattern(tdd_pattern_profile_fr1_30khz::DDDDDDDSUU)},
  singleue_tdd_test_params{true, create_tdd_pattern(tdd_pattern_profile_fr1_30khz::DDDDDDDSUU), 2},
  singleue_tdd_test_params{true, create_tdd_pattern(tdd_pattern_profile_fr1_30khz::DDDSU)},
  singleue_tdd_test_params{true, create_tdd_pattern(tdd_pattern_profile_fr1_30khz::DDDSUDDSUU), 2},
  singleue_tdd_test_params{true, create_tdd_pattern(tdd_pattern_profile_fr1_30khz::DDDSUUDDDD)},
  singleue_tdd_test_params{true, create_tdd_pattern(tdd_pattern_profile_fr1_30khz::DSUU), 2},
  singleue_tdd_test_params{true, create_tdd_pattern(tdd_pattern_profile_fr1_30khz::DSSU)},
  // > Other DL heavy patterns
  singleue_tdd_test_params{true,  {subcarrier_spacing::kHz30, {10, 8, 5, 1, 4}}}, // DDDDDDDDSU
  // >> DDDSUUUUDD
  singleue_tdd_test_params{true, {subcarrier_spacing::kHz30, {8,  3, 6, 4, 4}, tdd_ul_dl_pattern{2, 2, 0, 0, 0}}},
  singleue_tdd_test_params{true,  {subcarrier_spacing::kHz30, {4,  2, 9, 1, 0}}},  // DDSU
  singleue_tdd_test_params{true,  {subcarrier_spacing::kHz30, {10, 4, 5, 5, 0}}, 5}, // DDDDSUUUUU
  singleue_tdd_test_params{true,  {subcarrier_spacing::kHz30, {10, 6, 13, 3, 0}}, 4}, // DDDDDDSUUU, with 13 DL symbols in special slot
  // UL heavy
  singleue_tdd_test_params{true, {subcarrier_spacing::kHz30, {10, 3,  5, 6, 0}}},
  singleue_tdd_test_params{true, {subcarrier_spacing::kHz30, {5,  1, 10, 3, 0}, tdd_ul_dl_pattern{5, 1, 10, 3, 0}}, 2},
  singleue_tdd_test_params{true, {subcarrier_spacing::kHz30, {6,  2, 10, 3, 0}, tdd_ul_dl_pattern{4, 1, 0, 3, 0}}, 2},
  singleue_tdd_test_params{true, {subcarrier_spacing::kHz30, {4,  1, 10, 2, 0}, tdd_ul_dl_pattern{6, 1, 10, 4, 0}}, 2},
  singleue_tdd_test_params{true, {subcarrier_spacing::kHz30, {10, 2, 10, 7, 0}}, 2},
  singleue_tdd_test_params{true, {subcarrier_spacing::kHz30, {10, 2,  6, 7, 4}}, 2},
  singleue_tdd_test_params{true, {subcarrier_spacing::kHz30, {10, 2,  6, 7, 4}}},
  singleue_tdd_test_params{true, {subcarrier_spacing::kHz30, {5,  1, 10, 3, 0}}, 2}
));
// clang-format on

// ------------------------------------ multi-UE case --------------------------------------------

// Background traffic profile for the persistent UEs of the multi-UE TDD tests. \c mixed assigns each background UE a
// random direction; the others force the same direction for all of them.
enum class multiue_bg_traffic { dl_only, ul_only, bidir, mixed };

const char* to_string(multiue_bg_traffic t)
{
  switch (t) {
    case multiue_bg_traffic::dl_only:
      return "dl_only";
    case multiue_bg_traffic::ul_only:
      return "ul_only";
    case multiue_bg_traffic::bidir:
      return "bidir";
    default:
      return "mixed";
  }
}

// Parameters for the multi-UE TDD stress test.
struct multiue_tdd_test_params {
  tdd_ul_dl_config_common tdd_cfg;
  unsigned                min_k = 4;
  // Number of persistent UEs with constant traffic used to saturate the grid.
  unsigned nof_background_ues;
  // Total transient UEs exercised over the run; also the shared index pool, so it bounds the peak concurrency.
  unsigned nof_transient_ues;
  // Number of transient UEs launched together per wave (arrival burst size). The number of waves is derived so
  // that the whole transient pool is exercised: nof_waves = ceil(nof_transient_ues / ues_per_wave).
  unsigned ues_per_wave;
  // Spacing between consecutive waves, in slots.
  unsigned wave_period_in_slots;
  // When set, each transient UE arrives via the full RACH procedure (RAR + Msg3) before entering fallback, so the
  // RAR/Msg3 load competes with the ConRes CE for the same slots (as in a real deployment).
  bool rach_driven = false;
  // Direction of the persistent background UEs' traffic.
  multiue_bg_traffic background_traffic = multiue_bg_traffic::mixed;
  // When set, every UE (background and transient) gets a periodic SRS resource with this periodicity, in slots.
  // Periodic SRS occasions are placed at the end of the UL slot, so on UL-scarce TDD patterns they narrow the
  // symbol window available for Msg3/retx PUSCH and other UL grants on the same (few) UL slots.
  std::optional<srs_periodicity> srs_period;
};

void PrintTo(const multiue_tdd_test_params& value, ::std::ostream* os)
{
  *os << fmt::format(
      "tdd={} bg_ues={} transient_ues={} ues_per_wave={} wave_period_slots={} rach_driven={} bg_traffic={} srs={}",
      value.tdd_cfg,
      value.nof_background_ues,
      value.nof_transient_ues,
      value.ues_per_wave,
      value.wave_period_in_slots,
      value.rach_driven,
      to_string(value.background_traffic),
      value.srs_period.has_value() ? fmt::format("{}", static_cast<unsigned>(*value.srs_period)) : "disabled");
}

/// \brief Base fixture that saturates the grid with background traffic while overlapping waves of UEs attach in
/// fallback, complete contention resolution and detach. Derived fixtures pick the background traffic direction and the
/// per-UE transient traffic (Msg4 vs pending UL) exercised on top of contention resolution.
class base_scheduler_multiue_tdd_test : public base_scheduler_tdd_tester
{
protected:
  static constexpr unsigned msg4_size      = 128;
  static constexpr unsigned huge_buffer    = 10000000;
  static constexpr unsigned sr_grant_bytes = 512;
  static constexpr rnti_t   base_rnti      = to_rnti(0x4601);
  // Slots to wait after Msg4 delivery before releasing a UE, so the ConRes/Msg4 HARQ is auto-ACKed first. The
  // fallback common PUCCH lands at most max_k1 (=7, see ue_fallback_scheduler) slots after the PDSCH.
  static const unsigned ack_margin_slots = 8;

  enum class background_traffic_direction { dl_only, ul_only, bidir };

  // Force the reference deployment's cell config on top of the base params: 100 MHz, CORESET#0 index 13 (48 RB x 2
  // symbols), PUCCH F1 with 2 cyclic shifts and no OCC (so SR/HARQ F1 spread across the band edges), common SS#1 with 8
  // AL4 candidates, and the dedicated SS#2 trimmed so the monitored PDCCH candidates per slot stay within the 36 limit
  // for the 2-symbol CORESET#0.
  static common_tdd_tester_params deployment_cell_params(const multiue_tdd_test_params& p)
  {
    common_tdd_tester_params out;
    out.tdd_cfg           = p.tdd_cfg;
    out.min_k             = p.min_k;
    out.bw                = bs_channel_bandwidth::MHz100;
    out.cs_type           = common_tdd_tester_params::coreset_type::dur2;
    out.f1_nof_cyc_shifts = pucch_nof_cyclic_shifts::two;
    out.f1_occ_supported  = false;
    out.srs_period        = p.srs_period;
    return out;
  }

  explicit base_scheduler_multiue_tdd_test(const multiue_tdd_test_params& params_) :
    base_scheduler_tdd_tester(deployment_cell_params(params_)), test_params(params_)
  {
    // Derive timing quantities from the cell configuration.
    tdd_period_slots    = nof_slots_per_tdd_period(*cell_cfg().params.tdd_cfg);
    conres_window_slots = cell_cfg().params.ul_cfg_common.init_ul_bwp.rach_cfg_common->ra_con_res_timer.count() *
                          get_nof_slots_per_subframe(cell_cfg().scs_common());
    wave_period_slots = test_params.wave_period_in_slots;

    // Each wave launches ues_per_wave transient UEs; derive the wave count so the whole transient pool is exercised.
    wave_size = std::max(1U, test_params.ues_per_wave);
    nof_waves = (test_params.nof_transient_ues + wave_size - 1) / wave_size;

    // Transient UE indices/RNTIs occupy the range right after the background UEs.
    for (unsigned i = 0; i != test_params.nof_transient_ues; ++i) {
      transient_pool.push_back(to_du_ue_index(test_params.nof_background_ues + i));
    }
  }

  virtual ~base_scheduler_multiue_tdd_test() = default;

  // Traffic direction assigned to each background UE (called once per background UE), derived from the test parameter.
  // \c mixed draws a random direction per UE; the others force the configured direction.
  background_traffic_direction background_direction()
  {
    switch (test_params.background_traffic) {
      case multiue_bg_traffic::dl_only:
        return background_traffic_direction::dl_only;
      case multiue_bg_traffic::ul_only:
        return background_traffic_direction::ul_only;
      case multiue_bg_traffic::bidir:
        return background_traffic_direction::bidir;
      default:
        return static_cast<background_traffic_direction>(test_rng::uniform_int<unsigned>(0, 2));
    }
  }

  // Begin the transient UE's scenario-specific traffic right after it attaches (Msg4 or pending UL).
  virtual void start_transient_traffic(du_ue_index_t idx, rnti_t rnti) = 0;

  // Wait for and assert the transient UE's scenario-specific outcome (Msg4 delivered or UL granted).
  virtual async_task<void> verify_transient_traffic(du_ue_index_t idx, rnti_t rnti) = 0;

  // Run the overlapping-wave scenario and assert that no ConRes timer expired.
  void run_scenario()
  {
    add_background_ues();

    // Warmup so that the background traffic reaches steady-state saturation before waves start.
    for (unsigned i = 0; i != 2 * tdd_period_slots; ++i) {
      run_slot();
    }

    // Launch overlapping waves at a fixed phase; each transient UE runs its own async lifecycle task.
    for (unsigned slot_count = 0, launched_waves = 0, next_wave_count = 0; launched_waves < nof_waves; ++slot_count) {
      if (slot_count == next_wave_count) {
        launch_wave();
        ++launched_waves;
        next_wave_count += wave_period_slots;
      }
      run_slot();
    }

    // Drain all in-flight transient UE tasks (contention resolution, scenario traffic and confirmed removal).
    run_until_all_pending_tasks_completion();

    // Backstop: the scheduler must not have flagged any ConRes timer expiry over the whole run.
    request_metrics_on_next_slot();
    run_slot();
    ASSERT_TRUE(last_metrics().has_value());
    test_logger.info("Peak concurrent transient UEs in contention resolution: {}, failed fallback UCI allocs: {}",
                     peak_in_conres,
                     last_metrics()->failed_fallback_uci_allocs);
    ASSERT_EQ(last_metrics()->nof_conres_timer_expired, 0U) << "A ConRes timer expired before its ConRes CE";
    ASSERT_EQ(last_metrics()->nof_conres_ce_never_acked, 0U) << "A ConRes CE was scheduled but never ACKed";
  }

  // Add persistent UEs, each with a fixed traffic direction and a large buffer, to saturate the grid.
  void add_background_ues()
  {
    background_ues.resize(test_params.nof_background_ues);
    for (unsigned i = 0; i != test_params.nof_background_ues; ++i) {
      const du_ue_index_t idx  = to_du_ue_index(i);
      const rnti_t        rnti = to_rnti(to_value(base_rnti) + i);
      add_ue(build_ue_request(idx, rnti, {LCID_MIN_DRB}), false);
      background_ues[i] = background_direction();
    }
    push_huge_buffers_to_background_ues();
  }

  void push_huge_buffers_to_background_ues()
  {
    for (unsigned i = 0; i != test_params.nof_background_ues; ++i) {
      const du_ue_index_t idx  = to_du_ue_index(i);
      const rnti_t        rnti = to_rnti(to_value(base_rnti) + i);

      if (background_ues[i] != background_traffic_direction::ul_only) {
        push_dl_buffer_state(dl_buffer_state_indication_message{idx, LCID_MIN_DRB, huge_buffer});
      }
      if (background_ues[i] != background_traffic_direction::dl_only) {
        push_bsr(ul_bsr_indication_message{to_du_cell_index(0),
                                           idx,
                                           rnti,
                                           bsr_format::SHORT_BSR,
                                           {ul_bsr_lcg_report{uint_to_lcg_id(0), huge_buffer}}});
      }
    }
  }

  // Launch a wave of up to \c wave_size transient UEs, each driven by its own async task drawn from the shared pool.
  void launch_wave()
  {
    const unsigned to_launch = std::min<unsigned>(wave_size, transient_pool.size());
    if (to_launch < wave_size) {
      test_logger.info("Transient UE pool exhausted: launched {}/{} UEs in this wave", to_launch, wave_size);
    }
    std::vector<du_ue_index_t> wave_ues;
    wave_ues.reserve(to_launch);
    for (unsigned i = 0; i != to_launch; ++i) {
      wave_ues.push_back(transient_pool.back());
      transient_pool.pop_back();
    }

    // Inject the RACH for the whole wave in one occasion, so its RAR + Msg3 load lands together (as when a burst of UEs
    // ramps up at once). Each transient UE's task then waits for its own Msg3 before entering fallback.
    if (test_params.rach_driven) {
      push_wave_rach(wave_ues);
    }
    for (du_ue_index_t idx : wave_ues) {
      schedule_task(launch_transient_ue_task(idx));
    }

    // Just keep feeding the background UE buffers, so they don't deplete.
    push_huge_buffers_to_background_ues();
  }

  // Push a single RACH indication carrying one distinct preamble (TC-RNTI) per transient UE in the wave. The scheduler
  // answers with a RAR and a Msg3 PUSCH per preamble, loading the DL common search space and the (single) UL slot.
  void push_wave_rach(span<const du_ue_index_t> wave_ues)
  {
    ocudu_assert(wave_ues.size() <= MAX_PREAMBLES_PER_PRACH_OCCASION,
                 "Wave size {} exceeds the preambles available in one PRACH occasion",
                 wave_ues.size());
    std::vector<rach_indication_message::preamble> preambles;
    preambles.reserve(wave_ues.size());
    for (unsigned i = 0, e = wave_ues.size(); i != e; ++i) {
      preambles.push_back(test_helper::create_preamble(i, to_rnti(to_value(base_rnti) + to_value(wave_ues[i]))));
    }
    sched->handle_rach_indication(test_helper::create_rach_indication(next_slot_rx(), preambles));
  }

  // Async lifecycle of a transient UE: attach in fallback, complete contention resolution before the ConRes timer
  // expires, run the scenario-specific traffic, and detach; the index is returned to the pool only once the removal is
  // confirmed.
  async_task<void> launch_transient_ue_task(du_ue_index_t idx)
  {
    const rnti_t rnti      = to_rnti(to_value(base_rnti) + to_value(idx));
    auto         req       = build_ue_request(idx, rnti, {});
    req.starts_in_fallback = true;

    return launch_async([this, idx, rnti, req = std::move(req), ce_ok = false, msg3_ok = false](
                            coro_context<async_task<void>>& ctx) mutable {
      CORO_BEGIN(ctx);

      // In the RACH-driven scenario, wait for the RA scheduler to grant this UE's Msg3 PUSCH (its RAR was sent
      // after the wave's RACH). The UE only enters fallback once its Msg3 CCCH is "decoded".
      if (test_params.rach_driven) {
        CORO_AWAIT_VALUE(msg3_ok, launch_run_until(msg3_scheduled(rnti), conres_window_slots));
        EXPECT_TRUE(msg3_ok) << fmt::format("UE rnti={} did not get its Msg3 PUSCH granted", rnti);
      }

      // Create the UE in fallback (Msg3 CCCH decoded); the scheduler auto-injects the ConRes CE.
      req.ul_ccch_slot_rx = next_slot.without_hyper_sfn();
      CORO_AWAIT(launch_add_ue_task(req));
      ++cur_in_conres;
      peak_in_conres = std::max(peak_in_conres, cur_in_conres);

      start_transient_traffic(idx, rnti);

      // ConRes CE must be scheduled within the ConRes timer window.
      CORO_AWAIT_VALUE(ce_ok, launch_run_until(conres_ce_scheduled(rnti), conres_window_slots));
      --cur_in_conres;
      EXPECT_TRUE(ce_ok) << fmt::format("UE rnti={} did not schedule its ConRes CE before the timer expired", rnti);

      CORO_AWAIT(verify_transient_traffic(idx, rnti));

      // Let the ConRes HARQ be auto-ACKed, then release the UE.
      CORO_AWAIT(launch_run_until(false_until_slots(ack_margin_slots)));
      CORO_AWAIT(launch_rem_ue_task(idx));
      // Free the UE's PUCCH/SRS resources so the next transient UE reusing this index gets a fresh allocation.
      pucch_builder.dealloc_resources(req.cfg.cells.value()[0]);
      if (srs_builder != nullptr) {
        srs_builder->dealloc_resources(req.cfg.cells.value()[0]);
      }
      transient_pool.push_back(idx);

      CORO_RETURN();
    });
  }

  // Re-arm a small UL buffer (one SR's worth) each slot until the UE gets a UL grant, keeping the fallback UE's UL
  // pending without over-demanding the saturated grid.
  async_task<void> launch_ul_bsr_task(du_ue_index_t idx, rnti_t rnti)
  {
    return launch_async([this, idx, rnti, count = 0U](coro_context<async_task<void>>& ctx) mutable {
      CORO_BEGIN(ctx);
      for (count = 0; count != conres_window_slots; ++count) {
        if (find_ue_pusch(rnti, *last_sched_result()) != nullptr or find_ue_ul_pdcch(rnti) != nullptr) {
          break;
        }
        push_bsr(ul_bsr_indication_message{to_du_cell_index(0),
                                           idx,
                                           rnti,
                                           bsr_format::SHORT_BSR,
                                           {ul_bsr_lcg_report{uint_to_lcg_id(0), sr_grant_bytes}}});
        CORO_AWAIT(next_slot_signal);
      }
      CORO_RETURN();
    });
  }

  // Enqueue a Msg4 SRB0 buffer for \c idx after \c delay slots.
  async_task<void> launch_msg4_push_task(du_ue_index_t idx, unsigned delay)
  {
    return launch_async([this, idx, delay, count = 0U](coro_context<async_task<void>>& ctx) mutable {
      CORO_BEGIN(ctx);
      for (count = 0; count != delay; ++count) {
        CORO_AWAIT(next_slot_signal);
      }
      push_dl_buffer_state(dl_buffer_state_indication_message{idx, LCID_SRB0, msg4_size});
      CORO_RETURN();
    });
  }

  // Condition that stays false for \c nof_slots slots, used to wait a fixed number of slots inside a task.
  static unique_function<bool()> false_until_slots(unsigned nof_slots)
  {
    return [nof_slots, count = 0U]() mutable { return count++ >= nof_slots; };
  }

  // True once the RA scheduler has granted a Msg3 PUSCH for this TC-RNTI (its rnti matches the transient UE's rnti).
  unique_function<bool()> msg3_scheduled(rnti_t rnti) const
  {
    return [this, rnti]() { return find_ue_pusch(rnti, *last_sched_result()) != nullptr; };
  }

  unique_function<bool()> conres_ce_scheduled(rnti_t rnti) const
  {
    return [this, rnti]() {
      return find_ue_pdsch_with_lcid(rnti, lcid_dl_sch_t::UE_CON_RES_ID, *last_sched_result()) != nullptr;
    };
  }

  unique_function<bool()> srb0_scheduled(rnti_t rnti)
  {
    return [this, rnti]() { return find_ue_pdsch_with_lcid(rnti, LCID_SRB0, *last_sched_result()) != nullptr; };
  }

  unique_function<bool()> ul_grant_scheduled(rnti_t rnti)
  {
    return [this, rnti]() {
      return find_ue_pusch(rnti, *last_sched_result()) != nullptr or find_ue_ul_pdcch(rnti) != nullptr;
    };
  }

  multiue_tdd_test_params test_params;

  unsigned tdd_period_slots    = 0;
  unsigned conres_window_slots = 0;
  unsigned wave_period_slots   = 0;
  unsigned wave_size           = 0;
  unsigned nof_waves           = 0;

  // Instrumentation: peak number of transient UEs concurrently between fallback creation and ConRes-CE scheduling.
  unsigned cur_in_conres  = 0;
  unsigned peak_in_conres = 0;

  std::vector<du_ue_index_t>                transient_pool;
  std::vector<background_traffic_direction> background_ues;
};

/// \brief Contention-resolution stress: transient UEs exchange Msg4 (DL) under a mixed DL/UL background load, and must
/// complete ConRes before the timer expires.
class scheduler_multiue_conres_tdd_test : public base_scheduler_multiue_tdd_test,
                                          public ::testing::TestWithParam<multiue_tdd_test_params>
{
public:
  scheduler_multiue_conres_tdd_test() : base_scheduler_multiue_tdd_test(GetParam()) {}

protected:
  void start_transient_traffic(du_ue_index_t idx, rnti_t /* rnti */) override
  {
    // Push Msg4 after a random delay, so it may be multiplexed with the ConRes CE or sent separately.
    schedule_task(launch_msg4_push_task(idx, test_rng::uniform_int<unsigned>(0, 2 * tdd_period_slots)));
  }

  async_task<void> verify_transient_traffic(du_ue_index_t /* idx */, rnti_t rnti) override
  {
    return launch_async([this, rnti, msg4_ok = false](coro_context<async_task<void>>& ctx) mutable {
      CORO_BEGIN(ctx);
      CORO_AWAIT_VALUE(msg4_ok, launch_run_until(srb0_scheduled(rnti), conres_window_slots));
      EXPECT_TRUE(msg4_ok) << fmt::format("UE rnti={} did not get its Msg4 scheduled", rnti);
      CORO_RETURN();
    });
  }
};

TEST_P(scheduler_multiue_conres_tdd_test, all_ues_schedule_conres_before_timeout)
{
  run_scenario();
}

INSTANTIATE_TEST_SUITE_P(
    scheduler_tdd_test,
    scheduler_multiue_conres_tdd_test,
    testing::Values(
        // clang-format off
  // {base_scheduler_tdd_tester_params}, nof_background_ues, nof_transient_ues, ues_per_wave, wave_period_in_slots
  // DL-heavy DDDDDDSUUU.
  multiue_tdd_test_params{{subcarrier_spacing::kHz30, {10, 6, 5, 3, 4}}, 4,        8, 16, 2, 20},
  // DL-heavy DDDDDDSUUU with tight HARQ timing (min_k=2).
  multiue_tdd_test_params{{subcarrier_spacing::kHz30, {10, 6, 5, 3, 4}}, 2,        8, 16, 2, 20},
  // Very DL-heavy DDDDDDDSUU (2 UL slots per period).
  multiue_tdd_test_params{create_tdd_pattern(tdd_pattern_profile_fr1_30khz::DDDDDDDSUU), 4, 8, 16, 2, 20},
  // Balanced DDDSU.
  multiue_tdd_test_params{create_tdd_pattern(tdd_pattern_profile_fr1_30khz::DDDSU), 4, 8, 16, 1, 10},
  // Balanced DDDSU, back-to-back waves and heavier transient load.
  multiue_tdd_test_params{create_tdd_pattern(tdd_pattern_profile_fr1_30khz::DDDSU), 4, 12, 24, 1, 5},
  // UL-heavy DDDSUUUUUU.
  multiue_tdd_test_params{{subcarrier_spacing::kHz30, {10, 3, 5, 6, 0}}, 4,     8, 16, 2, 20},
  // DDSU with min_k=4.
  multiue_tdd_test_params{{subcarrier_spacing::kHz30, {4, 2, 9, 1, 0}}, 4,      8, 16, 1, 8},
  // Single-burst arrivals: ues_per_wave is derived from measured performance, not the 3GPP spec. Sitting right at the
  // ConRes-timer-expiry cliff made this case flaky across random seeds (rare PUCCH-layout draws still tip it over),
  // so a margin below the cliff is kept; if a scheduler change shifts the cliff, re-sweep across many
  // --gtest_random_seed values (not just a handful) and update this value to keep the same margin below the new one.
  multiue_tdd_test_params{create_tdd_pattern(tdd_pattern_profile_fr1_30khz::DDDSU), 4, 100, 22, 22, 5},
  multiue_tdd_test_params{create_tdd_pattern(tdd_pattern_profile_fr1_30khz::DDDSU), 2, 100, 85, 85, 5},
  multiue_tdd_test_params{create_tdd_pattern(tdd_pattern_profile_fr1_30khz::DDDDDDSUUU), 4, 100, 40, 40, 5},
  multiue_tdd_test_params{create_tdd_pattern(tdd_pattern_profile_fr1_30khz::DDDDDDSUUU), 2, 100, 60, 60, 5},
  // Sustained-stream arrivals (~waves of fallback UEs per TDD period): ues_per_wave is derived from measured
  // performance, not the 3GPP spec. Kept with a margin below the ConRes-timer-expiry cliff (sitting right on the
  // cliff flaked across random seeds); if a scheduler change shifts either cliff, re-sweep across many
  // --gtest_random_seed values (not just a handful) and update these values to keep the same margin below the new
  // one.
  multiue_tdd_test_params{create_tdd_pattern(tdd_pattern_profile_fr1_30khz::DDDSU), 4, 100, 100, 1, 5},
  multiue_tdd_test_params{create_tdd_pattern(tdd_pattern_profile_fr1_30khz::DDDSU), 2, 100, 100, 14, 5},
  multiue_tdd_test_params{create_tdd_pattern(tdd_pattern_profile_fr1_30khz::DDDDDDSUUU), 4, 100, 100, 2, 5},
  multiue_tdd_test_params{create_tdd_pattern(tdd_pattern_profile_fr1_30khz::DDDDDDSUUU), 2, 100, 100, 6, 5},
  multiue_tdd_test_params{create_tdd_pattern(tdd_pattern_profile_fr1_30khz::DDDSU), 4, 100, 100, 1, 5, true},
  // SRS enabled, RACH-driven Msg3: periodic SRS carves into the tail of the sole UL slot, narrowing the symbol window
  // left for Msg3 and UE PUSCHs.
  multiue_tdd_test_params{
      create_tdd_pattern(tdd_pattern_profile_fr1_30khz::DDDSU), 2, 8, 16, 1, 10, true, multiue_bg_traffic::mixed, srs_periodicity::sl5}
));
// clang-format on

/// \brief Fallback-UL starvation: the background UEs saturate only the UL, and each transient UE keeps a small pending
/// UL while in fallback that must still be granted a PUSCH. Reproduces the fallback-UL starvation caused by the
/// min_k1/min_k2 k2-asymmetry (DL-heavy pattern with small min_k).
class scheduler_multiue_ul_starvation_tdd_test : public base_scheduler_multiue_tdd_test,
                                                 public ::testing::TestWithParam<multiue_tdd_test_params>
{
public:
  scheduler_multiue_ul_starvation_tdd_test() : base_scheduler_multiue_tdd_test(GetParam()) {}

protected:
  static constexpr unsigned max_slots_until_fallback_ul = 40;

  void start_transient_traffic(du_ue_index_t idx, rnti_t rnti) override
  {
    // UL channel will be saturated.
    schedule_task(launch_ul_bsr_task(idx, rnti));
  }

  async_task<void> verify_transient_traffic(du_ue_index_t /* idx */, rnti_t rnti) override
  {
    return launch_async([this, rnti, ul_ok = false](coro_context<async_task<void>>& ctx) mutable {
      CORO_BEGIN(ctx);
      CORO_AWAIT_VALUE(ul_ok, launch_run_until(ul_grant_scheduled(rnti), max_slots_until_fallback_ul));
      EXPECT_TRUE(ul_ok) << fmt::format("UE rnti={} was starved of UL grants under the saturated grid", rnti);
      CORO_RETURN();
    });
  }
};

TEST_P(scheduler_multiue_ul_starvation_tdd_test, fallback_ue_is_served_under_saturated_ul_grid)
{
  run_scenario();
}

INSTANTIATE_TEST_SUITE_P(
    scheduler_tdd_test,
    scheduler_multiue_ul_starvation_tdd_test,
    testing::Values(
        // clang-format off
  // {base_scheduler_tdd_tester_params}, nof_background_ues, nof_transient_ues, ues_per_wave, wave_period_in_slots
  // DDDDDDSUUU (6 DL symbols in special slot), min_k sweep across the k2-asymmetry boundary.
  multiue_tdd_test_params{ {subcarrier_spacing::kHz30, {10, 6, 6, 3, 4}}, 1,        16, 8, 1, 20, false, multiue_bg_traffic::ul_only},
  multiue_tdd_test_params{ {subcarrier_spacing::kHz30, {10, 6, 6, 3, 4}}, 2,        16, 8, 1, 20, false, multiue_bg_traffic::ul_only},
  multiue_tdd_test_params{ {subcarrier_spacing::kHz30, {10, 6, 6, 3, 4}}, 3,        16, 8, 1, 20, false, multiue_bg_traffic::ul_only},
  // DDDDDDSUUU with 5 DL symbols in the special slot.
  multiue_tdd_test_params{ {subcarrier_spacing::kHz30, {10, 6, 5, 3, 4}}, 2,        16, 8, 1, 20, false, multiue_bg_traffic::ul_only},
  // DDSU (single full-UL slot per period) with min_k=4.
  multiue_tdd_test_params{ {subcarrier_spacing::kHz30, {4, 2, 9, 1, 0}}, 4,         16, 8, 1, 8, false, multiue_bg_traffic::ul_only},
  // DDDSU (single full-UL slot per period) with min_k=1. Considering it is a DL-heavy scenario, only k2=1 will be
  // used and PUSCHs will be scheduled from the special slot.
  multiue_tdd_test_params{ create_tdd_pattern(tdd_pattern_profile_fr1_30khz::DDDSU), 1, 16, 8, 1, 8, false, multiue_bg_traffic::ul_only},
  // SRS enabled on the UL-scarce patterns above: periodic SRS carves into the tail of the sole full-UL slot,
  // narrowing the symbol window left for the fallback UE's UL grant.
  multiue_tdd_test_params{ create_tdd_pattern(tdd_pattern_profile_fr1_30khz::DDDSU), 1, 16, 8, 1, 8, false, multiue_bg_traffic::ul_only, srs_periodicity::sl10},
  multiue_tdd_test_params{ {subcarrier_spacing::kHz30, {4, 2, 9, 1, 0}}, 4, 16, 8, 1, 8, false, multiue_bg_traffic::ul_only, srs_periodicity::sl16}
));
// clang-format on

} // namespace
