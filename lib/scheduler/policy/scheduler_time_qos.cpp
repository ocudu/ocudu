// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "scheduler_time_qos.h"
#include "../slicing/slice_ue_repository.h"
#include "../support/dmrs_helpers.h"
#include "../ue_scheduling/grant_params_selector.h"
#include <algorithm>

using namespace ocudu;

// [Implementation-defined] Limit for the coefficient of the proportional fair metric to avoid issues with double
// imprecision.
static constexpr unsigned MAX_PF_COEFF = 10;

ue_history_repository::ue_history_repository()
{
  ue_db.reserve(MAX_NOF_DU_UES);
  ue_row_ids.reserve(MAX_NOF_DU_UES);
  last_samples_buffer.resize(MAX_NOF_DU_UES, 0);
}

void ue_history_repository::add_ue(du_ue_index_t ue_index)
{
  ocudu_assert(ue_row_ids.size() <= ue_index or ue_row_ids[ue_index] == invalid_row_id,
               "UE was already added to the history repository");
  if (ue_row_ids.size() <= ue_index) {
    ue_row_ids.resize(ue_index + 1, soa::row_id{std::numeric_limits<uint32_t>::max()});
  }
  auto row_id          = ue_db.insert(0.0, 0.0);
  ue_row_ids[ue_index] = row_id;
}

void ue_history_repository::rem_ue(du_ue_index_t ue_index)
{
  ocudu_assert(ue_row_ids.size() > ue_index and ue_row_ids[ue_index] != invalid_row_id,
               "UE was not added to the history repository");
  auto row_id = ue_row_ids[ue_index];
  ue_db.erase(row_id);
  ue_row_ids[ue_index] = soa::row_id{std::numeric_limits<uint32_t>::max()};
}

void ue_history_repository::save_dl_newtx_grants(span<const dl_msg_alloc> dl_grants)
{
  if (dl_grants.empty()) {
    // Ignore slots with no grants in the statistics, as they don't add any differentiation between UEs.
    return;
  }

  // Compute DL average rates for all UEs.
  span<float> dl_rate = ue_db.column<component::dl_avg>().to_span();
  std::fill(last_samples_buffer.begin(), last_samples_buffer.begin() + dl_rate.size(), 0.0f);
  for (const dl_msg_alloc& grant : dl_grants) {
    unsigned offset = ue_db.get_offset(ue_row_ids[grant.context.ue_index]);
    for (const auto& cw : grant.pdsch_cfg.codewords) {
      last_samples_buffer[offset] += cw.tb_size_bytes.value();
    }
  }
  auto sample_it = last_samples_buffer.begin();
  for (auto rate_it = dl_rate.begin(), end_it = dl_rate.end(); rate_it != end_it; ++rate_it, ++sample_it) {
    *rate_it = (1.0 - exp_avg_alpha) * (*rate_it) + exp_avg_alpha * (*sample_it);
  }
}

void ue_history_repository::save_ul_newtx_grants(span<const ul_sched_info> ul_grants)
{
  if (ul_grants.empty()) {
    // Ignore slots with no grants in the statistics, as they don't add any differentiation between UEs.
    return;
  }

  // Compute UL average rates for all UEs.
  span<float> ul_rate = ue_db.column<component::ul_avg>().to_span();
  std::fill(last_samples_buffer.begin(), last_samples_buffer.begin() + ul_rate.size(), 0.0f);
  for (const ul_sched_info& grant : ul_grants) {
    unsigned offset = ue_db.get_offset(ue_row_ids[grant.context.ue_index]);
    last_samples_buffer[offset] += grant.pusch_cfg.tb_size_bytes.value();
  }
  auto sample_it = last_samples_buffer.begin();
  for (auto rate_it = ul_rate.begin(), end_it = ul_rate.end(); rate_it != end_it; ++rate_it, ++sample_it) {
    *rate_it = (1.0 - exp_avg_alpha) * (*rate_it) + exp_avg_alpha * (*sample_it);
  }
}

