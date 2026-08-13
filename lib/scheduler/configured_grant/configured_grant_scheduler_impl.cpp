// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "configured_grant_scheduler_impl.h"
#include "../cell/resource_grid.h"
#include "../support/bwp_helpers.h"
#include "../support/dmrs_helpers.h"
#include "../support/mcs_tbs_calculator.h"
#include "../support/repetition_helpers.h"
#include "../support/sch_pdu_builder.h"
#include "../uci_scheduling/uci_allocator_impl.h"
#include "ocudu/ran/csi_report/csi_report_config_helpers.h"
#include "ocudu/ran/direct_current_offset.h"
#include <optional>

using namespace ocudu;

static harq_id_t get_harq_id(slot_point                      pusch_slot,
                             unsigned                        symbol,
                             cg_configuration::periodicity_t periodicity,
                             uint8_t                         nof_harq_processes)
{
  // As per TS 38.321, Section 5.4.1.
  const unsigned current_symbol =
      static_cast<unsigned>(pusch_slot.system_slot()) * NOF_OFDM_SYM_PER_SLOT_NORMAL_CP + symbol;
  const unsigned periodicity_sym = static_cast<unsigned>(periodicity) * NOF_OFDM_SYM_PER_SLOT_NORMAL_CP;
  return to_harq_id((current_symbol / periodicity_sym) % nof_harq_processes);
}

static vrb_interval compute_cg_vrbs(const cg_configuration::rrc_configured_ul_grant& ul_grant)
{
  // Compute VRBs: CG PUSCH uses non-interleaved VRB-to-PRB mapping, so VRBs = PRBs.
  const auto& cg_freq_alloc = std::get<ra_frequency_type1_configuration>(ul_grant.freq_domain_res);
  return vrb_interval::start_and_len(cg_freq_alloc.start_vrb, cg_freq_alloc.length_vrb);
}

configured_grant_scheduler_impl::configured_grant_scheduler_impl(const cell_configuration& cell_cfg_,
                                                                 uci_allocator&            uci_alloc_,
                                                                 ue_repository&            ues_) :
  cell_cfg(cell_cfg_), uci_alloc(uci_alloc_), ues(ues_), logger(ocudulog::fetch_basic_logger("SCHED"))
{
  periodic_pusch_slot_wheel.resize(max_cg_slot_periodicity);

  // Pre-reserve space for the UEs that will be added.
  // Note: The list is cleared every slot, so it generally holds far fewer UEs than the cell can have contexts for.
  updated_ues.reserve(cell_cfg.max_nof_ue_contexts);
}

const ue_cell* configured_grant_scheduler_impl::get_ue_cell(rnti_t rnti) const
{
  auto* u = ues.find_by_rnti(rnti);
  if (u != nullptr) {
    return u->find_cell(cell_cfg.cell_index);
  }
  return nullptr;
}

void configured_grant_scheduler_impl::update_harq_reservation(const ue_cell_configuration& ue_cfg) const
{
  auto* u = ues.find_by_rnti(ue_cfg.crnti);
  if (u == nullptr) {
    return;
  }
  auto* ue_cc = u->find_cell(cell_cfg.cell_index);
  if (ue_cc == nullptr) {
    return;
  }

  const auto*    ul_ded = ue_cfg.init_bwp().ul.ded();
  const unsigned nof_cg_reserved =
      (ul_ded != nullptr and ul_ded->cg_cfg.has_value() and ul_ded->cg_cfg->rrc_configured_ul_grant_cfg.has_value())
          ? ul_ded->cg_cfg->nof_harq_processes
          : 0U;

  ue_cc->harqs.reconfigure(ue_cc->harqs.nof_dl_harqs(), ue_cc->harqs.nof_ul_harqs(), {}, {}, nof_cg_reserved);
}

