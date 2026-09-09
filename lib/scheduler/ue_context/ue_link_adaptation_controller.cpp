// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ue_link_adaptation_controller.h"
#include "../config/time_domain_mapper.h"
#include "../support/mcs_calculator.h"

using namespace ocudu;

ue_link_adaptation_controller::ue_link_adaptation_controller(const cell_configuration&       cell_cfg_,
                                                             const ue_channel_state_manager& ue_channel_state) :
  cell_cfg(cell_cfg_), ue_ch_st(ue_channel_state)
{
  if (cell_cfg.expert_cfg.ue.olla_cqi_inc > 0) {
    dl_olla.emplace(cell_cfg.expert_cfg.ue.olla_dl_target_bler,
                    cell_cfg.expert_cfg.ue.olla_cqi_inc,
                    cell_cfg.expert_cfg.ue.olla_max_cqi_offset);
  }
  last_dl_mcs_table = pdsch_mcs_table::qam64LowSe; // Set a different value to force update.
  update_dl_mcs_lims(pdsch_mcs_table::qam64);

  if (cell_cfg.expert_cfg.ue.olla_ul_snr_inc > 0) {
    ul_olla.emplace(cell_cfg.expert_cfg.ue.olla_ul_target_bler,
                    cell_cfg.expert_cfg.ue.olla_ul_snr_inc,
                    cell_cfg.expert_cfg.ue.olla_max_ul_snr_offset);
  }
  // Set a different value to force update.
  last_ul_mcs_table = pusch_mcs_table::qam64LowSe;
  update_ul_mcs_lims(pusch_mcs_table::qam64, cell_cfg.use_msg3_transform_precoder());
}

void ue_link_adaptation_controller::handle_dl_ack_info(bool                         ack_value,
                                                       sch_mcs_index                used_mcs,
                                                       pdsch_mcs_table              mcs_table,
                                                       std::optional<sch_mcs_index> olla_mcs)
{
  if (not dl_olla.has_value()) {
    // DL OLLA is disabled.
    return;
  }

  // Only run OLLA if the chosen MCS actually matches the MCS suggested by the OLLA.
  if (not olla_mcs.has_value() or olla_mcs.value() != used_mcs) {
    return;
  }

  // Update the MCS boundaries based on the chosen MCS table.
  update_dl_mcs_lims(mcs_table);

  // Finally, run OLLA algorithm.
  dl_olla->update(ack_value, used_mcs, dl_mcs_lims);
}

void ue_link_adaptation_controller::handle_ul_crc_info(bool                         crc,
                                                       sch_mcs_index                used_mcs,
                                                       pusch_mcs_table              mcs_table,
                                                       std::optional<sch_mcs_index> olla_mcs,
                                                       std::optional<float>         pusch_snr_db)
{
  if (not ul_olla.has_value()) {
    // UL OLLA is disabled.
    return;
  }

  // A NACK reported at very low SINR (e.g. deep in a coverage hole) does not reflect a bias in the link adaptation
  // and would spuriously drag the OLLA offset down, so such CRCs are excluded from the update.
  if (pusch_snr_db.has_value() and pusch_snr_db.value() < cell_cfg.expert_cfg.ue.olla_ul_min_pusch_snr) {
    return;
  }

  // Only run OLLA if the chosen MCS actually matches the MCS suggested by the OLLA.
  if (not olla_mcs.has_value() or olla_mcs.value() != used_mcs) {
    return;
  }

  // Update the MCS boundaries based on the chosen MCS table.
  update_ul_mcs_lims(mcs_table, cell_cfg.use_msg3_transform_precoder());

  // Finally, run OLLA algorithm.
  ul_olla->update(crc, used_mcs, ul_mcs_lims);
}

float ue_link_adaptation_controller::get_effective_cqi() const
{
  float eff_cqi = static_cast<float>(ue_ch_st.get_wideband_cqi().value());
  if (eff_cqi == 0.0F) {
    // Reported CQI==0 is a special case, where the channel is not considered in a valid state.
    return -1;
  }

  if (dl_olla.has_value()) {
    // For the case of DL outer loop link adaptation enabled.
    eff_cqi += dl_olla->offset_db();
    // Ensure CQI remains within [1, 15].
    eff_cqi = std::min(std::max(1.0F, eff_cqi), static_cast<float>(cqi_value::max()));
  }
  return eff_cqi;
}

float ue_link_adaptation_controller::get_effective_snr() const
{
  return ue_ch_st.get_pusch_snr() + (ul_olla.has_value() ? ul_olla.value().offset_db() : 0.0f);
}

std::optional<sch_mcs_index> ue_link_adaptation_controller::calculate_dl_mcs(pdsch_mcs_table mcs_table) const
{
  if (cell_cfg.expert_cfg.ue.dl_mcs.length() == 0) {
    // Fixed MCS.
    return cell_cfg.expert_cfg.ue.dl_mcs.start();
  }

  // Derive MCS using the combination of CQI + outer loop link adaptation.
  const float eff_cqi = get_effective_cqi();
  if (eff_cqi <= 0.0F) {
    // Special case, where reported CQI==0.
    return std::nullopt;
  }

  // There are fewer CQIs than MCS values, so we perform a linear interpolation.
  const float   cqi_lb = std::floor(eff_cqi), cqi_ub = std::ceil(eff_cqi);
  const float   coeff  = eff_cqi - cqi_lb;
  const float   mcs_lb = static_cast<float>(map_cqi_to_mcs(static_cast<unsigned>(cqi_lb), mcs_table).value().value());
  const float   mcs_ub = static_cast<float>(map_cqi_to_mcs(static_cast<unsigned>(cqi_ub), mcs_table).value().value());
  sch_mcs_index mcs{static_cast<uint8_t>(std::floor(mcs_lb * (1 - coeff) + mcs_ub * coeff))};

  // Ensures that the MCS is within the configured range.
  mcs = std::min(std::max(mcs, cell_cfg.expert_cfg.ue.dl_mcs.start()), cell_cfg.expert_cfg.ue.dl_mcs.stop());
  return mcs;
}