rate_estimator::rate_estimator(const cell_configuration& cell_cfg) :
  dl_tbs_cfg_ref{.nof_symb_sh      = NOF_OFDM_SYM_PER_SLOT_NORMAL_CP,
                 .nof_oh_prb       = 0,
                 .tb_scaling_field = 1,
                 .n_prb            = cell_cfg.params.dl_cfg_common.init_dl_bwp.generic_params.crbs.length()},
  ul_tbs_cfg_ref{.nof_symb_sh      = NOF_OFDM_SYM_PER_SLOT_NORMAL_CP,
                 .nof_oh_prb       = 0,
                 .tb_scaling_field = 1,
                 .n_prb            = cell_cfg.params.ul_cfg_common.init_ul_bwp.generic_params.crbs.length()}
{
  dl_dmrs_rbs_per_nof_layers.resize(cell_cfg.params.dl_carrier.nof_ant);
  for (unsigned nof_layers = 1, max_layers = dl_dmrs_rbs_per_nof_layers.size(); nof_layers <= max_layers;
       ++nof_layers) {
    const pdsch_time_domain_resource_allocation pdsch_td_cfg{
        0, sch_mapping_type::typeA, {0, NOF_OFDM_SYM_PER_SLOT_NORMAL_CP}};
    const dmrs_downlink_config dmrs_cfg{};
    auto                       dmrs = make_dmrs_info_dedicated(
        pdsch_td_cfg, cell_cfg.params.pci, cell_cfg.params.dmrs_typeA_pos, dmrs_cfg, nof_layers, max_layers, false);
    dl_dmrs_rbs_per_nof_layers[nof_layers - 1] = calculate_nof_dmrs_per_rb(dmrs);
  }

  ul_dmrs_rbs_per_nof_layers.resize(cell_cfg.params.ul_carrier.nof_ant);
  for (unsigned nof_layers = 1, max_layers = ul_dmrs_rbs_per_nof_layers.size(); nof_layers <= max_layers;
       ++nof_layers) {
    const pusch_time_domain_resource_allocation pusch_td_cfg{
        2, sch_mapping_type::typeA, {0, NOF_OFDM_SYM_PER_SLOT_NORMAL_CP}};
    const dmrs_uplink_config dmrs_cfg{};
    auto                     dmrs = make_dmrs_info_dedicated(
        pusch_td_cfg, cell_cfg.params.pci, cell_cfg.params.dmrs_typeA_pos, dmrs_cfg, nof_layers, max_layers, false);
    ul_dmrs_rbs_per_nof_layers[nof_layers - 1] = calculate_nof_dmrs_per_rb(dmrs);
  }
}

unsigned rate_estimator::estimate_max_dl_tbs(const ue_cell& ue_cc) const
{
  static constexpr pdsch_mcs_table ref_mcs_table = pdsch_mcs_table::qam256;

  auto mcs = ue_cc.link_adaptation_controller().calculate_dl_mcs(ref_mcs_table);
  if (not mcs.has_value()) {
    // CQI is either 0 or above 15, which means no DL.
    return 0;
  }

  const unsigned            nof_layers = ue_cc.channel_state_manager().get_nof_dl_layers();
  const sch_mcs_description mcs_info   = pdsch_mcs_get_config(ref_mcs_table, mcs.value());
  const units::bytes        tbs_bytes =
      tbs_calculator_calculate(tbs_calculator_configuration{.nof_symb_sh  = dl_tbs_cfg_ref.nof_symb_sh,
                                                            .nof_dmrs_prb = dl_dmrs_rbs_per_nof_layers[nof_layers - 1],
                                                            .nof_oh_prb   = dl_tbs_cfg_ref.nof_oh_prb,
                                                            .mcs_descr    = mcs_info,
                                                            .nof_layers   = nof_layers,
                                                            .tb_scaling_field = dl_tbs_cfg_ref.tb_scaling_field,
                                                            .n_prb            = dl_tbs_cfg_ref.n_prb});
  return tbs_bytes.value();
}

unsigned rate_estimator::estimate_max_ul_tbs(const ue_cell& ue_cc) const
{
  static constexpr pusch_mcs_table ref_mcs_table          = pusch_mcs_table::qam256;
  static constexpr bool            use_transform_precoder = false;

  const sch_mcs_index mcs = ue_cc.link_adaptation_controller().calculate_ul_mcs(ref_mcs_table, false);

  const unsigned            nof_layers = ue_cc.channel_state_manager().get_nof_ul_layers();
  const sch_mcs_description mcs_info   = pusch_mcs_get_config(ref_mcs_table, mcs, use_transform_precoder, false);

  units::bytes tbs_bytes =
      tbs_calculator_calculate(tbs_calculator_configuration{.nof_symb_sh  = ul_tbs_cfg_ref.nof_symb_sh,
                                                            .nof_dmrs_prb = ul_dmrs_rbs_per_nof_layers[nof_layers - 1],
                                                            .nof_oh_prb   = ul_tbs_cfg_ref.nof_oh_prb,
                                                            .mcs_descr    = mcs_info,
                                                            .nof_layers   = nof_layers,
                                                            .tb_scaling_field = ul_tbs_cfg_ref.tb_scaling_field,
                                                            .n_prb            = ul_tbs_cfg_ref.n_prb});

  // Return the estimated throughput, considering that the number of bytes is for a slot.
  return tbs_bytes.value();
}

