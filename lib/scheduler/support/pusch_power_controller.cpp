// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "pusch_power_controller.h"
#include "../config/ue_configuration.h"
#include "ocudu/ocudulog/ocudulog.h"
#include "ocudu/ran/power_control/tpc_mapping.h"

using namespace ocudu;

pusch_power_controller::pusch_power_controller(const ue_cell_configuration&    ue_cell_cfg,
                                               const ue_channel_state_manager& ch_state_manager,
                                               ocudulog::basic_logger&         logger_) :
  rnti(ue_cell_cfg.crnti),
  cl_pw_control_enabled(ue_cell_cfg.cell_cfg_common.expert_cfg.ue.ul_power_ctrl.enable_pusch_cl_pw_control),
  phr_bw_adaptation_enabled(ue_cell_cfg.cell_cfg_common.expert_cfg.ue.ul_power_ctrl.enable_phr_bw_adaptation),
  p0_nominal_pusch(ue_cell_cfg.cell_cfg_common.params.ul_cfg_common.init_ul_bwp.pusch_cfg_common.value()
                       .p0_nominal_with_grant.value()),
  channel_state_manager(ch_state_manager),
  pusch_sinr_target_dB(ue_cell_cfg.cell_cfg_common.expert_cfg.ue.ul_power_ctrl.target_pusch_sinr),
  ref_path_loss_for_target_sinr(
      ue_cell_cfg.cell_cfg_common.expert_cfg.ue.ul_power_ctrl.path_loss_for_target_pusch_sinr),
  tpc_adjust_prohibit_time_sl([&ue_cell_cfg]() -> unsigned {
    return tpc_adjust_prohibit_time_ms << to_numerology_value(
               ue_cell_cfg.cell_cfg_common.params.ul_cfg_common.init_ul_bwp.generic_params.scs);
  }()),
  logger(logger_)
{
  (void)logger;

  // Save the PUSCH power control configuration.
  reconfigure(ue_cell_cfg);

  // Initialize PUSCH PRB grid.
  static constexpr unsigned init_nof_prbs     = 0U;
  static constexpr int      init_f_pw_control = 0;
  std::fill(pusch_pw_ctrl_grid.begin(),
            pusch_pw_ctrl_grid.end(),
            pusch_pw_ctrl_data{slot_point{}, init_nof_prbs, init_f_pw_control});

  // Dummy casts only needed to prevent Clang from complaining about unused variables.
  static_cast<void>(rnti);
  static_cast<void>(cl_pw_control_enabled);
  static_cast<void>(phr_bw_adaptation_enabled);
  static_cast<void>(channel_state_manager);
  static_cast<void>(ref_path_loss_for_target_sinr);
  static_cast<void>(pusch_sinr_target_dB);
}

void pusch_power_controller::reconfigure(const ue_cell_configuration& ue_cell_cfg)
{
  if (ue_cell_cfg.init_bwp().ul.ded() != nullptr and ue_cell_cfg.init_bwp().ul.ded()->pusch_cfg.has_value() and
      ue_cell_cfg.init_bwp().ul.ded()->pusch_cfg.value().pusch_pwr_ctrl.has_value()) {
    pusch_pwr_ctrl.emplace(ue_cell_cfg.init_bwp().ul.ded()->pusch_cfg.value().pusch_pwr_ctrl.value());
  }
}

void pusch_power_controller::update_pusch_pw_ctrl_state(slot_point slot_rx, unsigned nof_prbs)
{
  const int latest_f_cl_pw_control =
      latest_pusch_pw_control.has_value() ? latest_pusch_pw_control.value().f_cl_pw_control : 0;
  const unsigned grid_idx      = slot_rx.count();
  pusch_pw_ctrl_grid[grid_idx] = {slot_rx, nof_prbs, latest_f_cl_pw_control};
}