sch_mcs_index ue_link_adaptation_controller::calculate_ul_mcs(pusch_mcs_table mcs_table,
                                                              bool            use_transform_precoder) const
{
  if (cell_cfg.expert_cfg.ue.ul_mcs.length() == 0) {
    // Fixed MCS.
    return cell_cfg.expert_cfg.ue.ul_mcs.start();
  }

  // Derive MCS using the combination of estimated UL SNR + outer loop link adaptation.
  sch_mcs_index mcs = map_snr_to_mcs_ul(get_effective_snr(), mcs_table, use_transform_precoder);
  mcs               = std::min(std::max(mcs, ul_mcs_lims.start()), ul_mcs_lims.stop());

  return mcs;
}

pucch_repetition_factor ue_link_adaptation_controller::get_recommended_pucch_rep_factor() const
{
  if (cell_cfg.params.init_bwp.pucch.resources.harq_ack_rep.has_value()) {
    const auto& rep_params = cell_cfg.params.init_bwp.pucch.resources.harq_ack_rep.value();
    const float eff_snr    = get_effective_snr();

    for (unsigned i = rep_params.sinr_thresholds.size(); i > 0; --i) {
      // Check if the effective SNR is below the threshold for the current repetition factor.
      if (eff_snr < rep_params.sinr_thresholds[i - 1]) {
        // 2 -> n8, 1 -> n4, 0 -> n2
        return static_cast<pucch_repetition_factor>(2U << (i - 1));
      }
    }
  }
  return pucch_repetition_factor::n1;
}

std::optional<uint8_t>
ue_link_adaptation_controller::select_pdsch_repetition_count(const search_space_info& ss_info) const
{
  if (cell_cfg.expert_cfg.ue.pdsch_cqi_rep_threshold == 0.0F) {
    return std::nullopt;
  }

  // Repetitions are feature-gated and only apply to a SearchSpace whose dedicated PDSCH TDRA list carries a
  // repetition row (i.e., a Rel-16 list).
  const std::optional<uint8_t> max_reps =
      ss_info.bwp->dl.td_mapper().max_pdsch_repetitions(ss_info.get_dl_dci_format());
  if (not max_reps.has_value()) {
    return std::nullopt;
  }

  // Repetitions are triggered when the effective CQI (reported CQI corrected by OLLA) is below the threshold.
  if (get_effective_cqi() >= cell_cfg.expert_cfg.ue.pdsch_cqi_rep_threshold) {
    return std::nullopt;
  }

  // Single repetition level for now: request the maximum configured. Future link adaptation may pick a lower level
  // based on the effective CQI.
  return max_reps;
}

std::optional<uint8_t>
ue_link_adaptation_controller::select_pusch_repetition_count(const search_space_info& ss_info) const
{
  if (not cell_cfg.expert_cfg.ue.pusch_force_rep and not cell_cfg.expert_cfg.ue.pusch_sinr_rep_threshold.has_value()) {
    return std::nullopt;
  }

  // Repetitions only apply to a SearchSpace whose dedicated PUSCH TDRA list carries a repetition row (Rel-16 list).
  const std::optional<uint8_t> max_reps =
      ss_info.bwp->ul.td_mapper().max_pusch_repetitions(ss_info.get_ul_dci_format());
  if (not max_reps.has_value()) {
    return std::nullopt;
  }

  if (cell_cfg.expert_cfg.ue.pusch_force_rep) {
    return max_reps;
  }

  // Repetitions are triggered when the effective SNR (estimated UL SINR corrected by OLLA) is below the threshold.
  if (get_effective_snr() >= cell_cfg.expert_cfg.ue.pusch_sinr_rep_threshold.value()) {
    return std::nullopt;
  }

  // Single repetition level for now: request the maximum configured.
  return max_reps;
}

void ue_link_adaptation_controller::update_dl_mcs_lims(pdsch_mcs_table mcs_table)
{
  if (last_dl_mcs_table == mcs_table) {
    return;
  }

  last_dl_mcs_table           = mcs_table;
  const sch_mcs_index max_mcs = map_cqi_to_mcs(cqi_value::max(), mcs_table).value();
  dl_mcs_lims                 = interval<sch_mcs_index, true>{cell_cfg.expert_cfg.ue.dl_mcs.start(),
                                                              std::min(cell_cfg.expert_cfg.ue.dl_mcs.stop(), max_mcs)};
}

void ue_link_adaptation_controller::update_ul_mcs_lims(pusch_mcs_table mcs_table, bool transform_precoder)
{
  if (last_ul_mcs_table == mcs_table and last_transform_precoder == transform_precoder) {
    return;
  }

  last_ul_mcs_table       = mcs_table;
  last_transform_precoder = transform_precoder;

  ul_mcs_lims = interval<sch_mcs_index, true>{
      cell_cfg.expert_cfg.ue.ul_mcs.start(),
      std::min(cell_cfg.expert_cfg.ue.ul_mcs.stop(), get_max_mcs_ul(mcs_table, transform_precoder))};
}
