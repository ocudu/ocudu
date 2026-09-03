// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "lib/scheduler/support/pusch_power_controller.h"
#include "tests/test_doubles/utils/test_rng.h"
#include "tests/unittests/scheduler/test_utils/sched_custom_test_bench.h"
#include "ocudu/ran/power_control/tpc_mapping.h"
#include "ocudu/scheduler/config/scheduler_expert_config_factory.h"
#include <gtest/gtest.h>

using namespace ocudu;

static cell_ph_report make_phr_report(int ph_lower_bound)
{
  // We assume the UE always reports the maximum power.
  // In the PH interval reported, we set the upper bound only 1dB higher than the lower bound, as it's the former the
  // one we are interested in.
  return cell_ph_report{.serv_cell_id = SERVING_PCELL_IDX,
                        .ph_type      = ph_field_type_t::type1,
                        .ph           = {ph_lower_bound, ph_lower_bound + 1},
                        .p_cmax       = std::optional<p_cmax_dbm_range>({22, 23})};
}

namespace pusch_pw_ctrl_test {

struct pusch_pw_ctrl_params {
  unsigned phr_periodic_timer_sf    = 2;
  bool     two_ul_slots_per_pattern = false;
};

// Dummy operator to avoid Valgrind warnings.
std::ostream& operator<<(std::ostream& os, const pusch_pw_ctrl_params& params)
{
  os << "PHR_timer_" << params.phr_periodic_timer_sf << "_SF__"
     << (params.two_ul_slots_per_pattern ? "2_UL_slots" : "3_UL_slots");
  return os;
}

class phr_ul_power_control_test_bench : public ::testing::TestWithParam<pusch_pw_ctrl_params>,
                                        public sched_basic_custom_test_bench
{
protected:
  phr_ul_power_control_test_bench() :
    sched_basic_custom_test_bench(
        []() {
          scheduler_expert_config exp_cfg                     = config_helpers::make_default_scheduler_expert_config();
          exp_cfg.ue.ul_power_ctrl.enable_pusch_cl_pw_control = false;
          exp_cfg.ue.ul_power_ctrl.enable_phr_bw_adaptation   = true;
          exp_cfg.ue.ul_power_ctrl.target_pusch_sinr          = default_pusch_sinr;
          return exp_cfg;
        }(),
        cell_config_builder_params{.scs_common = subcarrier_spacing::kHz30,
                                   .dl_carrier = {.carrier_bw = bs_channel_bandwidth::MHz20, .arfcn_f_ref = 520000U}}),
    phr_periodic_timer_sl(GetParam().phr_periodic_timer_sf * NOF_SLOT_PER_SF)
  {
    add_ue(cfg_mng.get_default_ue_config_request());
  }

  ~phr_ul_power_control_test_bench() { ocudulog::flush(); }

  // Basic constants for the test.
  static constexpr interval<unsigned> PUSCH_TO_PHR_DELAY_BOUNDS = {2U, 10U};
  static constexpr unsigned           MAX_PUSCH_ENTRIES         = PUSCH_TO_PHR_DELAY_BOUNDS.stop();
  static constexpr unsigned           TDD_PATTERN_LENGTH        = 10U;
  // We test this with TDD period of 10 slots per pattern, 30kHz SCS.
  static constexpr unsigned NOF_SLOT_PER_SF    = 20U;
  static constexpr float    default_pusch_sinr = 15.0f;

  void SetUp() override
  {
    // Code here will be called immediately after the constructor (right
    // before each test).
    auto* test_ue = ues.find(to_du_ue_index(0));
    ue_rnti       = test_ue->crnti;
    ocudu_assert(test_ue != nullptr, "UE not found in the UE repository");
    ch_state_manager   = &(test_ue->get_pcell().channel_state_manager());
    ul_pw_ctrl_manager = &(test_ue->get_pcell().get_pusch_power_controller());
    ocudulog::init();
    mac_logger.set_level(ocudulog::basic_levels::warning);
    test_logger.set_level(ocudulog::basic_levels::info);
  }

  const unsigned phr_periodic_timer_sl;

  // Keep track of the PUSCH PRBs allocations
  struct pusch_prbs_entry {
    slot_point slot_rx;
    unsigned   nof_prbs;
  };
  // We need to save at as many PUSCH txs as the PUSCH to PHR delay, to be able to check the number of PRBs allocated.
  // This is to assess the corresponding allocation in the Channel State manager.
  circular_array<pusch_prbs_entry, MAX_PUSCH_ENTRIES> pusch_nof_prbs_grid;

  ue_channel_state_manager* ch_state_manager   = nullptr;
  pusch_power_controller*   ul_pw_ctrl_manager = nullptr;

  rnti_t ue_rnti = rnti_t::INVALID_RNTI;

  // Struct to save the latest PHR report.
  struct ue_phr_report {
    cell_ph_report last_phr_report;
    float          ph_normalized;
  };
  std::optional<ue_phr_report> latest_phr;

  ocudulog::basic_logger& mac_logger  = ocudulog::fetch_basic_logger("SCHED", false);
  ocudulog::basic_logger& test_logger = ocudulog::fetch_basic_logger("TEST", true);
};

TEST_P(phr_ul_power_control_test_bench, pw_control_reduces_prbs_when_estimated_ph_value_is_negative)
{
  // This test is with closed-loop power control disabled.

  // PH value (in dBm) that is reported in the PHR.
  std::optional<int> ph_value_to_report = std::nullopt;
  // Slot at which the PHR is received.
  std::optional<slot_point> sl_phr = std::nullopt;

  // Generate random PUSCH to PHR delay.
  auto pusch_to_phr_delay =
      test_rng::uniform_int<unsigned>(PUSCH_TO_PHR_DELAY_BOUNDS.start(), PUSCH_TO_PHR_DELAY_BOUNDS.stop() - 1);
  test_logger.debug("Initial PUSCH to PHR delay: {}", pusch_to_phr_delay);

  slot_point sl = slot_point(to_numerology_value(subcarrier_spacing::kHz30), 0U);

  // Simulate at 100 PHR periods.
  for (unsigned sl_cnt = 0, sl_max = phr_periodic_timer_sl * 40; sl_cnt != sl_max; ++sl_cnt, ++sl) {
    // > If the PHR timer expires, generate the PH value to report and the PUSCH-to-PHR delay.
    if (sl.count() % phr_periodic_timer_sl == 0) {
      // Set a random value from -20 to 10 dB for the PH value to report. This is to simulate both cases of positive and
      // negative PHRs.
      ph_value_to_report.emplace(test_rng::uniform_int<int>(-20, 10));
      pusch_to_phr_delay =
          test_rng::uniform_int<unsigned>(PUSCH_TO_PHR_DELAY_BOUNDS.start(), PUSCH_TO_PHR_DELAY_BOUNDS.stop() - 1);
      test_logger.debug(
          "Slot={}: Generating PHR={}dBm PUSCH-PHR delay={}", sl, ph_value_to_report.value(), pusch_to_phr_delay);
    }

    // > At UL slots, check if the PUSCH transmission is to be performed.
    const unsigned slot_idx_tdd_slot = sl.count() % TDD_PATTERN_LENGTH;
    const bool     is_tdd_slot = GetParam().two_ul_slots_per_pattern ? slot_idx_tdd_slot > 7U : slot_idx_tdd_slot > 6U;
    if (is_tdd_slot) {
      // Simulate a PUSCH transmission with probability 0.5 at every UL slot in the TDD pattern. For large PHR periods,
      // reduce the probability.
      const float PUSCH_tx_prob = GetParam().phr_periodic_timer_sf < 50 ? 0.5f : 0.1f;
      if (PUSCH_tx_prob) {
        // Generate a random number of PRBs from 1 to 50.
        const auto nof_prbs = test_rng::uniform_int<unsigned>(1, 50);

        // We can check the number of PRBs allocated to the PUSCH only if we have received a PHR report.
        if (latest_phr.has_value()) {
          ch_state_manager->update_pusch_snr(default_pusch_sinr);
          uint8_t tpc = ul_pw_ctrl_manager->compute_tpc_command(sl);

          // Compute the number of PRBs to allocate to the PUSCH based on the PHR value.
          const auto updated_pusch_nof_prbs = ul_pw_ctrl_manager->adapt_pusch_prbs_to_phr(nof_prbs);
          test_logger.debug(
              "Slot={}: TPC command={} Updated PUSCH PRBs from {} to {}", sl, tpc, nof_prbs, updated_pusch_nof_prbs);

          // Verify whether the number of PRBs to allocate to the PUSCH based on the PHR value is correct.
          constexpr float min_pusch_nof_prbs = 1.0f;
          const float     est_nof_prbs = std::max(convert_dB_to_power(latest_phr->ph_normalized), min_pusch_nof_prbs);
          const unsigned  actual_nof_prbs = std::min(static_cast<unsigned>(std::floor(est_nof_prbs)), nof_prbs);
          test_logger.debug("Slot={}: Previously reported PHR={} normalized PRBs={}, EXPECTED_PRBs={}",
                            sl,
                            latest_phr->last_phr_report.ph,
                            latest_phr->ph_normalized,
                            actual_nof_prbs);
          // This is to prevent the float approximation in due to log and floor operations from affecting the test.
          const float espilon_float_approx    = 0.01f;
          const bool  has_float_approx        = std::ceil(est_nof_prbs) - est_nof_prbs < espilon_float_approx;
          const bool  expected_nof_prbs_match = has_float_approx ? (updated_pusch_nof_prbs >= actual_nof_prbs and
                                                                   updated_pusch_nof_prbs <= actual_nof_prbs + 1)
                                                                 : updated_pusch_nof_prbs == actual_nof_prbs;
          ASSERT_TRUE(expected_nof_prbs_match);
          const float epsilon_error_check = has_float_approx ? 1.0f : 0.0f;
          ASSERT_LE(static_cast<float>(updated_pusch_nof_prbs),
                    std::max(convert_dB_to_power(latest_phr->ph_normalized) + epsilon_error_check, min_pusch_nof_prbs));

          // Save the number of PRBs allocated to the PUSCH, both in the channel state manager and in the test-bench.
          ul_pw_ctrl_manager->update_pusch_pw_ctrl_state(sl, updated_pusch_nof_prbs);
          pusch_nof_prbs_grid[sl.count() % pusch_nof_prbs_grid.size()] = pusch_prbs_entry{sl, updated_pusch_nof_prbs};
        } else {
          ch_state_manager->update_pusch_snr(default_pusch_sinr);
          uint8_t tpc = ul_pw_ctrl_manager->compute_tpc_command(sl);

          const auto updated_pusch_nof_prbs = ul_pw_ctrl_manager->adapt_pusch_prbs_to_phr(nof_prbs);
          test_logger.debug("Slot={}: Init step: TPC command={}, Updated PUSCH PRBs from {} to {}",
                            sl,
                            tpc,
                            nof_prbs,
                            updated_pusch_nof_prbs);
          ASSERT_EQ(updated_pusch_nof_prbs, nof_prbs);
          // Save the number of PRBs allocated to the PUSCH, both in the channel state manager and in the test-bench.
          ul_pw_ctrl_manager->update_pusch_pw_ctrl_state(sl, updated_pusch_nof_prbs);
          pusch_nof_prbs_grid[sl.count() % pusch_nof_prbs_grid.size()] = pusch_prbs_entry{sl, nof_prbs};
        }
        // If there is a PH value to report, save the slot at which the PHR is received.
        if (ph_value_to_report.has_value() and not sl_phr.has_value()) {
          sl_phr.emplace(sl);
          test_logger.debug("Slot={}: Next PHR will sent this slot", sl);
        }
      }
    }

    // Report PHR indication.
    if (sl_phr.has_value() and sl == sl_phr.value() + pusch_to_phr_delay and ph_value_to_report.has_value()) {
      // Retrieve the saved PUSCH PRBs allocation for the slot at which the PHR is received.
      const slot_point saved_phr_slot = sl_phr.value();
      const auto*      pusch_prb_it =
          std::find_if(pusch_nof_prbs_grid.begin(), pusch_nof_prbs_grid.end(), [saved_phr_slot](const auto& entry) {
            return entry.slot_rx == saved_phr_slot;
          });
      ASSERT_NE(pusch_prb_it, pusch_nof_prbs_grid.end());
      test_logger.debug(
          "Slot={}: Saved PUSCH data [slot_rx={} nof_prbs={}]", sl, pusch_prb_it->slot_rx, pusch_prb_it->nof_prbs);

      // If the number of PRBs is less than or equal to 10, recompute the PH value to report and make it non-negative.
      // This is to simulate the fact that, when there PUSCH is over few PRBs, it's less likely that the PHR is
      // negative. Without this condition, the test would have too often negative PHRs resulting with the min usable
      // grant of 1 PRB.
      constexpr unsigned min_n_prbs_negative_ph = 10U;
      const int          ph_value = pusch_prb_it->nof_prbs >= min_n_prbs_negative_ph ? ph_value_to_report.value()
                                                                                     : test_rng::uniform_int<int>(0, 10);
      if (pusch_prb_it->nof_prbs < min_n_prbs_negative_ph) {
        test_logger.debug("Slot={}: Recomputed PH value={}dBm", sl, ph_value);
      }
      const auto phr_to_handle = make_phr_report(ph_value);
      ul_pw_ctrl_manager->handle_phr(phr_to_handle, sl_phr.value(), ue_rnti);
      test_logger.debug("Slot={}: Handle PHR [PHR={} slot_rx={}]", sl, phr_to_handle.ph, sl_phr.value());

      // Save the PHR and the derived parameters. This step is necessary to assess the correctness of the function
      // adapt_pusch_prbs_to_phr().
      latest_phr.emplace(ue_phr_report{phr_to_handle,
                                       static_cast<float>(ph_value) +
                                           convert_power_to_dB(static_cast<float>(pusch_prb_it->nof_prbs))});
      test_logger.debug("Slot={}: Saved TEST PHR [PHR={} slot_rx={} PUSCH_prbs={} norm_PHR={} max_PRBs={}]",
                        sl,
                        phr_to_handle.ph,
                        sl_phr.value(),
                        pusch_prb_it->nof_prbs,
                        static_cast<float>(ph_value) + convert_power_to_dB(static_cast<float>(pusch_prb_it->nof_prbs)),
                        convert_dB_to_power(static_cast<float>(ph_value) +
                                            convert_power_to_dB(static_cast<float>(pusch_prb_it->nof_prbs))));
      // Reset the slot and value for PHR. This will be set again in the next opportunity
      sl_phr.reset();
      ph_value_to_report.reset();
    }
  }
}

INSTANTIATE_TEST_SUITE_P(test_ul_power_control_phr,
                         phr_ul_power_control_test_bench,
                         testing::Values(pusch_pw_ctrl_params{10, false},
                                         pusch_pw_ctrl_params{20, false},
                                         pusch_pw_ctrl_params{50, false},
                                         pusch_pw_ctrl_params{10, true},
                                         pusch_pw_ctrl_params{20, true},
                                         pusch_pw_ctrl_params{50, true}),
                         [](const testing::TestParamInfo<phr_ul_power_control_test_bench::ParamType>& info_) {
                           return fmt::format("PHR_timer_{}_SF__{}",
                                              info_.param.phr_periodic_timer_sf,
                                              (info_.param.two_ul_slots_per_pattern ? "2_UL_slots" : "3_UL_slots"));
                         });

struct cl_pw_ctrl_params {
  float target_sinr;
  alpha pusch_pw_ctrl_alpha;
};

// Dummy operator to avoid Valgrind warnings.
std::ostream& operator<<(std::ostream& os, const cl_pw_ctrl_params& params)
{
  return os;
}

} // namespace pusch_pw_ctrl_test