void pusch_power_controller::handle_phr(const cell_ph_report& phr, slot_point slot_rx, rnti_t phr_rnti)
{
  if (not pusch_pwr_ctrl.has_value()) {
    return;
  }

  // PHR reported in Configured Grant PUSCH have a different RNTI from the C-RNTI used by the UE.
  // We ignore these PHRs and only log the event when the reported PH is negative.
  if (phr_rnti != rnti) {
    if (phr.ph.stop() < 0) {
      logger.info("cs-rnti={}: negative PHR reported by Configured Grant PUSCH at slot={}", phr_rnti, slot_rx);
    }
    return;
  }

  // Find the entry in the grid that corresponds to the slot where the PHR was received.
  const auto& pusch_pw_ctrl = pusch_pw_ctrl_grid[slot_rx.count()];
  if (pusch_pw_ctrl.slot_rx != slot_rx) {
    const bool first_phr_reporting =
        std::all_of(pusch_pw_ctrl_grid.begin(), pusch_pw_ctrl_grid.end(), [](const pusch_pw_ctrl_data& data) {
          return not data.slot_rx.valid();
        });
    if (not first_phr_reporting) {
      logger.info("rnti={}: No PUSCH allocation corresponding to the PHR received at slot={} grid_idx={}. Consider "
                  "increasing the grid size",
                  rnti,
                  slot_rx,
                  slot_rx.count() % pusch_pw_ctrl_grid.size());
    }
    latest_phr.emplace(ue_phr_report{phr, std::nullopt});
    return;
  }

  ocudu_assert(phr.p_cmax.has_value(), "P_cmax is mandatory in PHR with single PHR entry");
  // \c latest_path_loss_db is only used to compute the TPC command, hence skip this if the closed-loop power control is
  // disabled.
  float latest_path_loss_db = 0;
  if (cl_pw_control_enabled) {
    const auto p0_ue_pusch = static_cast<int>(pusch_pwr_ctrl.value().p0_alphasets.front().p0.value());

    const float alpha_fractional_pl = alpha_to_float(pusch_pwr_ctrl.value().p0_alphasets.front().p0_pusch_alpha);

    const int estimated_pl_dB =
        static_cast<int>(std::round((static_cast<float>(phr.p_cmax.value().start() - phr.ph.stop() -
                                                        (p0_nominal_pusch + p0_ue_pusch) - pusch_pw_ctrl.f_pw_control) -
                                     convert_power_to_dB(static_cast<float>(pusch_pw_ctrl.nof_prbs))) /
                                    alpha_fractional_pl));

    latest_path_loss_db = estimated_pl_dB > 0.0f ? static_cast<float>(estimated_pl_dB) : 0.0f;
  }

  // Save the PHR value multiplied by the number of PRBs; this will be used to compute the next PUSCH's number of PRBs.
  const float n_rbs_db = convert_power_to_dB(static_cast<float>(pusch_pw_ctrl.nof_prbs));
  // Take the start of the PHR interval, as we need to consider the worst case (i.e., min value) for PHR.
  latest_phr.emplace(ue_phr_report{phr,
                                   ue_phr_report::phr_derived_data{static_cast<float>(phr.ph.start()) + n_rbs_db,
                                                                   pusch_pw_ctrl.f_pw_control,
                                                                   latest_path_loss_db}});
}

unsigned pusch_power_controller::adapt_pusch_prbs_to_phr(unsigned nof_prbs) const
{
  // The goal of this function is to prevent that the UE transmit a PUSCH grant resulting in a nominal power that is too
  // low. Based in the PHR and on the number of PRBs that were used for the PUSCH transmission that carried the PHR, we
  // can estimate the current PHR value. If this is negative, then we need to adjust the number of PRBs to be used for
  // the PUSCH, so the UE won't have to reduce the nominal TX power for the PUSCH to meet the max power constraint.

  if (not phr_bw_adaptation_enabled or not latest_phr.has_value() or
      (not latest_phr.value().phr_data_for_pw_ctrl.has_value()) or (not latest_pusch_pw_control.has_value())) {
    return nof_prbs;
  }

  // To prevent that the next PHR is negative (assuming p_cmax and the path-loss do not change over time), the
  // following inequality needs to be met.
  // 10log10(n'_PRBs) <=  PHR + 10log10(n_PRBs) -  Delta_f_CL,
  // where n'_PRBs is the number of PRBs to be used for the next PUSCH transmission, PHR is the latest report PH value,
  // Delta_f_CL is the latest power control adjustment value used for the PUSCH transmission that carried the PHR, and
  // n_PRBs is the number of PRBs used for the PUSCH transmission that carried the PHR.
  const auto delta_f_pw_control = static_cast<float>(latest_phr.value().phr_data_for_pw_ctrl.value().f_cl_pw_control -
                                                     latest_pusch_pw_control.value().f_cl_pw_control);
  static constexpr float min_pusch_grant_prbs = 1.0f;
  const float            estimated_nof_prbs   = std::max(
      convert_dB_to_power(latest_phr.value().phr_data_for_pw_ctrl.value().ph_normalized_pw_dB - delta_f_pw_control),
      min_pusch_grant_prbs);

  // We only need to adjust the number of PRBs if the estimated number of PRBs is less than the current number of PRBs.
  return std::min(nof_prbs, static_cast<unsigned>(std::floor(estimated_nof_prbs)));
}