void configured_grant_scheduler_impl::add_ue_to_wheel(const ue_cell_configuration& ue_cfg)
{
  auto* u = ues.find_by_rnti(ue_cfg.crnti);
  if (u == nullptr) {
    logger.error("rnti={}: UE not found in the CG scheduler UEs repo", ue_cfg.crnti);
    return;
  }

  if (ue_cfg.init_bwp().ul.ded() == nullptr or not ue_cfg.init_bwp().ul.ded()->cg_cfg.has_value()) {
    return;
  }
  const cg_configuration& cg_cfg = ue_cfg.init_bwp().ul.ded()->cg_cfg.value();

  // Only handle Type 1 CG (RRC-configured grant; Type 2 requires DCI activation).
  if (not cg_cfg.rrc_configured_ul_grant_cfg.has_value()) {
    return;
  }
  const auto& ul_grant = cg_cfg.rrc_configured_ul_grant_cfg.value();

  const auto period_slots = static_cast<unsigned>(cg_cfg.periodicity);

  // Fill the slot wheel at every slot where a CG PUSCH opportunity occurs.
  for (unsigned wheel_offset = ul_grant.time_domain_offset; wheel_offset < max_cg_slot_periodicity;
       wheel_offset += period_slots) {
    auto& slot_wheel = periodic_pusch_slot_wheel[wheel_offset];

    if (std::find(slot_wheel.begin(), slot_wheel.end(), ue_cfg.crnti) != slot_wheel.end()) {
      logger.error("rnti={}: CG grant already present in slot wheel at offset={}", ue_cfg.crnti, wheel_offset);
      continue;
    }
    slot_wheel.push_back(ue_cfg.crnti);
  }

  // Register the UE in the list of recently updated UEs, so that its CG resources get pre-reserved over the whole
  // resource grid at the next run_slot().
  updated_ues.push_back(ue_cfg.crnti);

  // Register the UE TBS in the TBS table.
  pusch_config_params pusch_params = build_cg_pusch_cfg_params(ue_cfg);
  const auto          cg_vrbs      = compute_cg_vrbs(ul_grant);
  const units::bytes  tbs          = compute_ul_tbs_unsafe(pusch_params, sch_mcs_index{ul_grant.mcs}, cg_vrbs.length());
  ocudu_assert(not ue_tbs_values.contains(u->ue_index), "UE={} already present in the TBS table", u->ue_index);
  ue_tbs_values.emplace(u->ue_index, tbs);
}

void configured_grant_scheduler_impl::rem_ue(const ue_cell_configuration& ue_cfg)
{
  const auto* u = ues.find_by_rnti(ue_cfg.crnti);
  if (u == nullptr) {
    logger.error("rnti={}: UE not found in the CG scheduler UEs repo during UE removal", ue_cfg.crnti);
    return;
  }

  if (ue_cfg.init_bwp().ul.ded() == nullptr or not ue_cfg.init_bwp().ul.ded()->cg_cfg.has_value()) {
    return;
  }
  const cg_configuration& cg_cfg = ue_cfg.init_bwp().ul.ded()->cg_cfg.value();

  if (not cg_cfg.rrc_configured_ul_grant_cfg.has_value()) {
    return;
  }
  const auto& ul_grant = cg_cfg.rrc_configured_ul_grant_cfg.value();

  const auto period_slots = static_cast<unsigned>(cg_cfg.periodicity);

  for (unsigned wheel_offset = ul_grant.time_domain_offset; wheel_offset < max_cg_slot_periodicity;
       wheel_offset += period_slots) {
    auto& slot_wheel = periodic_pusch_slot_wheel[wheel_offset];

    auto* it = std::find(slot_wheel.begin(), slot_wheel.end(), ue_cfg.crnti);
    if (it == slot_wheel.end()) {
      logger.error(
          "rnti={}: CG grant not found in slot wheel at offset={} during UE removal", ue_cfg.crnti, wheel_offset);
      continue;
    }

    // Swap with last element and pop for O(1) removal.
    std::swap(*it, slot_wheel.back());
    slot_wheel.pop_back();
  }

  // Remove UE TBS from TBS table.
  ocudu_assert(ue_tbs_values.contains(u->ue_index), "UE={} not found in the TBS table", u->ue_index);
  ue_tbs_values.erase(u->ue_index);
}