using namespace pusch_pw_ctrl_test;

class pusch_closed_loop_power_control_base_test_bench : public sched_basic_custom_test_bench
{
protected:
  pusch_closed_loop_power_control_base_test_bench(float target_sinr_, alpha alpha_pw_ctrl_) :
    sched_basic_custom_test_bench(
        [target_sinr_]() {
          scheduler_expert_config exp_cfg                     = config_helpers::make_default_scheduler_expert_config();
          exp_cfg.ue.ul_power_ctrl.enable_pusch_cl_pw_control = true;
          exp_cfg.ue.ul_power_ctrl.target_pusch_sinr          = target_sinr_;
          return exp_cfg;
        }(),
        cell_config_builder_params{.scs_common = subcarrier_spacing::kHz30,
                                   .dl_carrier = {.carrier_bw = bs_channel_bandwidth::MHz20, .arfcn_f_ref = 520000U}}),
    pusch_sinr_target_dB(target_sinr_)
  {
    ocudulog::init();
    mac_logger.set_level(ocudulog::basic_levels::warning);
  }

  static constexpr interval<unsigned> PUSCH_TO_PHR_DELAY_BOUNDS = {2U, 10U};
  static constexpr unsigned           MAX_PUSCH_ENTRIES         = PUSCH_TO_PHR_DELAY_BOUNDS.stop();
  static constexpr unsigned           TDD_PATTERN_LENGTH        = 10U;
  static constexpr unsigned           NOF_PUSCH_PRBS            = 20U;