uint8_t pusch_power_controller::compute_tpc_command(slot_point pusch_slot)
{
  static constexpr uint8_t default_tpc = 1;

  if (not cl_pw_control_enabled or not pusch_pwr_ctrl.has_value() or not latest_pusch_pw_control.has_value()) {
    static constexpr int default_f_cl_pw_control = 0;
    latest_pusch_pw_control.emplace(pusch_pw_control{default_f_cl_pw_control, pusch_slot});
    return default_tpc;
  }
  if (pusch_slot <= latest_pusch_pw_control.value().latest_tpc_slot + tpc_adjust_prohibit_time_sl) {
    return default_tpc;
  }

  if (not latest_phr.has_value() or (not latest_phr.value().phr_data_for_pw_ctrl.has_value())) {
    return default_tpc;
  }
  const float alpha_fractional_pl = alpha_to_float(pusch_pwr_ctrl.value().p0_alphasets.front().p0_pusch_alpha);

  // This takes into account the path loss compensation for PUSCH fractional power control.
  const float fract_pl_compensation =
      (1.0f - alpha_fractional_pl) *
      (ref_path_loss_for_target_sinr - latest_phr.value().phr_data_for_pw_ctrl.value().latest_path_loss_db);

  float sinr_to_target_diff =
      pusch_sinr_target_dB + fract_pl_compensation - channel_state_manager.get_pusch_average_sinr();

  // To prevent that the next PHR is negative (assuming p_cmax and the path-loss do not change over time), the
  // following inequality needs to be met.
  // Delta_f_CL <=  PHR + 10log10(n_PRBs) -  10log10(n_PRBs_min),
  // where Delta_f_CL is the difference between the next and latest power control adjustment value used for the PUSCH
  // transmission that carried the PHR, n_PRBs is the number of PRBs used for the PUSCH transmission that carried the
  // PHR, and n_PRBs_min is the minimum number of PRBs that can be used for a PUSCH transmission.
  static constexpr float pusch_min_prb_alloc = 1.0f;
  const float            max_delta_f_cl_pw_control =
      latest_phr.value().phr_data_for_pw_ctrl.value().ph_normalized_pw_dB - convert_dB_to_power(pusch_min_prb_alloc);

  uint8_t tpc_command = 1U;
  // [Implementation-defined] In the following, we compute the TPC command based on the SINR difference, to approximate
  // the mapping in Table 7.1.1-1, TS 38.213. The mapping we use is the following:
  // SINR diff. (dB) > 2.5dB -> delta_PUSCH = 3; TCP = 3.
  // 0.5dB < SINR diff. (dB) <= 2.5dB -> delta_PUSCH = 1; TCP = 2.
  // -0.5dB < SINR diff. (dB) <= 0.5dB -> delta_PUSCH = 0; TCP = 1.
  // SINR diff. (dB) < -0.5dB -> delta_PUSCH = -1; TCP = 0.
  // NOTE: we need also to ensure that the power control adjustment corresponding to the TPC commands is not greater
  // than \c max_delta_f_cl_pw_control.
  if (sinr_to_target_diff > 2.5f and max_delta_f_cl_pw_control > 3.0f) {
    tpc_command = 3U;
  } else if (sinr_to_target_diff > 0.5f and max_delta_f_cl_pw_control > 1.0f) {
    tpc_command = 2U;
  } else if (sinr_to_target_diff > -0.5f and max_delta_f_cl_pw_control >= 0.0f) {
    tpc_command = 1U;
  } else {
    tpc_command = 0U;
  }

  // Cap latest_pusch_pw_control to its minimum value.
  if (tpc_command == 0 and latest_pusch_pw_control.value().f_cl_pw_control <= min_f_cl_pw_control) {
    tpc_command = 0;
  }

  latest_pusch_pw_control.value().f_cl_pw_control += tpc_mapping(tpc_command);
  latest_pusch_pw_control.value().latest_tpc_slot = pusch_slot;
  return tpc_command;
}