void configured_grant_scheduler_impl::add_reconf_ue(const ue_cell_configuration& new_ue_cfg,
                                                    const ue_cell_configuration* old_ue_cfg)
{
  if (old_ue_cfg == nullptr) {
    add_ue_to_wheel(new_ue_cfg);
    update_harq_reservation(new_ue_cfg);
    return;
  }

  const auto* new_ul_ded = new_ue_cfg.init_bwp().ul.ded();
  const auto* old_ul_ded = old_ue_cfg->init_bwp().ul.ded();

  if (new_ul_ded != nullptr and old_ul_ded != nullptr and new_ul_ded->cg_cfg.has_value() and
      old_ul_ded->cg_cfg.has_value() and new_ul_ded->cg_cfg.value() == old_ul_ded->cg_cfg.value()) {
    // CG configuration unchanged — nothing to do.
    return;
  }

  rem_ue(*old_ue_cfg);
  add_ue_to_wheel(new_ue_cfg);
  update_harq_reservation(new_ue_cfg);
}

void configured_grant_scheduler_impl::run_slot(cell_resource_allocator& cell_alloc)
{
  // For UEs whose CG configuration was recently added/updated, pre-reserve their CG PUSCH resources over the whole
  // resource grid, so that dynamic PUSCH grants scheduled in advance do not use these resources.
  reserve_updated_ues_resources(cell_alloc);

  // Only pre-reserve in the farthest slot in the grid, as the previous part of the grid has been filled in earlier
  // calls to this function.
  reserve_slot_cg_resources(cell_alloc[cell_alloc.max_ul_slot_alloc_delay]);

  // We only allocate CG PUSCH for the current slot; else, we incur the possibility that the PUSCH gets allocated before
  // the PUCCH.
  static constexpr unsigned look_ahead_alloc_slots = 0U;
  allocate_slot_cg_opportunities(cell_alloc[look_ahead_alloc_slots]);
}

void configured_grant_scheduler_impl::reserve_updated_ues_resources(cell_resource_allocator& res_alloc)
{
  // For all UEs whose CG config has been recently updated, reserve their CG resources up until one slot before the
  // farthest slot in the resource grid (the farthest slot is handled by reserve_slot_cg_resources()).
  for (const rnti_t rnti : updated_ues) {
    const ue_cell* ue_cc = get_ue_cell(rnti);
    if (ue_cc == nullptr) {
      logger.error("rnti={}: UE for which CG resources are being reserved was not found", rnti);
      continue;
    }

    for (unsigned n = 0; n != res_alloc.max_ul_slot_alloc_delay; ++n) {
      cell_slot_resource_allocator& slot_alloc = res_alloc[n];
      const auto&                   rnti_list  = periodic_pusch_slot_wheel[slot_alloc.slot.to_uint()];
      if (std::find(rnti_list.begin(), rnti_list.end(), rnti) != rnti_list.end()) {
        reserve_cg_resources(slot_alloc, *ue_cc);
      }
    }
  }

  // Clear the list of updated UEs.
  updated_ues.clear();
}

void configured_grant_scheduler_impl::reserve_slot_cg_resources(cell_slot_resource_allocator& slot_alloc) const
{
  const auto& rnti_list = periodic_pusch_slot_wheel[slot_alloc.slot.to_uint()];
  for (const rnti_t rnti : rnti_list) {
    const ue_cell* ue_cc = get_ue_cell(rnti);
    if (ue_cc == nullptr) {
      continue;
    }
    reserve_cg_resources(slot_alloc, *ue_cc);
  }
}