  void slot_indication()
  {
    // This approximates small SINR variations over time.
    sinr = test_rng::normal_dist<float>(initial_pusch_sinr_dB, 0.5f);
  }

  // Keep track of the PUSCH power control parameters.
  struct pusch_prbs_entry {
    slot_point slot_rx;
    unsigned   nof_prbs;
    int        f_cl_pw_control;
  };
  // We need to save at as many PUSCH txs as the PUSCH to PHR delay, to be able to check the number of PRBs allocated.
  // This is to assess the corresponding allocation in the Channel State manager.
  circular_array<pusch_prbs_entry, MAX_PUSCH_ENTRIES> pusch_nof_prbs_grid;

  const p_cmax_dbm_range p_cmax_dbm{22, 23};
  const unsigned         phr_periodic_timer_sl = 10;

  ue_channel_state_manager* ch_state_manager   = nullptr;
  pusch_power_controller*   ul_pw_ctrl_manager = nullptr;

  // PUSCH SINR target that would be achieved with closed-loop power control at the reference path-loss \c
  // ref_path_loss_for_target_sinr.
  const float pusch_sinr_target_dB;
  // Starting average SINR value that the UE would experience without closed-loop power control, with path-loss =
  // ref_path_loss_for_target_sinr. The closed-loop power control allows the actual UE's PUSCH SINR to change from this
  // value to the target SINR value.
  float initial_pusch_sinr_dB;
  // This adds a Normal Distribution to \c initial_pusch_sinr_dB to simulate small SINR variations over time.
  float sinr;
  float pw_per_prb;
  // Parameter that defines the Fractional path-loss compensation.
  float alpha_fractional_pl;
  // Actual path-loss for the UE; this is set as a random variable for each test and determines \c
  // pl_dependent_pusch_sinr_target_dB.
  float    actual_path_loss_dB;
  unsigned pusch_to_phr_delay;
};

