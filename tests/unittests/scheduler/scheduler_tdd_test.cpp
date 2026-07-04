// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

/// \file
/// \brief Unit test for scheduler using different TDD patterns.

#include "test_utils/scheduler_test_simulator.h"
#include "tests/test_doubles/scheduler/cell_config_builder_profiles.h"
#include "tests/test_doubles/scheduler/pucch_res_test_builder_helper.h"
#include "tests/test_doubles/scheduler/scheduler_config_helper.h"
#include "tests/test_doubles/utils/test_rng.h"
#include "ocudu/du/du_update_config_helpers.h"
#include "ocudu/ran/tdd/tdd_ul_dl_config_formatters.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace cell_config_builder_profiles;

struct tdd_test_params {
  bool                    csi_rs_enabled;
  tdd_ul_dl_config_common tdd_cfg;
  unsigned                min_k = 4;
};

class base_scheduler_tdd_tester : public scheduler_test_simulator
{
protected:
  // Default scheduler expert config for the TDD testers: remove the PRACH guardbands so PUSCH can fill the UL slot, and
  // raise the per-slot PUCCH / UL-grant ceilings so many UEs can be served in the single UL slot.
  static scheduler_expert_config default_expert_cfg()
  {
    auto expert_cfg                        = config_helpers::make_default_scheduler_expert_config();
    expert_cfg.ra.nof_prach_guardbands_rbs = 0;
    expert_cfg.ue.max_pucchs_per_slot      = 120;
    expert_cfg.ue.max_ul_grants_per_slot   = 140;
    return expert_cfg;
  }

  base_scheduler_tdd_tester(const tdd_test_params& testparams, bs_channel_bandwidth bw = bs_channel_bandwidth::MHz20) :
    scheduler_test_simulator(scheduler_test_sim_config{.sched_cfg = default_expert_cfg(),
                                                       .max_scs   = testparams.tdd_cfg.ref_scs,
                                                       .auto_uci  = true,
                                                       .auto_crc  = true})
  {
    ocudu_assert(testparams.tdd_cfg.ref_scs == subcarrier_spacing::kHz30, "Only 30kHz SCS is supported in this test");
    params                      = cell_config_builder_profiles::create(duplex_mode::TDD, frequency_range::FR1, bw);
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
    f1.nof_cyc_shifts                     = pucch_nof_cyclic_shifts::twelve;
    f1.occ_supported                      = true;
    // Increase PUCCH Format 2 code rate to support DL-heavy TDD configurations.
    std::get<pucch_f2_params>(pucch_params.f2_or_f3_or_f4_params).max_code_rate = max_pucch_code_rate::dot_35;
    cell_req.ran.init_bwp.pucch.resources                                       = pucch_params;
    // Place PRACH clear of the PUCCH resource pool (as the DU does), so a large PUCCH pool does not overlap the PRACH
    // occasion.
    cell_req.ran.ul_cfg_common.init_ul_bwp.rach_cfg_common->rach_cfg_generic.msg1_frequency_start =
        config_helpers::compute_prach_frequency_start(
            pucch_params, cell_req.ran.ul_cfg_common.init_ul_bwp.generic_params.crbs.length(), false);
    this->add_cell(cell_req);
  }

  // Build a UE creation request for this cell, validating the PUCCH Format 2 resource the TDD config relies on.
  sched_ue_creation_request_message
  create_ue_request(du_ue_index_t ue_index, rnti_t rnti, const std::initializer_list<lcid_t>& lcids)
  {
    auto ue_cfg     = sched_config_helper::create_default_sched_ue_creation_request(cell_cfg().params, lcids);
    ue_cfg.ue_index = ue_index;
    ue_cfg.crnti    = rnti;
    ocudu_assert((*ue_cfg.cfg.cells)[0].serv_cell_cfg.ul_config->init_ul_bwp.pucch_cfg.has_value(),
                 "The PUCCH config is not set");
    const auto&    pucch_cfg     = (*ue_cfg.cfg.cells)[0].serv_cell_cfg.ul_config->init_ul_bwp.pucch_cfg.value();
    pucch_res_id_t any_res_f2_id = pucch_cfg.pucch_res_set[1].resources.front();
    const auto*    res_f2        = std::find_if(pucch_cfg.pucch_res_list.begin(),
                                      pucch_cfg.pucch_res_list.end(),
                                      [any_res_f2_id](const auto& res) { return res.res_id == any_res_f2_id; });
    ocudu_assert(res_f2 != pucch_cfg.pucch_res_list.end(), "PUCCH resource F2 not found");
    return ue_cfg;
  }