void configured_grant_scheduler_impl::reserve_cg_resources(cell_slot_resource_allocator& slot_alloc,
                                                           const ue_cell&                ue_cc) const
{
  // Skip if UL is not enabled in this slot (e.g., TDD DL slot, or the slot falls in the UL measurement gap window).
  if (not ue_cc.is_ul_enabled(slot_alloc.slot)) {
    return;
  }
  const ue_cell_configuration& ue_cfg = ue_cc.cfg();

  if (not cell_cfg.params.ul_cfg_common.init_ul_bwp.pusch_cfg_common.has_value()) {
    return;
  }
  const auto& pusch_td_list = cell_cfg.params.ul_cfg_common.init_ul_bwp.pusch_cfg_common->pusch_td_alloc_list;

  // NOTE: the CG and UL grant configs were validated when the UE was added to the wheel.
  const auto& ul_grant = ue_cfg.init_bwp().ul.ded()->cg_cfg.value().rrc_configured_ul_grant_cfg.value();

  // Compute CRBs: CG PUSCH uses non-interleaved VRB-to-PRB mapping, so VRBs = PRBs.
  const bwp_configuration& ul_bwp_cfg    = cell_cfg.params.ul_cfg_common.init_ul_bwp.generic_params;
  const auto&              cg_freq_alloc = std::get<ra_frequency_type1_configuration>(ul_grant.freq_domain_res);
  const auto               cg_vrbs = vrb_interval::start_and_len(cg_freq_alloc.start_vrb, cg_freq_alloc.length_vrb);
  const crb_interval       crbs    = prb_to_crb(ul_bwp_cfg, cg_vrbs.convert_to<prb_interval>());

  // Mark the CG PUSCH resources as allocated in the UL resource grid.
  // NOTE: the validity ul_grant.time_domain_allocation w.r.t. pusch_td_list has been verified in the config validator.
  slot_alloc.ul_res_grid.fill(grant_info{ul_bwp_cfg.scs, pusch_td_list[ul_grant.time_domain_allocation].symbols, crbs});
}

void configured_grant_scheduler_impl::stop()
{
  updated_ues.clear();
  for (auto& sl : periodic_pusch_slot_wheel) {
    sl.clear();
  }
  ue_tbs_values.clear();
}

pusch_config_params
configured_grant_scheduler_impl::build_cg_pusch_cfg_params(const ue_cell_configuration& ue_cell_cfg) const
{
  const auto& pusch_td_list = cell_cfg.params.ul_cfg_common.init_ul_bwp.pusch_cfg_common->pusch_td_alloc_list;

  const auto* ul_ded   = ue_cell_cfg.init_bwp().ul.ded();
  const auto& cg_cfg   = ul_ded->cg_cfg.value();
  const auto& ul_grant = cg_cfg.rrc_configured_ul_grant_cfg.value();

  const pusch_time_domain_resource_allocation& pusch_td_cfg = pusch_td_list[ul_grant.time_domain_allocation];
  pusch_config_params                          pusch_params;
  pusch_params.symbols = pusch_td_cfg.symbols;

  // Build DMRS information from the CG-specific DMRS configuration.
  static constexpr unsigned nof_layers           = 1;
  static constexpr bool     are_both_cws_enabled = false;
  const dmrs_information    dmrs                 = make_dmrs_info_dedicated(pusch_td_cfg,
                                                         cell_cfg.params.pci,
                                                         cell_cfg.params.dmrs_typeA_pos,
                                                         cg_cfg.cg_dmrs_cfg,
                                                         nof_layers,
                                                         cell_cfg.params.ul_carrier.nof_ant,
                                                         are_both_cws_enabled);

  // Build PUSCH configuration parameters for TBS computation.
  pusch_params.dmrs       = dmrs;
  pusch_params.mcs_table  = cg_cfg.mcs_table;
  pusch_params.nof_layers = nof_layers;
  // TODO: Import p_pi2bpsk_present from PUSCH Config once it will have been added there.
  pusch_params.tp_pi2bpsk_present = false;
  // CG PUSCH uses CP-OFDM (no transform precoding). The mcs_table_transform_precoder field is separate.
  pusch_params.use_transform_precoder = false;
  // As per TS 38.214, Section 5.1.3.2 and 6.1.4.2, and TS 38.212, Section 7.3.1.1 and 7.3.1.2, TB scaling filed is only
  // used for DCI Format 1-0 (in the DL). Therefore, for the PUSCH this is set to 0.
  pusch_params.tb_scaling_field = 0;
  // As per TS 38.214, Section 6.1.4.2, nof_oh_prb equals xOverhead when configured; otherwise 0.
  pusch_params.nof_oh_prb = ue_cell_cfg.pusch_serving_cell_cfg() != nullptr
                                ? static_cast<unsigned>(ue_cell_cfg.pusch_serving_cell_cfg()->x_ov_head)
                                : static_cast<unsigned>(x_overhead::not_set);
  // If aperiodic CSI is configured, it is assumed that it will be carried by dynamic grants.
  pusch_params.aperiodic_csi = false;

  // NOTE: pusch_params.nof_harq_ack_bits can only be built at scheduling time, as it needs the pusch slot.
  return pusch_params;
}