class pusch_closed_loop_power_control_test_bench : public ::testing::TestWithParam<cl_pw_ctrl_params>,
                                                   public pusch_closed_loop_power_control_base_test_bench
{
protected:
  pusch_closed_loop_power_control_test_bench() :
    pusch_closed_loop_power_control_base_test_bench(GetParam().target_sinr, GetParam().pusch_pw_ctrl_alpha),
    average_reported_sinr_dB(0.5f)
  {
    sched_ue_creation_request_message ue_req = cfg_mng.get_default_ue_config_request();

    // Set alpha value for Fractional path-loss compensation.
    ue_req.cfg.cells.value()
        .front()
        .serv_cell_cfg.ul_config.value()
        .init_ul_bwp.pusch_cfg.value()
        .pusch_pwr_ctrl.value()
        .p0_alphasets.front()
        .p0_pusch_alpha = GetParam().pusch_pw_ctrl_alpha;
    alpha_fractional_pl = alpha_to_float(GetParam().pusch_pw_ctrl_alpha);

    // Set the reference path-loss for the target SINR.
    ref_path_loss_for_target_sinr =
        config_helpers::make_default_scheduler_expert_config().ue.ul_power_ctrl.path_loss_for_target_pusch_sinr;

    // Set the PUSCH power control parameters.
    pw_per_prb = static_cast<int>(
        cell_cfg.params.ul_cfg_common.init_ul_bwp.pusch_cfg_common.value().p0_nominal_with_grant.value() +
        static_cast<int>(ue_req.cfg.cells.value()
                             .front()
                             .serv_cell_cfg.ul_config.value()
                             .init_ul_bwp.pusch_cfg.value()
                             .pusch_pwr_ctrl.value()
                             .p0_alphasets.front()
                             .p0.value()));

    actual_path_loss_dB = static_cast<float>(test_rng::uniform_int(60, 100));

    // This is the maximum value of the closed-loop power control parameter that prevents negative PHR.
    f_cl_pw_control_max =
        p_cmax_dbm.start() -
        static_cast<int>(std::round(pw_per_prb + convert_power_to_dB(static_cast<float>(NOF_PUSCH_PRBS)) +
                                    alpha_fractional_pl * actual_path_loss_dB));

    // The actual SINR target is computed based on the path loss and the target SINR.
    pl_dependent_pusch_sinr_target_dB =
        pusch_sinr_target_dB + (1 - alpha_fractional_pl) * (ref_path_loss_for_target_sinr - actual_path_loss_dB);

    // Suppose the intial SINR is the target SINR plus a random value between -15 and 15 dB.
    initial_pusch_sinr_dB = pusch_sinr_target_dB + static_cast<float>(test_rng::uniform_int(-15, 15));

    // Recompute the SINR and path-loss values until the condition is satisfied. This ensures the PHR will be positive.
    while (pl_dependent_pusch_sinr_target_dB > initial_pusch_sinr_dB + f_cl_pw_control_max) {
      actual_path_loss_dB = static_cast<float>(test_rng::uniform_int(60, 100));
      f_cl_pw_control_max =
          p_cmax_dbm.start() -
          static_cast<int>(std::round(pw_per_prb + convert_power_to_dB(static_cast<float>(NOF_PUSCH_PRBS)) +
                                      alpha_fractional_pl * actual_path_loss_dB));
      pl_dependent_pusch_sinr_target_dB =
          pusch_sinr_target_dB + (1 - alpha_fractional_pl) * (ref_path_loss_for_target_sinr - actual_path_loss_dB);
      initial_pusch_sinr_dB = pusch_sinr_target_dB + static_cast<float>(test_rng::uniform_int(-15, 15));
    }

    // Generate random PUSCH to PHR delay.
    pusch_to_phr_delay =
        test_rng::uniform_int<unsigned>(PUSCH_TO_PHR_DELAY_BOUNDS.start(), PUSCH_TO_PHR_DELAY_BOUNDS.stop() - 1);

    add_ue(ue_req);
    sinr = initial_pusch_sinr_dB;
  }

  void SetUp() override
  {
    auto* test_ue = ues.find(to_du_ue_index(0));
    ue_rnti       = test_ue->crnti;
    ocudu_assert(test_ue != nullptr, "UE not found in the UE repository");
    ch_state_manager   = &(test_ue->get_pcell().channel_state_manager());
    ul_pw_ctrl_manager = &(test_ue->get_pcell().get_pusch_power_controller());
  }

  // Object that keeps track of the average SINR reported by the UE.
  exp_average_fast_start<float> average_reported_sinr_dB;

  rnti_t ue_rnti = rnti_t::INVALID_RNTI;

  // Reference path-loss, in dB, for which the UE would achieve the target SINR \c pusch_sinr_target_dB through
  // closed-loop power control.
  float ref_path_loss_for_target_sinr;
  // PUSCH SINR target that depends on the UE's path-loss to be achieved with closed-loop power control.
  float pl_dependent_pusch_sinr_target_dB;
  // Max value of the closed-loop power control parameter that prevents negative PHR.
  float f_cl_pw_control_max;
};