scheduler_time_qos::scheduler_time_qos(const time_qos_scheduler_config& policy_cfg_,
                                       const cell_configuration&        cell_cfg) :
  params(policy_cfg_), rate_estim(cell_cfg)
{
}

void scheduler_time_qos::add_ue(du_ue_index_t ue_index)
{
  ue_history_db.add_ue(ue_index);
}

void scheduler_time_qos::rem_ue(du_ue_index_t ue_index)
{
  ue_history_db.rem_ue(ue_index);
}

void scheduler_time_qos::compute_ue_dl_priorities(slot_point               pdcch_slot,
                                                  slot_point               pdsch_slot,
                                                  span<ue_newtx_candidate> ue_candidates)
{
  last_pdsch_slot = pdsch_slot;

  // Compute UE candidate priorities.
  for (auto& u : ue_candidates) {
    u.priority = compute_dl_prio(*u.ue, pdcch_slot, pdsch_slot);
  }
}

void scheduler_time_qos::compute_ue_ul_priorities(slot_point               pdcch_slot,
                                                  slot_point               pusch_slot,
                                                  span<ue_newtx_candidate> ue_candidates)
{
  last_pusch_slot = pusch_slot;

  // Compute UE candidate priorities.
  for (auto& u : ue_candidates) {
    u.priority = compute_ul_prio(*u.ue, pdcch_slot, pusch_slot);
  }
}

void scheduler_time_qos::save_dl_newtx_grants(span<const dl_msg_alloc> dl_grants)
{
  // Save result of DL grants in UE history.
  ue_history_db.save_dl_newtx_grants(dl_grants);
}