void configured_grant_scheduler_impl::allocate_slot_cg_opportunities(cell_slot_resource_allocator& slot_alloc) const
{
  const auto& rnti_list = periodic_pusch_slot_wheel[slot_alloc.slot.to_uint()];
  for (const rnti_t rnti : rnti_list) {
    allocate_cg_opportunity(slot_alloc, rnti);
  }
}

bool configured_grant_scheduler_impl::validate_cg_opportunity(const cell_slot_resource_allocator& slot_alloc,
                                                              rnti_t                              rnti) const
{
  // Fetch UE and its cell context.
  auto* u = ues.find_by_rnti(rnti);
  if (u == nullptr) {
    logger.error("rnti={}: CG opportunity scheduled but UE not found", rnti);
    return false;
  }

  ocudu_assert(
      u->ue_cfg_dedicated()->get_cs_rnti().has_value(), "Missing CS-RNTI for UE={} with Configured Grant", rnti);

  auto* ue_cc = u->find_cell(cell_cfg.cell_index);
  if (ue_cc == nullptr) {
    logger.error("rnti={}: CG opportunity scheduled but UE cell not found", rnti);
    return false;
  }
  // Skip if UL is not enabled in this slot (e.g., TDD DL slot, or the slot falls in the UL measurement gap window).
  if (not ue_cc->is_ul_enabled(slot_alloc.slot)) {
    return false;
  }

  const slot_point pusch_slot = slot_alloc.slot;

  // Check that the PUSCH result list has capacity.
  if (slot_alloc.result.ul.puschs.full()) {
    logger.warning("rnti={}: CG PUSCH cannot be allocated at slot={}: PUSCH list is full", rnti, pusch_slot);
    return false;
  }

  return true;
}