TEST_P(pusch_closed_loop_power_control_test_bench, with_cl_pw_control_pusch_reaches_its_target_sinr)
{
  // This tests the closed-loop power control for the PUSCH. The UE is starts at a given average SINR \c
  // initial_pusch_sinr_dB. Through Closed-loop power control, after some time, the UE is expected to reach the target
  // SINR \c pl_dependent_pusch_sinr_target_dB, which depends on its path-loss.

  // Flag to indicate there is a PHR to be reported.
  bool phr_to_report = false;
  // Slot at which the PHR is received.
  std::optional<slot_point> sl_phr = std::nullopt;
  // Keeps track of the SINR value that is reported to the UE.
  float f_cl_pw_control = 0;

  // Used to indicate if the test has been successful.
  bool convergence_reached = false;

  slot_point sl = slot_point(to_numerology_value(subcarrier_spacing::kHz30), 0U);
  for (unsigned sl_cnt = 0, sl_max = 10000; sl_cnt != sl_max; ++sl_cnt, ++sl) {
    // > Update the SINR value for the slot.
    // Compute the SINR with the path loss and the power control.
    slot_indication();
    // Instantaneous SINR that would be achieved without closed-loop power control for the given path loss.
    const float sinr_no_pw_ctrl =
        sinr + (1 - alpha_fractional_pl) * (ref_path_loss_for_target_sinr - actual_path_loss_dB);

    // > If the PHR timer expires, generate the PUSCH-to-PHR delay.
    if (sl.count() % phr_periodic_timer_sl == 0) {
      phr_to_report = true;
      pusch_to_phr_delay =
          test_rng::uniform_int<unsigned>(PUSCH_TO_PHR_DELAY_BOUNDS.start(), PUSCH_TO_PHR_DELAY_BOUNDS.stop() - 1);
    }

    // > At UL slots, check if the PUSCH transmission is to be performed.
    const unsigned slot_idx_tdd_slot = sl.count() % TDD_PATTERN_LENGTH;
    const bool     is_tdd_slot       = slot_idx_tdd_slot > 7U;
    if (is_tdd_slot) {
      // Simulate a PUSCH transmission with probability 0.5 at every UL slot in the TDD pattern.
      if (test_rng::bernoulli(0.5)) {
        // We assume the instantaneous SINR that is reported to the UE is the one achieved with the power control.
        const float sinr_pw_control = sinr_no_pw_ctrl + f_cl_pw_control;
        average_reported_sinr_dB.push(sinr_pw_control);
        ch_state_manager->update_pusch_snr(sinr_pw_control);
        uint8_t tpc_cmd = ul_pw_ctrl_manager->compute_tpc_command(sl);
        // We assume the number of PRBs is constant and equal to 20.
        const auto updated_pusch_nof_prbs = ul_pw_ctrl_manager->adapt_pusch_prbs_to_phr(NOF_PUSCH_PRBS);

        // Save the number of PRBs allocated to the PUSCH, both in the channel state manager and in the test-bench.
        ul_pw_ctrl_manager->update_pusch_pw_ctrl_state(sl, updated_pusch_nof_prbs);
        // Save the Closed-loop power control value for the slot; this is used in the test to generate the PHR value.
        f_cl_pw_control += tpc_mapping(tpc_cmd);

        pusch_nof_prbs_grid[sl.count() % pusch_nof_prbs_grid.size()] =
            pusch_prbs_entry{sl, updated_pusch_nof_prbs, static_cast<int>(f_cl_pw_control)};

        // If there is a PH value to report, save the slot at which the PHR is received.
        if (phr_to_report and not sl_phr.has_value()) {
          sl_phr.emplace(sl);
        }
      }
    }

    // > Report PHR indication.
    if (sl_phr.has_value() and sl == sl_phr.value() + pusch_to_phr_delay and phr_to_report) {
      const slot_point saved_phr_slot = sl_phr.value();
      const auto*      pusch_prb_it =
          std::find_if(pusch_nof_prbs_grid.begin(), pusch_nof_prbs_grid.end(), [saved_phr_slot](const auto& entry) {
            return entry.slot_rx == saved_phr_slot;
          });
      ASSERT_NE(pusch_prb_it, pusch_nof_prbs_grid.end());

      // Compute the PH value to report; this value guarantees that estimated path-loss at the GNB the same as
      // actual_path_loss_dB, with 1 dB tolerance due to the precision limits in the PHR handling.
      const int ph_value =
          p_cmax_dbm.start() - pusch_prb_it->f_cl_pw_control -
          static_cast<int>(std::round(pw_per_prb + convert_power_to_dB(static_cast<float>(pusch_prb_it->nof_prbs)) +
                                      alpha_fractional_pl * actual_path_loss_dB));
      ul_pw_ctrl_manager->handle_phr(make_phr_report(ph_value), sl_phr.value(), ue_rnti);

      // Reset the slot and value for PHR. This will be set again in the next opportunity
      sl_phr.reset();
      phr_to_report = false;
    }

    // > Check if the SINR has converged to the target SINR.
    const float actual_av_sinr_dB = average_reported_sinr_dB.average();

    // We consider the SINR has converged when the average SINR is within 0.5 dB of the target SINR. This is to account
    // the precision of the TPC commands.
    if (actual_av_sinr_dB < pl_dependent_pusch_sinr_target_dB + 0.5f and
        actual_av_sinr_dB > pl_dependent_pusch_sinr_target_dB - 0.5f) {
      convergence_reached = true;
      break;
    }
  }

  ASSERT_TRUE(convergence_reached);
}

