// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ue_channel_state_manager.h"

using namespace ocudu;

ue_channel_state_manager::ue_channel_state_manager(const scheduler_ue_expert_config& expert_cfg,
                                                   unsigned                          nof_dl_ports_) :
  nof_dl_ports(nof_dl_ports_),
  pusch_snr_db(expert_cfg.initial_ul_sinr),
  average_pusch_sinr_dB(expert_cfg.ul_power_ctrl.ema_alpha_cl_pw_control_sinr),
  wideband_cqi(expert_cfg.initial_cqi),
  // Only relevant if the srs_prohibit_time has a value.
  srs_prohibit_window(expert_cfg.srs_prohibit_time.value_or(srs_periodicity::sl40))
{
  // Set initial precoding value when no CSI has yet been received.
  if (nof_dl_ports == 2) {
    recommended_prg_info.resize(2, pdsch_precoding_info::prg_info{pmi_two_antenna_port{.pmi = 0}});
  } else if (nof_dl_ports == 4) {
    recommended_prg_info.resize(
        4,
        pdsch_precoding_info::prg_info{pmi_typeI_single_panel{.panel_config = pmi_codebook_single_panel_config::two_one,
                                                              .i_1_1        = 0,
                                                              .i_1_2        = std::nullopt,
                                                              .i_1_3        = std::nullopt,
                                                              .i_2          = 0}});
  }
}

bool ue_channel_state_manager::handle_csi_report(const csi_report_data& csi_report)
{
  latest_csi_report = csi_report;

  // Set wideband CQI.
  if (csi_report.first_tb_wideband_cqi.has_value()) {
    wideband_cqi = csi_report.first_tb_wideband_cqi.value();
  }

  // Update recommended number of layers based on RI.
  if (csi_report.ri.has_value()) {
    if (csi_report.ri.value() > nof_dl_ports) {
      return false;
    }
    recommended_dl_layers = csi_report.ri.value().value();
  }

  if (csi_report.pmi.has_value()) {
    if (nof_dl_ports <= 1) {
      return false;
    }
    const unsigned table_idx        = nof_layers_to_index(recommended_dl_layers);
    recommended_prg_info[table_idx] = csi_report.pmi.value();
  }

  return true;
}

void ue_channel_state_manager::update_pusch_snr(float snr_db)
{
  pusch_snr_db = snr_db;
  average_pusch_sinr_dB.push(snr_db);
}

void ue_channel_state_manager::update_srs_channel_matrix(const srs_channel_matrix& channel_matrix,
                                                         tx_scheme_codebook        codebook_cfg)
{
  // [Implementation-defined] Assume noise variance is 30dB below the average received power.
  const float norm      = channel_matrix.frobenius_norm();
  const float noise_var = norm * norm / (1000 * channel_matrix.get_nof_tx_ports());

  // Calculate TPMI information.
  last_pusch_tpmi_select_info =
      get_tpmi_select_info(channel_matrix, noise_var, codebook_cfg.max_rank.value(), codebook_cfg.codebook_subset);
}

unsigned ue_channel_state_manager::get_recommended_pusch_tpmi(unsigned nof_layers) const
{
  if (last_pusch_tpmi_select_info.has_value() &&
      (nof_layers <= last_pusch_tpmi_select_info.value().get_max_nof_layers())) {
    return last_pusch_tpmi_select_info.value().get_tpmi_select(nof_layers).tpmi;
  }
  return 0;
}

unsigned ue_channel_state_manager::get_nof_ul_layers() const
{
  // SINR maximum differential between the highest and lowest SINR in decibels. The number of layers is ignored if the
  // differential is greater than the threshold.
  static constexpr float sinr_dB_max_diff = 6.0F;
  // SINR threshold in decibels: to select a number of layers, the SINR of all layers must be greater than the
  // threshold.
  static constexpr float sinr_dB_threshold = 18.0F;
  // SINR comparison penalty in decibels. A lower number of layers is selected only if its SINR is larger than the SINR
  // of a higher number of layers plus the penalty.
  static constexpr float sinr_dB_penalty = 2.0F;

  // Skip if it does not have a TPMI.
  if (!last_pusch_tpmi_select_info.has_value()) {
    return 1;
  }

  unsigned max_nof_layers = last_pusch_tpmi_select_info->get_max_nof_layers();

  // Find maximum number of layers.
  float    best_sinr       = -std::numeric_limits<float>::infinity();
  unsigned best_nof_layers = 1;
  for (unsigned nof_layers = max_nof_layers; nof_layers != 0; --nof_layers) {
    // Obtain TPMI information for the number of layers.
    const pusch_tpmi_select_info::tpmi_info& info = last_pusch_tpmi_select_info->get_tpmi_select(nof_layers);

    // Get the minimum SINR.
    const float min_sinr_dB = *std::min_element(info.sinr_dB_layer.begin(), info.sinr_dB_layer.end());

    // Get the maximum SINR.
    const float max_sinr_dB = *std::max_element(info.sinr_dB_layer.begin(), info.sinr_dB_layer.end());

    // If the minimum SINR is above the threshold, use this number of layers.
    if (min_sinr_dB > sinr_dB_threshold) {
      return nof_layers;
    }

    // Discard number of layers if the differential between the maximum and minimum exceeds the limit.
    if (max_sinr_dB - min_sinr_dB > sinr_dB_max_diff) {
      continue;
    }

    // If the minimum SINR is better than the previous best above the hysteresis, select this number of layers.
    if (info.avg_sinr_dB > best_sinr) {
      best_sinr       = info.avg_sinr_dB + sinr_dB_penalty;
      best_nof_layers = nof_layers;
    }
  }

  return best_nof_layers;
}