void scheduler_time_qos::save_ul_newtx_grants(span<const ul_sched_info> ul_grants)
{
  // Save result of UL grants in UE history.
  ue_history_db.save_ul_newtx_grants(ul_grants);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// [Implementation-defined] Helper value to set a maximum metric weight that is low enough to avoid overflows during
// the final QoS weight computation.
static constexpr double max_metric_weight = 1.0e12;

static double compute_pf_metric(double estim_rate, double avg_rate, double fairness_coeff)
{
  double pf_weight = 0.0;
  if (estim_rate > 0) {
    if (avg_rate != 0) {
      if (fairness_coeff >= MAX_PF_COEFF) {
        // For very high coefficients, the pow(.) will be very high, leading to pf_weight of 0 due to lack of precision.
        // In such scenarios, we change the way to compute the PF weight. Instead, we completely disregard the estimated
        // rate, as its impact is minimal.
        pf_weight = 1 / avg_rate;
      } else {
        pf_weight = estim_rate / std::pow(avg_rate, fairness_coeff);
      }
    } else {
      // In case the avg rate is zero, the division would be inf. Instead, we give the highest priority to the UE.
      pf_weight = max_metric_weight;
    }
  }
  return pf_weight;
}

static double combine_qos_metrics(double                           pf_weight,
                                  double                           gbr_weight,
                                  double                           prio_weight,
                                  double                           delay_weight,
                                  const time_qos_scheduler_config& policy_params)
{
  if (policy_params.combine_function == time_qos_scheduler_config::combine_function_type::gbr_prioritized and
      gbr_weight > 1.0) {
    // GBR target has not been met and we prioritize GBR over PF.
    pf_weight = std::max(1.0, pf_weight);
  }

  // The return is a combination of QoS priority, ARP priority, GBR and PF weight functions.
  return gbr_weight * pf_weight * prio_weight * delay_weight;
}

/// \brief Computes DL rate weight used in computation of DL priority value for a UE in a slot.
static double compute_dl_qos_weights(const slice_ue&                  u,
                                     double                           estim_dl_rate,
                                     double                           avg_dl_rate,
                                     slot_point                       slot_tx,
                                     const time_qos_scheduler_config& policy_params)
{
  if (avg_dl_rate == 0) {
    // Highest priority to UEs that have not yet received any allocation.
    return std::numeric_limits<double>::max();
  }

  static constexpr uint16_t max_combined_prio_level = qos_prio_level_t::max() * arp_prio_level_t::max();
  uint16_t                  min_combined_prio       = max_combined_prio_level;
  double                    gbr_weight              = 0;
  double                    delay_weight            = 0;
  if (policy_params.gbr_enabled or policy_params.priority_enabled or policy_params.pdb_enabled) {
    for (logical_channel_config_ptr lc : *u.logical_channels()) {
      if (not lc->qos.has_value() or not u.contains(lc->lcid) or u.pending_dl_newtx_bytes(lc->lcid) == 0) {
        // LC is not part of the slice, No QoS config was provided for this LC or there is no pending data for this LC
        continue;
      }

      // Track the LC with the lowest combined priority (combining QoS and ARP priority levels).
      if (policy_params.priority_enabled) {
        min_combined_prio = std::min(
            static_cast<uint16_t>(lc->qos->qos.priority.value() * lc->qos->arp_priority.value()), min_combined_prio);
      }

      slot_point hol_toa = u.dl_hol_toa(lc->lcid);
      if (hol_toa.valid() and slot_tx >= hol_toa) {
        const unsigned hol_delay_ms = (slot_tx - hol_toa) / slot_tx.nof_slots_per_subframe();
        const unsigned pdb          = lc->qos->qos.packet_delay_budget_ms;
        delay_weight += hol_delay_ms / static_cast<double>(pdb);
      }

      if (not lc->qos->gbr_qos_info.has_value()) {
        // LC is a non-GBR flow.
        continue;
      }

      // GBR flow.
      double dl_avg_rate = u.dl_avg_bit_rate(lc->lcid);
      if (dl_avg_rate != 0) {
        gbr_weight += std::min(lc->qos->gbr_qos_info->gbr_dl / dl_avg_rate, max_metric_weight);
      } else {
        gbr_weight += max_metric_weight;
      }
    }
  }

  // If no QoS flows are configured, the weight is set to 1.0.
  gbr_weight   = policy_params.gbr_enabled and gbr_weight != 0 ? gbr_weight : 1.0;
  delay_weight = policy_params.pdb_enabled and delay_weight != 0 ? delay_weight : 1.0;

  double pf_weight = compute_pf_metric(estim_dl_rate, avg_dl_rate, policy_params.pf_fairness_coeff);
  // If priority is disabled, set the priority weight of all UEs to 1.0.
  double prio_weight = policy_params.priority_enabled ? (max_combined_prio_level + 1 - min_combined_prio) /
                                                            static_cast<double>(max_combined_prio_level + 1)
                                                      : 1.0;

  // The return is a combination of ARP and QoS priorities, GBR and PF weight functions.
  return combine_qos_metrics(pf_weight, gbr_weight, prio_weight, delay_weight, policy_params);
}

/// \brief Computes UL weights used in computation of UL priority value for a UE in a slot.
static double compute_ul_qos_weights(const slice_ue&                  u,
                                     double                           estim_ul_rate,
                                     double                           avg_ul_rate,
                                     const time_qos_scheduler_config& policy_params,
                                     slot_point                       pusch_slot)
{
  if (avg_ul_rate == 0) {
    // Highest priority to UEs that have not yet received any allocation.
    return max_sched_priority;
  }

  if (u.has_pending_sr()) {
    // High priority given to UEs that have pending SRs, prioritizing those with the oldest SRs.
    // We subtract to \c max_sched_priority a factor that is a multiple of \c slot_prio_coeff and proportional to the
    // the slot difference between pusch_slot and the slot at which the SR was received.
    static constexpr double slot_prio_coeff = max_sched_priority * 1e-6;
    auto                    slot_diff       = pusch_slot - u.pending_sr_slot_rx();
    return max_sched_priority - (pusch_slot.nof_slots_per_hyper_system_frame() - slot_diff) * slot_prio_coeff;
  }

  static constexpr uint16_t max_combined_prio_level = qos_prio_level_t::max() * arp_prio_level_t::max();
  uint16_t                  min_combined_prio       = max_combined_prio_level;
  double                    gbr_weight              = 0;
  if (policy_params.gbr_enabled or policy_params.priority_enabled) {
    for (logical_channel_config_ptr lc : *u.logical_channels()) {
      if (not u.contains(lc->lcid) or not lc->qos.has_value() or u.pending_ul_unacked_bytes(lc->lc_group) == 0) {
        // LC is not part of the slice or no QoS config was provided for this LC or there are no pending bytes for this
        // group.
        continue;
      }

      // Track the LC with the lowest combined priority (combining QoS and ARP priority levels).
      if (policy_params.priority_enabled) {
        min_combined_prio = std::min(
            static_cast<uint16_t>(lc->qos->qos.priority.value() * lc->qos->arp_priority.value()), min_combined_prio);
      }

      if (not lc->qos->gbr_qos_info.has_value()) {
        // LC is a non-GBR flow.
        continue;
      }

      // GBR flow.
      lcg_id_t lcg_id  = u.get_lcg_id(lc->lcid);
      double   ul_rate = u.ul_avg_bit_rate(lcg_id);
      if (ul_rate != 0) {
        gbr_weight += std::min(lc->qos->gbr_qos_info->gbr_ul / ul_rate, max_metric_weight);
      } else {
        gbr_weight = max_metric_weight;
      }
    }
  }

  // If no GBR flows are configured, the gbr rate is set to 1.0.
  gbr_weight = policy_params.gbr_enabled and gbr_weight != 0 ? gbr_weight : 1.0;
  // If priority is disabled, set the priority weight of all UEs to 1.0.
  double prio_weight = policy_params.priority_enabled ? (max_combined_prio_level + 1 - min_combined_prio) /
                                                            static_cast<double>(max_combined_prio_level + 1)
                                                      : 1.0;
  double pf_weight   = compute_pf_metric(estim_ul_rate, avg_ul_rate, policy_params.pf_fairness_coeff);

  return combine_qos_metrics(pf_weight, gbr_weight, prio_weight, 1.0, policy_params);
}

ue_sched_priority scheduler_time_qos::compute_dl_prio(const slice_ue& u, slot_point pdcch_slot, slot_point pdsch_slot)
{
  const auto&         ue_cc    = u.get_cc();
  const du_ue_index_t ue_index = u.ue_index();
  // This should be ensured at this point.
  ocudu_sanity_check(ue_cc.is_pdsch_enabled(pdcch_slot, pdsch_slot) and ue_cc.harqs.has_empty_dl_harqs() and
                         u.has_pending_dl_newtx_bytes(),
                     "Invalid DL UE candidate state");

  // Calculate DL QoS-aware priority.
  // NOTE: Estimated instantaneous DL rate is calculated assuming entire BWP CRBs are allocated to UE.
  const unsigned estimated_max_tbs = rate_estim.estimate_max_dl_tbs(ue_cc);
  if (estimated_max_tbs == 0) {
    // No DL rate can be achieved.
    return forbid_prio;
  }

  const double current_total_avg_rate = ue_history_db[ue_index].dl_avg_rate();
  return compute_dl_qos_weights(u, estimated_max_tbs, current_total_avg_rate, pdcch_slot, params);
}

ue_sched_priority scheduler_time_qos::compute_ul_prio(const slice_ue& u, slot_point pdcch_slot, slot_point pusch_slot)
{
  const ue_cell& ue_cc = u.get_cc();
  ocudu_sanity_check(not ue_cc.is_in_fallback_mode() and ue_cc.is_pusch_enabled(pdcch_slot, pusch_slot) and
                         ue_cc.harqs.has_empty_ul_harqs() and u.pending_ul_newtx_bytes() > 0,
                     "UE UL candidate in invalid state");

  // Calculate UL PF priority.
  // NOTE: Estimated instantaneous UL rate is calculated assuming entire BWP CRBs are allocated to UE.
  const unsigned estimated_max_tbs = rate_estim.estimate_max_ul_tbs(ue_cc);
  if (estimated_max_tbs == 0) {
    return forbid_prio;
  }
  const double current_avg_rate = ue_history_db[u.ue_index()].ul_avg_rate();

  // Compute LC weight function.
  return compute_ul_qos_weights(u, estimated_max_tbs, current_avg_rate, params, pusch_slot);
}