INSTANTIATE_TEST_SUITE_P(test_closed_loop_pw_ctrl,
                         pusch_closed_loop_power_control_test_bench,
                         testing::Values(cl_pw_ctrl_params{5.0, ocudu::alpha::alpha1},
                                         cl_pw_ctrl_params{10.0, ocudu::alpha::alpha1},
                                         cl_pw_ctrl_params{15.0, ocudu::alpha::alpha1},
                                         cl_pw_ctrl_params{20.0, ocudu::alpha::alpha1},
                                         cl_pw_ctrl_params{25.0, ocudu::alpha::alpha1},
                                         cl_pw_ctrl_params{5.0, ocudu::alpha::alpha09},
                                         cl_pw_ctrl_params{10.0, ocudu::alpha::alpha09},
                                         cl_pw_ctrl_params{15.0, ocudu::alpha::alpha09},
                                         cl_pw_ctrl_params{20.0, ocudu::alpha::alpha09},
                                         cl_pw_ctrl_params{25.0, ocudu::alpha::alpha09},
                                         cl_pw_ctrl_params{5.0, ocudu::alpha::alpha08},
                                         cl_pw_ctrl_params{10.0, ocudu::alpha::alpha08},
                                         cl_pw_ctrl_params{15.0, ocudu::alpha::alpha08},
                                         cl_pw_ctrl_params{20.0, ocudu::alpha::alpha08},
                                         cl_pw_ctrl_params{25.0, ocudu::alpha::alpha08}));