bool configured_grant_scheduler_impl::allocate_cg_opportunity(cell_slot_resource_allocator& slot_alloc,
                                                              rnti_t                        rnti) const
{
  if (not validate_cg_opportunity(slot_alloc, rnti)) {
    return false;
  }

  // NOTE: the existence of u and ue_cc has been validated above.
  auto*                        u      = ues.find_by_rnti(rnti);
  auto*                        ue_cc  = u->find_cell(cell_cfg.cell_index);
  const ue_cell_configuration& ue_cfg = ue_cc->cfg();

  const slot_point pusch_slot = slot_alloc.slot;
  const auto*      ul_ded     = ue_cfg.init_bwp().ul.ded();
  const auto&      cg_cfg     = ul_ded->cg_cfg.value();
  const auto&      ul_grant   = cg_cfg.rrc_configured_ul_grant_cfg.value();

  pusch_config_params pusch_params = build_cg_pusch_cfg_params(ue_cfg);
  // The HARQ-ACK bits are the only element of pusch_params that need to be updated at scheduling time.
  pusch_params.nof_harq_ack_bits = uci_alloc.get_scheduled_pdsch_counter_in_ue_uci(pusch_slot, u->crnti);

  static constexpr unsigned nof_harq_retx = 0;
  const harq_id_t           h_id =
      get_harq_id(pusch_slot, pusch_params.symbols.start(), cg_cfg.periodicity, cg_cfg.nof_harq_processes);
  const unsigned cg_harq_timeout = cg_configuration::configured_grant_timer * static_cast<unsigned>(cg_cfg.periodicity);
  auto h_ul = ue_cc->harqs.alloc_ul_harq(pusch_slot, nof_harq_retx, cg_harq_alloc_params{h_id, cg_harq_timeout});
  ocudu_assert(h_ul.has_value(), "Failed to allocate UL HARQ id={}", fmt::underlying(h_id));

  // Compute VRBs: CG PUSCH uses non-interleaved VRB-to-PRB mapping, so VRBs = PRBs.
  const auto cg_vrbs = compute_cg_vrbs(ul_grant);
  // NOTE: the CG PUSCH RBs and symbols have already been pre-reserved in the UL resource grid, either by
  // reserve_slot_cg_resources() when this slot entered the resource grid, or by reserve_updated_ues_resources() right
  // after the UE was added; hence, no collision check nor grid fill is needed here.

  // Compute TBS from the configured MCS and VRB count.
  const sch_mcs_index mcs_idx{ul_grant.mcs};
  // NOTE: the TBS should have been computed to be valid when the UE config was built.
  ocudu_assert(ue_tbs_values.contains(u->ue_index), "UE={} not found in the TBS table", u->ue_index);
  const units::bytes tbs = ue_tbs_values[u->ue_index];

  // Fill UL scheduling result.
  ul_sched_info& sched_info = slot_alloc.result.ul.puschs.emplace_back();

  // Configured Grant repetitions are not currently supported.
  constexpr unsigned rep_idx = 0U;
  build_pusch_cs_rnti(sched_info.pusch_cfg,
                      u->ue_cfg_dedicated()->get_cs_rnti().value(),
                      pusch_params,
                      {mcs_idx, tbs},
                      ue_cfg,
                      ue_cc->active_bwp(),
                      cg_vrbs,
                      get_cg_repetition_rv(cg_cfg.rep, rep_idx),
                      h_ul.value().id());

  // Check if there is any UCI grant allocated on the PUCCH that can be moved to the PUSCH.
  constexpr bool configured_grant = true;
  uci_alloc.multiplex_uci_on_pusch(sched_info, slot_alloc, ue_cfg, pusch_params.aperiodic_csi, configured_grant);

  // Fill decision context (informational; not forwarded to the PHY).
  sched_info.context.ue_index = u->ue_index;
  // Not applicable for CG Type 1 (no PDCCH).
  sched_info.context.ss_id = to_search_space_id(0);
  // Not applicable for CG Type 1 (no PDCCH timing).
  sched_info.context.k2 = 0;
  // With CG, the periodic allocated grants always contained new-tx.
  sched_info.context.nof_retxs  = nof_harq_retx;
  sched_info.context.nof_oh_prb = pusch_params.nof_oh_prb;

  // NOTE 1: Do not reset the SR here, as it might be needed for SRB, which we exclude from CG.
  // NOTE 2: We should call ue_logical_channel_repository::handle_ul_grant() here, but that would not be safe, as we
  // might wrongly update the SRB in that. Unless we defined by design that SRBs are mapped to a specific LCG (not
  // defined by the standard), we should skip updating the LCG repository.

  return true;
}