  const du_ue_index_t ue_idx      = to_du_ue_index(0);
  const rnti_t        ue_rnti     = to_rnti(0x4601);
  const lcid_t        ue_drb_lcid = LCID_MIN_DRB;

  cell_config_builder_params params;
};

/// Formatter for test params.
void PrintTo(const tdd_test_params& value, ::std::ostream* os)
{
  *os << fmt::format(
      "csi={} tdd={} min_k={}", value.csi_rs_enabled ? "enabled" : "disabled", value.tdd_cfg, value.min_k);
}

class scheduler_dl_tdd_tester : public base_scheduler_tdd_tester, public ::testing::TestWithParam<tdd_test_params>
{
public:
  scheduler_dl_tdd_tester() : base_scheduler_tdd_tester(GetParam())
  {
    this->add_ue(create_ue_request(ue_idx, ue_rnti, {ue_drb_lcid}));
  }
};

TEST_P(scheduler_dl_tdd_tester, all_dl_slots_are_scheduled)
{
  // Enqueue enough bytes for continuous DL tx.
  dl_buffer_state_indication_message dl_buf_st{ue_idx, ue_drb_lcid, 10000000};
  this->push_dl_buffer_state(dl_buf_st);

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

class scheduler_ul_tdd_tester : public base_scheduler_tdd_tester, public ::testing::TestWithParam<tdd_test_params>
{
public:
  scheduler_ul_tdd_tester() : base_scheduler_tdd_tester(GetParam())
  {
    this->add_ue(create_ue_request(ue_idx, ue_rnti, {ue_drb_lcid}));

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

INSTANTIATE_TEST_SUITE_P(scheduler_tdd_test,
                         scheduler_dl_tdd_tester,
                         testing::Values(
                             // clang-format off
  // csi_enabled, {ref_scs, pattern1={slot_period, DL_slots, DL_symbols, UL_slots, UL_symbols}, pattern2={...}, min_k}
  tdd_test_params{true,  {subcarrier_spacing::kHz30, {10, 6, 5, 3, 4}}}, // DDDDDDSUUU
  // > TS 38.101-4, Table A.1.2-2: TDD UL-DL configuration for SCS 30kHz.
  tdd_test_params{true, create_tdd_pattern(tdd_pattern_profile_fr1_30khz::DDDDDDDSUU)},
  tdd_test_params{true, create_tdd_pattern(tdd_pattern_profile_fr1_30khz::DDDDDDDSUU), 2},
  tdd_test_params{true, create_tdd_pattern(tdd_pattern_profile_fr1_30khz::DDDSU)},
  tdd_test_params{true, create_tdd_pattern(tdd_pattern_profile_fr1_30khz::DDDSU), 2},
  tdd_test_params{true, create_tdd_pattern(tdd_pattern_profile_fr1_30khz::DDDSUDDSUU)},
  tdd_test_params{true, create_tdd_pattern(tdd_pattern_profile_fr1_30khz::DDDSUUDDDD)},
  tdd_test_params{true, create_tdd_pattern(tdd_pattern_profile_fr1_30khz::DSUU)},
  tdd_test_params{true, create_tdd_pattern(tdd_pattern_profile_fr1_30khz::DSSU)},
  // > Other DL-heavy patterns.
  tdd_test_params{false, {subcarrier_spacing::kHz30, {10, 8, 0, 1, 0}}},
  tdd_test_params{false, {subcarrier_spacing::kHz30, {10, 8, 0, 1, 0}}, 1},
  // >> DDDSUUUUDD
  tdd_test_params{true, {subcarrier_spacing::kHz30, {8,  3, 6, 4, 4}, tdd_ul_dl_pattern{2, 2, 0, 0, 0}}},
  tdd_test_params{true,  {subcarrier_spacing::kHz30, {4,  2, 9, 1, 0}}},
  tdd_test_params{true,  {subcarrier_spacing::kHz30, {4,  2, 9, 1, 0}}, 1},
  tdd_test_params{true,  {subcarrier_spacing::kHz30, {10, 6, 13, 3, 0}}, 4} // DDDDDDSUUU, with 13 DL symbols in special slot
  // TODO: Support more TDD patterns.
// Note: The params below lead to a failure due to "Not enough space in PUCCH". However, I don't think there is no valid
// k1 candidate list that accommodates all DL slots.
  //tdd_test_params{true, {subcarrier_spacing::kHz30, {10, 8, 5, 1, 4}}}
));
// clang-format on

INSTANTIATE_TEST_SUITE_P(
    scheduler_tdd_test,
    scheduler_ul_tdd_tester,
    testing::Values(
        // clang-format off
  // csi_enabled, {ref_scs, pattern1={slot_period, DL_slots, DL_symbols, UL_slots, UL_symbols}, pattern2={...}}
  tdd_test_params{true,  {subcarrier_spacing::kHz30, {10, 6, 5, 3, 4}}}, // DDDDDDSUUU
  // > TS 38.101-4, Table A.1.2-2: TDD UL-DL configuration for SCS 30kHz.
  tdd_test_params{true, create_tdd_pattern(tdd_pattern_profile_fr1_30khz::DDDDDDDSUU)},
  tdd_test_params{true, create_tdd_pattern(tdd_pattern_profile_fr1_30khz::DDDDDDDSUU), 2},
  tdd_test_params{true, create_tdd_pattern(tdd_pattern_profile_fr1_30khz::DDDSU)},
  tdd_test_params{true, create_tdd_pattern(tdd_pattern_profile_fr1_30khz::DDDSUDDSUU), 2},
  tdd_test_params{true, create_tdd_pattern(tdd_pattern_profile_fr1_30khz::DDDSUUDDDD)},
  tdd_test_params{true, create_tdd_pattern(tdd_pattern_profile_fr1_30khz::DSUU), 2},
  tdd_test_params{true, create_tdd_pattern(tdd_pattern_profile_fr1_30khz::DSSU)},
  // > Other DL heavy patterns
  tdd_test_params{true,  {subcarrier_spacing::kHz30, {10, 8, 5, 1, 4}}}, // DDDDDDDDSU
  // >> DDDSUUUUDD
  tdd_test_params{true, {subcarrier_spacing::kHz30, {8,  3, 6, 4, 4}, tdd_ul_dl_pattern{2, 2, 0, 0, 0}}},
  tdd_test_params{true,  {subcarrier_spacing::kHz30, {4,  2, 9, 1, 0}}},  // DDSU
  tdd_test_params{true,  {subcarrier_spacing::kHz30, {10, 4, 5, 5, 0}}, 5}, // DDDDSUUUUU
  tdd_test_params{true,  {subcarrier_spacing::kHz30, {10, 6, 13, 3, 0}}, 4}, // DDDDDDSUUU, with 13 DL symbols in special slot
  // UL heavy
  tdd_test_params{true, {subcarrier_spacing::kHz30, {10, 3,  5, 6, 0}}},
  tdd_test_params{true, {subcarrier_spacing::kHz30, {5,  1, 10, 3, 0}, tdd_ul_dl_pattern{5, 1, 10, 3, 0}}, 2},
  tdd_test_params{true, {subcarrier_spacing::kHz30, {6,  2, 10, 3, 0}, tdd_ul_dl_pattern{4, 1, 0, 3, 0}}, 2},
  tdd_test_params{true, {subcarrier_spacing::kHz30, {4,  1, 10, 2, 0}, tdd_ul_dl_pattern{6, 1, 10, 4, 0}}, 2},
  tdd_test_params{true, {subcarrier_spacing::kHz30, {10, 2, 10, 7, 0}}, 2},
  tdd_test_params{true, {subcarrier_spacing::kHz30, {10, 2,  6, 7, 4}}, 2},
  tdd_test_params{true, {subcarrier_spacing::kHz30, {10, 2,  6, 7, 4}}},
  tdd_test_params{true, {subcarrier_spacing::kHz30, {5,  1, 10, 3, 0}}, 2}
));
// clang-format on

// Parameters for the multi-UE TDD stress test.
struct multiue_tdd_test_params {
  tdd_test_params base;
  // Number of persistent UEs with constant traffic used to saturate the grid.
  unsigned nof_background_ues;
  // Maximum number of concurrent transient UEs (pool capacity shared across overlapping waves).
  unsigned nof_transient_ues;
  // Number of transient UE waves launched over the test.
  unsigned nof_waves;
  // Spacing between consecutive waves, expressed in TDD periods to keep the attach phase fixed.
  unsigned wave_period_in_tdd_periods;
};

void PrintTo(const multiue_tdd_test_params& value, ::std::ostream* os)
{
  *os << fmt::format("tdd={} bg_ues={} transient_ues={} waves={} wave_period_periods={}",
                     value.base.tdd_cfg,
                     value.nof_background_ues,
                     value.nof_transient_ues,
                     value.nof_waves,
                     value.wave_period_in_tdd_periods);
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

  explicit base_scheduler_multiue_tdd_test(const multiue_tdd_test_params& params_) :
    base_scheduler_tdd_tester(params_.base, bs_channel_bandwidth::MHz100), test_params(params_)
  {
    pucch_builder.setup(cell_cfg().params);

    // Derive timing quantities from the cell configuration.
    tdd_period_slots    = nof_slots_per_tdd_period(*cell_cfg().params.tdd_cfg);
    conres_window_slots = cell_cfg().params.ul_cfg_common.init_ul_bwp.rach_cfg_common->ra_con_res_timer.count() *
                          get_nof_slots_per_subframe(cell_cfg().scs_common());
    wave_period_slots = test_params.wave_period_in_tdd_periods * tdd_period_slots;

    // Bound the per-wave size so that, at worst-case dwell (a full ConRes window), the concurrent transient UEs never
    // exceed the pool capacity.
    const unsigned overlap_depth = (conres_window_slots + wave_period_slots - 1) / wave_period_slots;
    wave_size                    = std::max(1U, test_params.nof_transient_ues / std::max(1U, overlap_depth));

    // Transient UE indices/RNTIs occupy the range right after the background UEs.
    for (unsigned i = 0; i != test_params.nof_transient_ues; ++i) {
      transient_pool.push_back(to_du_ue_index(test_params.nof_background_ues + i));
    }
  }

  virtual ~base_scheduler_multiue_tdd_test() = default;

  // Traffic direction assigned to each background UE (called once per background UE).
  virtual background_traffic_direction background_direction() = 0;

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
    for (unsigned slot_count = 0, launched_waves = 0, next_wave_count = 0; launched_waves < test_params.nof_waves;
         ++slot_count) {
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
    ASSERT_EQ(last_metrics()->nof_conres_timer_expired, 0U) << "A ConRes timer expired during the test";
  }

  // Build a UE creation request, allocating a distinct dedicated PUCCH config (SR/CSI/HARQ) from the cell resource pool
  // so many UEs can coexist on the single UL slot.
  // Note: the builder never releases resources (the test helper exposes no dealloc), so the total number of UEs created
  // over a run (background + one per transient wave slot) must stay below the cell PUCCH pool capacity.
  sched_ue_creation_request_message
  build_ue_request(du_ue_index_t idx, rnti_t rnti, std::initializer_list<lcid_t> lcids)
  {
    auto ue_cfg     = sched_config_helper::create_default_sched_ue_creation_request(cell_cfg().params, lcids);
    ue_cfg.ue_index = idx;
    ue_cfg.crnti    = rnti;
    report_fatal_error_if_not(pucch_builder.add_build_new_ue_pucch_cfg((*ue_cfg.cfg.cells)[0]),
                              "Failed to allocate PUCCH resources for UE {}",
                              fmt::underlying(idx));
    return ue_cfg;
  }

  // Add persistent UEs, each with a fixed traffic direction and a large buffer, to saturate the grid.
  void add_background_ues()
  {
    background_ues.resize(test_params.nof_background_ues);
    for (unsigned i = 0; i != test_params.nof_background_ues; ++i) {
      const du_ue_index_t idx  = to_du_ue_index(i);
      const rnti_t        rnti = to_rnti(fmt::underlying(base_rnti) + i);
      add_ue(build_ue_request(idx, rnti, {LCID_MIN_DRB}), false);
      background_ues[i] = background_direction();
    }
    push_huge_buffers_to_background_ues();
  }

  void push_huge_buffers_to_background_ues()
  {
    for (unsigned i = 0; i != test_params.nof_background_ues; ++i) {
      const du_ue_index_t idx  = to_du_ue_index(i);
      const rnti_t        rnti = to_rnti(fmt::underlying(base_rnti) + i);

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
    for (unsigned i = 0; i != to_launch; ++i) {
      const du_ue_index_t idx = transient_pool.back();
      transient_pool.pop_back();
      schedule_task(launch_transient_ue_task(idx));
    }

    // Just keep feeding the background UE buffers, so they don't deplete.
    push_huge_buffers_to_background_ues();
  }

  // Async lifecycle of a transient UE: attach in fallback, complete contention resolution before the ConRes timer
  // expires, run the scenario-specific traffic, and detach; the index is returned to the pool only once the removal is
  // confirmed.
  async_task<void> launch_transient_ue_task(du_ue_index_t idx)
  {
    const rnti_t rnti      = to_rnti(fmt::underlying(base_rnti) + fmt::underlying(idx));
    auto         req       = build_ue_request(idx, rnti, {});
    req.starts_in_fallback = true;
    req.ul_ccch_slot_rx    = next_slot.without_hyper_sfn();

    return launch_async(
        [this, idx, rnti, req = std::move(req), ce_ok = false](coro_context<async_task<void>>& ctx) mutable {
          CORO_BEGIN(ctx);

          CORO_AWAIT(launch_add_ue_task(std::move(req)));

          start_transient_traffic(idx, rnti);

          // ConRes CE must be scheduled within the ConRes timer window.
          CORO_AWAIT_VALUE(ce_ok, launch_run_until(conres_ce_scheduled(rnti), conres_window_slots));
          EXPECT_TRUE(ce_ok) << fmt::format("UE rnti={} did not schedule its ConRes CE before the timer expired", rnti);

          CORO_AWAIT(verify_transient_traffic(idx, rnti));

          // Let the ConRes HARQ be auto-ACKed, then release the UE.
          CORO_AWAIT(launch_run_until(false_until_slots(ack_margin_slots)));
          CORO_AWAIT(launch_rem_ue_task(idx));
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

  std::vector<du_ue_index_t>                transient_pool;
  std::vector<background_traffic_direction> background_ues;

  // Builds a distinct per-UE dedicated PUCCH config from the cell pool for every UE created by these tests.
  pucch_res_builder_test_helper pucch_builder{120};
};

/// \brief Contention-resolution stress: transient UEs exchange Msg4 (DL) under a mixed DL/UL background load, and must
/// complete ConRes before the timer expires.
class scheduler_multiue_conres_tdd_test : public base_scheduler_multiue_tdd_test,
                                          public ::testing::TestWithParam<multiue_tdd_test_params>
{
public:
  scheduler_multiue_conres_tdd_test() : base_scheduler_multiue_tdd_test(GetParam()) {}

protected:
  background_traffic_direction background_direction() override
  {
    // Background UEs are a random combination of bi-dir, DL-only, UL-only.
    return static_cast<background_traffic_direction>(test_rng::uniform_int<unsigned>(0, 2));
  }

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
  // {tdd_test_params}, nof_background_ues, nof_transient_ues, nof_waves, wave_period_in_tdd_periods
  // DL-heavy DDDDDDSUUU.
  multiue_tdd_test_params{tdd_test_params{true, {subcarrier_spacing::kHz30, {10, 6, 5, 3, 4}}},           8, 16, 10, 2},
  // DL-heavy DDDDDDSUUU with tight HARQ timing (min_k=2).
  multiue_tdd_test_params{tdd_test_params{true, {subcarrier_spacing::kHz30, {10, 6, 5, 3, 4}}, 2},        8, 16, 10, 2},
  // Very DL-heavy DDDDDDDSUU (2 UL slots per period).
  multiue_tdd_test_params{tdd_test_params{true, create_tdd_pattern(tdd_pattern_profile_fr1_30khz::DDDDDDDSUU)}, 8, 16, 10, 2},
  // Balanced DDDSU.
  multiue_tdd_test_params{tdd_test_params{true, create_tdd_pattern(tdd_pattern_profile_fr1_30khz::DDDSU)}, 8, 16, 10, 2},
  // Balanced DDDSU, back-to-back waves and heavier transient load.
  multiue_tdd_test_params{tdd_test_params{true, create_tdd_pattern(tdd_pattern_profile_fr1_30khz::DDDSU)}, 12, 24, 12, 1},
  // UL-heavy DDDSUUUUUU.
  multiue_tdd_test_params{tdd_test_params{true, {subcarrier_spacing::kHz30, {10, 3, 5, 6, 0}}},           8, 16, 10, 2},
  // DDSU with min_k=4.
  multiue_tdd_test_params{tdd_test_params{true, {subcarrier_spacing::kHz30, {4, 2, 9, 1, 0}}, 4},         8, 16, 10, 2},
  // High-scale DDDSU: 100 dedicated UEs load the single UL slot while, in each wave, 50 fallback UEs attach
  // simultaneously and all must complete contention resolution. This stresses the common PUCCH that the fallback
  // ConRes ACKs share; kept below the cliff (~60-80 simultaneous fallback UEs here) where the common PUCCH can no
  // longer serve the burst in time and ConRes CEs start timing out.
  multiue_tdd_test_params{tdd_test_params{true, create_tdd_pattern(tdd_pattern_profile_fr1_30khz::DDDSU)}, 100, 50, 3, 26}
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

  // Saturate only the UL (as the dedicated UEs do), leaving the DL free for the transient UEs' ConRes CE.
  background_traffic_direction background_direction() override { return background_traffic_direction::ul_only; }

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
  // {tdd_test_params}, nof_background_ues, nof_transient_ues, nof_waves, wave_period_in_tdd_periods
  // DDDDDDSUUU (6 DL symbols in special slot), min_k sweep across the k2-asymmetry boundary.
  multiue_tdd_test_params{tdd_test_params{true, {subcarrier_spacing::kHz30, {10, 6, 6, 3, 4}}, 1},        16, 8, 6, 2},
  multiue_tdd_test_params{tdd_test_params{true, {subcarrier_spacing::kHz30, {10, 6, 6, 3, 4}}, 2},        16, 8, 6, 2},
  multiue_tdd_test_params{tdd_test_params{true, {subcarrier_spacing::kHz30, {10, 6, 6, 3, 4}}, 3},        16, 8, 6, 2},
  // DDDDDDSUUU with 5 DL symbols in the special slot.
  multiue_tdd_test_params{tdd_test_params{true, {subcarrier_spacing::kHz30, {10, 6, 5, 3, 4}}, 2},        16, 8, 6, 2},
  // DDSU (single full-UL slot per period) with min_k=4.
  multiue_tdd_test_params{tdd_test_params{true, {subcarrier_spacing::kHz30, {4, 2, 9, 1, 0}}, 4},         16, 8, 6, 2}
));
// clang-format on