class pusch_cl_pw_control_bounds_test_bench : public ::testing::Test,
                                              public pusch_closed_loop_power_control_base_test_bench
{
protected:
  pusch_cl_pw_control_bounds_test_bench() :
    pusch_closed_loop_power_control_base_test_bench(target_sinr, pusch_pw_ctrl_alpha)
  {
    sched_ue_creation_request_message ue_req = cfg_mng.get_default_ue_config_request();

    // Set alpha value for Fractional path-loss compensation.
    ue_req.cfg.cells.value()
        .front()
        .serv_cell_cfg.ul_config.value()
        .init_ul_bwp.pusch_cfg.value()
        .pusch_pwr_ctrl.value()
        .p0_alphasets.front()
        .p0_pusch_alpha = pusch_pw_ctrl_alpha;
    alpha_fractional_pl = alpha_to_float(pusch_pw_ctrl_alpha);

    // Set the PUSCH power control parameters.
    pw_per_prb = static_cast<int>(
        cell_cfg.params.ul_cfg_common.init_ul_bwp.pusch_cfg_common.value().p0_nominal_with_grant.value() +
        static_cast<int>(ue_req.cfg.cells.value()
                             .front()
                             .serv_cell_cfg.ul_config.value()
                             .init_ul_bwp.pusch_cfg.value()
                             .pusch_pwr_ctrl.value()
                             .p0_alphasets.front()
                             .p0.value()));

    actual_path_loss_dB = actual_path_loss_dB_fixed;

    // Suppose the intial SINR is the target SINR plus a random value between -15 and 15 dB.
    initial_pusch_sinr_dB = 0;

    // Generate random PUSCH to PHR delay.
    pusch_to_phr_delay =
        test_rng::uniform_int<unsigned>(PUSCH_TO_PHR_DELAY_BOUNDS.start(), PUSCH_TO_PHR_DELAY_BOUNDS.stop() - 1);

    add_ue(ue_req);
    sinr = initial_pusch_sinr_dB;
  }

  static constexpr interval<unsigned> PUSCH_TO_PHR_DELAY_BOUNDS = {2U, 10U};
  static constexpr unsigned           NOF_PUSCH_PRBS            = 5U;
  static constexpr float              target_sinr               = 30.0f;
  static constexpr unsigned           TDD_PATTERN_LENGTH        = 10U;
  static constexpr alpha              pusch_pw_ctrl_alpha       = alpha::alpha1;
  static constexpr float              actual_path_loss_dB_fixed = 90.0f;

  rnti_t ue_rnti = rnti_t::INVALID_RNTI;

  void SetUp() override
  {
    auto* test_ue = ues.find(to_du_ue_index(0));
    ue_rnti       = test_ue->crnti;
    ocudu_assert(test_ue != nullptr, "UE not found in the UE repository");
    ch_state_manager   = &(test_ue->get_pcell().channel_state_manager());
    ul_pw_ctrl_manager = &(test_ue->get_pcell().get_pusch_power_controller());
  }
};

TEST_F(pusch_cl_pw_control_bounds_test_bench, when_phr_is_non_positive_cl_stops_increasing_pw_target)
{
  // This test verifies that the power control adjustment doesn't result in estimated PHR (to be reported by the next
  // PUSCH) to be negative.

  // Flag to indicate there is a PHR to be reported.
  bool phr_to_report = false;
  // Slot at which the PHR is received.
  std::optional<slot_point> sl_phr = std::nullopt;
  // Keeps track of the SINR value that is reported to the UE.
  float f_cl_pw_control = 0;

  // This is the parameter that is used to determine the convergence of the test.
  exp_average_fast_start<float> average_f_cl_pw_control(0.5f);

  // Used to indicate if the test has been successful.
  bool convergence_reached = false;

  slot_point sl = slot_point(to_numerology_value(subcarrier_spacing::kHz30), 0U);
  for (unsigned sl_cnt = 0, sl_max = 10000; sl_cnt != sl_max; ++sl_cnt, ++sl) {
    // > Update the SINR value for the slot.
    // Compute the SINR with the path loss and the power control.
    slot_indication();

    // > If the PHR timer expires, generate the PUSCH-to-PHR delay.
    if (sl.count() % phr_periodic_timer_sl == 0) {
      phr_to_report = true;
      pusch_to_phr_delay =
          test_rng::uniform_int<unsigned>(PUSCH_TO_PHR_DELAY_BOUNDS.start(), PUSCH_TO_PHR_DELAY_BOUNDS.stop() - 1);
    }

    // > At UL slots, check if the PUSCH transmission is to be performed.
    const unsigned slot_idx_tdd_slot = sl.count() % TDD_PATTERN_LENGTH;
    const bool     is_tdd_slot       = slot_idx_tdd_slot > 7U;
    if (is_tdd_slot) {
      // Simulate a PUSCH transmission with probability 0.5 at every UL slot in the TDD pattern.
      if (test_rng::bernoulli(0.5)) {
        // We assume the instantaneous SINR that is reported to the UE is the one achieved with the power control.
        const float sinr_pw_control = sinr + f_cl_pw_control;
        ch_state_manager->update_pusch_snr(sinr_pw_control);
        ul_pw_ctrl_manager->compute_tpc_command(sl);
        // We assume the number of PRBs is constant and equal to 5.
        const auto updated_pusch_nof_prbs = ul_pw_ctrl_manager->adapt_pusch_prbs_to_phr(NOF_PUSCH_PRBS);

        // Save the number of PRBs allocated to the PUSCH, both in the channel state manager and in the test-bench.
        ul_pw_ctrl_manager->update_pusch_pw_ctrl_state(sl, updated_pusch_nof_prbs);
        // Save the Closed-loop power control value for the slot; this is used in the test to generate the PHR value.
        pusch_nof_prbs_grid[sl.count() % pusch_nof_prbs_grid.size()] =
            pusch_prbs_entry{sl, updated_pusch_nof_prbs, static_cast<int>(f_cl_pw_control)};

        // If there is a PH value to report, save the slot at which the PHR is received.
        if (phr_to_report and not sl_phr.has_value()) {
          sl_phr.emplace(sl);
        }
      }
    }

    // > Report PHR indication.
    if (sl_phr.has_value() and sl == sl_phr.value() + pusch_to_phr_delay and phr_to_report) {
      const slot_point saved_phr_slot = sl_phr.value();
      const auto*      pusch_prb_it =
          std::find_if(pusch_nof_prbs_grid.begin(), pusch_nof_prbs_grid.end(), [saved_phr_slot](const auto& entry) {
            return entry.slot_rx == saved_phr_slot;
          });
      ASSERT_NE(pusch_prb_it, pusch_nof_prbs_grid.end());

      // Compute the PH value to report; this value guarantees that estimated path-loss at the GNB the same as
      // actual_path_loss_dB, with 1 dB tolerance due to the precision limits in the PHR handling.
      const int ph_value =
          p_cmax_dbm.start() - pusch_prb_it->f_cl_pw_control -
          static_cast<int>(std::round(pw_per_prb + convert_power_to_dB(static_cast<float>(pusch_prb_it->nof_prbs)) +
                                      alpha_fractional_pl * actual_path_loss_dB));
      ul_pw_ctrl_manager->handle_phr(make_phr_report(ph_value), sl_phr.value(), ue_rnti);
      ASSERT_GE(ph_value, 0);

      // Reset the slot and value for PHR. This will be set again in the next opportunity
      sl_phr.reset();
      phr_to_report = false;
    }

    // We consider the SINR has converged when the average SINR is within 0.5 dB of the target SINR. This is to account
    // the precision of the TPC commands.
    if (f_cl_pw_control == average_f_cl_pw_control.average()) {
      convergence_reached = true;
      break;
    }
  }

  ASSERT_TRUE(convergence_reached);
}
