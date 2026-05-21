// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "paging_scheduler.h"
#include "../support/dci_builder.h"
#include "../support/dmrs_helpers.h"
#include "../support/pdsch/pdsch_default_time_allocation.h"
#include "../support/prbs_calculator.h"
#include "../support/rb_helper.h"
#include "../support/sch_pdu_builder.h"
#include "ocudu/ocudulog/ocudulog.h"
#include "ocudu/ran/slot_point_extended.h"
#include "ocudu/support/math/mod_math_utils.h"

using namespace ocudu;

// (Implementation-defined) limit for maximum number of pending paging indications.
static constexpr size_t PAGING_INFO_QUEUE_SIZE = 128;

paging_scheduler::paging_scheduler(const cell_configuration& cell_cfg_,
                                   pdcch_resource_allocator& pdcch_sch_,
                                   unsigned                  nof_slots_ahead_sched_) :
  expert_cfg(cell_cfg_.expert_cfg),
  cell_cfg(cell_cfg_),
  pdcch_sch(pdcch_sch_),
  nof_slots_ahead_sched(nof_slots_ahead_sched_),
  default_paging_cycle(static_cast<unsigned>(cell_cfg.params.dl_cfg_common.pcch_cfg.default_paging_cycle)),
  nof_pf_per_drx_cycle(static_cast<unsigned>(cell_cfg.params.dl_cfg_common.pcch_cfg.nof_pf)),
  paging_frame_offset(cell_cfg.params.dl_cfg_common.pcch_cfg.paging_frame_offset),
  nof_po_per_pf(static_cast<unsigned>(cell_cfg.params.dl_cfg_common.pcch_cfg.ns)),
  slot_helper(cell_cfg_),
  new_paging_notifications(PAGING_INFO_QUEUE_SIZE),
  logger(ocudulog::fetch_basic_logger("SCHED"))
{
  ocudu_assert(cell_cfg.params.dl_cfg_common.init_dl_bwp.pdcch_common.paging_search_space_id.has_value(),
               "Paging Search Space not configured in DL BWP.");

  paging_pending_ues.reserve(MAX_NOF_PENDING_PAGINGS);

  // Pre-fetch search space config.
  for (const auto& cfg : cell_cfg.params.dl_cfg_common.init_dl_bwp.pdcch_common.search_spaces) {
    if (cfg.get_id() != cell_cfg.params.dl_cfg_common.init_dl_bwp.pdcch_common.paging_search_space_id.value()) {
      continue;
    }
    ss_cfg = &cfg;
    break;
  }
  if (ss_cfg == nullptr) {
    ocudu_assertion_failure("Paging Search Space not configured in DL BWP.");
  }

  // See TS 38.214, 5.1.2.2.2, Downlink resource allocation type 1.
  bwp_cfg = cell_cfg.params.dl_cfg_common.init_dl_bwp.generic_params;
  if (ss_cfg->is_common_search_space()) {
    // See TS 38.214, 5.1.2.2.2, Downlink resource allocation type 1.
    if (cell_cfg.params.dl_cfg_common.init_dl_bwp.pdcch_common.coreset0.has_value()) {
      bwp_cfg.crbs = get_coreset0_crbs(cell_cfg.params.dl_cfg_common.init_dl_bwp.pdcch_common);
    }
  }

  // See TS 38.214, Table 5.1.2.1.1-1.
  // TODO: Select PDSCH time domain resource allocation to apply based on SS/PBCH and CORESET mux. pattern.
  pdsch_td_alloc_list = cell_cfg.params.dl_cfg_common.init_dl_bwp.pdsch_common.pdsch_td_alloc_list.empty()
                            ? pdsch_default_time_allocations_default_A_table(bwp_cfg.cp, cell_cfg.params.dmrs_typeA_pos)
                            : cell_cfg.params.dl_cfg_common.init_dl_bwp.pdsch_common.pdsch_td_alloc_list;

  // Generate an empty vector for each element of pdsch_time_res_idx_to_scheduled_ues_lookup; only then we can reserve
  // the capacity.
  pdsch_time_res_idx_to_scheduled_ues_lookup.resize(pdsch_td_alloc_list.size());
  for (auto& sched_paging_ues : pdsch_time_res_idx_to_scheduled_ues_lookup) {
    sched_paging_ues.reserve(MAX_PAGING_RECORDS_PER_PAGING_PDU);
  }
}

void paging_scheduler::run_slot(cell_resource_allocator& res_grid, uint32_t hyper_sfn_tx)
{
  // Pop pending Paging notification and process them.
  handle_pending_paging_requests();

  // NOTE:
  // - [Implementation defined] The pagingSearchSpace (in PDCCH-Common IE) value in UE's active BWP must be taken into
  //   consideration while paging a UE. However, for simplification we consider the value of pagingSearchSpace in UE's
  //   active BWP to be same as in initial DL BWP in this implementation. This is simplification can be applied due to
  //   the fact that DU is responsible for providing the PDCCH-Common configuration for UE.
  //
  // - [Implementation defined] As per Paging Frame (PF) definition, it may contain one or multiple PO(s) or starting
  //   point of a PO. For simplification, we do not support Paging Occasion which starts in PF and extends over multiple
  //   radio frames i.e. DU must ensure paging configuration is set to avoid Paging Occasion spanning multiple radio
  //   frames.

  // How much far ahead in time the scheduler will allocate the Paging.
  const slot_point_extended sl_tx_ext{res_grid.slot_tx(), hyper_sfn_tx};
  const auto                pdcch_slot = sl_tx_ext + nof_slots_ahead_sched;

  // Verify PDCCH slot is DL enabled.
  if (not cell_cfg.is_dl_enabled(pdcch_slot.without_hyper_sfn())) {
    return;
  }

  // Group pending paging opportunities based on their PDSCH time resource index.
  for (const auto& pg_it : paging_pending_ues) {
    if (std::optional<unsigned> time_res_idx = find_pdsch_time_resource(res_grid, pg_it.second.request, pdcch_slot);
        time_res_idx.has_value()) {
      // A suitable PDSCH time resource found for this UE.
      pdsch_time_res_idx_to_scheduled_ues_lookup[*time_res_idx].push_back(pg_it.second.request.paging_identity);
    }
  }

  // Update paging re-tries counter for all successful allocations.
  const auto paging_search_space =
      cell_cfg.params.dl_cfg_common.init_dl_bwp.pdcch_common.paging_search_space_id.value();
  for (unsigned pdsch_td_res_idx = 0, sz = pdsch_time_res_idx_to_scheduled_ues_lookup.size(); pdsch_td_res_idx != sz;
       ++pdsch_td_res_idx) {
    const auto& group = pdsch_time_res_idx_to_scheduled_ues_lookup[pdsch_td_res_idx];
    if (not group.empty() and
        allocate_paging(res_grid, pdcch_slot.without_hyper_sfn(), pdsch_td_res_idx, group, paging_search_space)) {
      // Allocation successful. Mark number of retries.
      for (ue_paging_id pg_id : group) {
        auto it = paging_pending_ues.find(pg_id);
        ocudu_sanity_check(it != paging_pending_ues.end(), "Paging info must be present in the pending map.");
        if (++it->second.retry_count >= expert_cfg.pg.max_paging_retries) {
          // Remove from pending paging UEs map if it exceeded maximum retries.
          paging_pending_ues.erase(it);
        }
      }
    }
  }

  // Clear all the computed candidates.
  for (auto& sched_paging_ues : pdsch_time_res_idx_to_scheduled_ues_lookup) {
    sched_paging_ues.clear();
  }
}

void paging_scheduler::handle_pending_paging_requests()
{
  sched_paging_information new_pg_info;
  while (new_paging_notifications.try_pop(new_pg_info)) {
    // Check whether Paging information is already present or not. i.e. tackle repeated Paging attempt from upper
    // layers.
    if (paging_pending_ues.find(new_pg_info.paging_identity) != paging_pending_ues.end()) {
      logger.info("Paging information for id={} discarded. Cause: It is already being handled.",
                  new_pg_info.paging_identity);
      continue;
    }
    if (paging_pending_ues.size() >= MAX_NOF_PENDING_PAGINGS) {
      logger.warning("Paging information for id={} discarded. Cause: Map of paging pending UEs is full.\n",
                     new_pg_info.paging_identity);
      return;
    }
    paging_pending_ues.emplace(new_pg_info.paging_identity, ue_paging_info{.request = new_pg_info, .retry_count = 0});
  }
}

/// Helper function to determine the parameter T, as per TS 38.304, 7.1.
static unsigned compute_paging_T(const sched_paging_information& pg_info, radio_frames default_paging_cycle)
{
  if (pg_info.edrx.has_value()) {
    // UE is operating in eDRX and eDRX is configured by upper layers.
    if (pg_info.edrx->cycle_idle <= hyper_frames{1}) {
      // If T_{eDRX,CN} is no longer than 1024 radio frames.
      return pg_info.edrx->cycle_idle.count();
    }
  }

  // T is determined by the shortest between the UE specific DRX values and the default DRX value broadcast in SI.
  if (pg_info.paging_drx.has_value()) {
    return std::min(default_paging_cycle, pg_info.paging_drx.value()).count();
  }
  return default_paging_cycle.count();
}

bool paging_scheduler::is_paging_opportunity(slot_point_extended pdcch_slot, const sched_paging_information& req) const
{
  // Compute parameters specified in TS 38.304, clause 7.1.
  const unsigned T           = compute_paging_T(req, radio_frames{default_paging_cycle});
  const unsigned UE_ID       = req.ue_identity;
  const unsigned N           = T / nof_pf_per_drx_cycle;
  const unsigned t_div_n     = T / N;
  const unsigned ue_id_mod_n = UE_ID % N;

  // Check if slot Tx is inside the paging frame (PF) for this UE.
  // (SFN + PF_offset) mod T = (T div N)*(UE_ID mod N). See TS 38.304, clause 7.1.
  if (((pdcch_slot.sfn() + paging_frame_offset) % T) != (t_div_n * ue_id_mod_n)) {
    // Not inside PF.
    return false;
  }

  // Index (i_s), indicating the index of the PO.
  // i_s = floor (UE_ID/N) mod Ns.
  const unsigned i_s = (UE_ID / N) % nof_po_per_pf;

  if (not slot_helper.is_paging_slot(pdcch_slot.without_hyper_sfn(), i_s)) {
    // Not a paging slot for this UE.
    return false;
  }

  // Check if UE is inside the Paging Time Window (PTW), in case eDRX is enabled.
  // Note: PTWs only apply in case DRX cycle is longer than 1024 radio frames, as per TS 38.304, 7.4.
  if (req.edrx.has_value() and req.edrx->cycle_idle > hyper_frames{1}) {
    ocudu_assert(req.edrx->ptw_len.has_value(),
                 "If eDRX cycle is longer than 1024 radio frames, Paging Time Window needs to be provided");
    ocudu_assert(*req.edrx->ptw_len < req.edrx->cycle_idle, "PTW must be shorter than eDRX cycle idle");
    const unsigned     UE_ID_H     = req.edrx->hashed_ue_identity;
    const radio_frames Tedrxcn_rfs = req.edrx->cycle_idle;
    const hyper_frames Tedrxcn     = std::chrono::duration_cast<hyper_frames>(Tedrxcn_rfs);
    const unsigned     iedrxcn     = (UE_ID_H / Tedrxcn.count()) % 8U;
    const radio_frames L           = *req.edrx->ptw_len;
    const radio_frames ptw_start{128 * iedrxcn};

    // Check if current slot is inside the PTW.
    const radio_frames win_offset  = hyper_frames{UE_ID_H % Tedrxcn.count()} + ptw_start;
    const radio_frames slot_offset = hyper_frames{pdcch_slot.hyper_sfn()} + radio_frames{pdcch_slot.sfn()};
    if (not is_inside_periodic_window(Tedrxcn_rfs.count(), win_offset.count(), L.count(), slot_offset.count())) {
      // Not inside the PTW.
      return false;
    }
  }

  return true;
}

std::optional<unsigned> paging_scheduler::find_pdsch_time_resource(const cell_resource_allocator&  res_grid,
                                                                   const sched_paging_information& request,
                                                                   slot_point_extended             pdcch_slot)
{
  if (not is_paging_opportunity(pdcch_slot, request)) {
    // Not a paging opportunity for this UE.
    return std::nullopt;
  }

  for (unsigned time_res_idx = 0, sz = pdsch_td_alloc_list.size(); time_res_idx != sz; ++time_res_idx) {
    const auto& group = pdsch_time_res_idx_to_scheduled_ues_lookup[time_res_idx];
    if (group.size() >= MAX_PAGING_RECORDS_PER_PAGING_PDU) {
      // Already reached maximum number of paging records for this PDSCH time resource index.
      continue;
    }

    const unsigned msg_size =
        get_accumulated_paging_msg_size(group) +
        (request.paging_type_indicator == paging_identity_type::cn_ue_paging_identity ? RRC_CN_PAGING_ID_RECORD_SIZE
                                                                                      : RRC_RAN_PAGING_ID_RECORD_SIZE);
    if (is_there_space_available_for_paging(res_grid, time_res_idx, msg_size, pdcch_slot.without_hyper_sfn())) {
      return time_res_idx;
    }
  }

  return std::nullopt;
}

unsigned paging_scheduler::get_accumulated_paging_msg_size(const std::vector<ue_paging_id>& ue_paging_ids) const
{
  // Estimate of the number of bytes required for the upper layer header in bytes.
  static constexpr unsigned RRC_HEADER_SIZE_ESTIMATE = 2U;

  unsigned payload_size = 0;
  for (ue_paging_id pg_id : ue_paging_ids) {
    const auto& pg_info = paging_pending_ues.at(pg_id).request;
    if (pg_info.paging_type_indicator == paging_identity_type::cn_ue_paging_identity) {
      payload_size += RRC_CN_PAGING_ID_RECORD_SIZE;
    } else {
      payload_size += RRC_RAN_PAGING_ID_RECORD_SIZE;
    }
  }

  return RRC_HEADER_SIZE_ESTIMATE + payload_size;
}

void paging_scheduler::handle_paging_information(const sched_paging_information& paging_info)
{
  if (not new_paging_notifications.try_push(paging_info)) {
    logger.warning("Discarding paging information for UE ID={}. Cause: Event queue is full", paging_info.ue_identity);
  }
}

void paging_scheduler::stop()
{
  sched_paging_information paging_info;
  while (new_paging_notifications.try_pop(paging_info)) {
  }
  paging_pending_ues.clear();
}

static bool has_space_for_pdsch(const cell_resource_allocator&                    res_grid,
                                const search_space_configuration&                 ss_cfg,
                                span<const pdsch_time_domain_resource_allocation> pdsch_td_alloc_list,
                                unsigned                                          time_res_idx)
{
  const cell_configuration&           cell_cfg     = res_grid.cfg;
  const cell_slot_resource_allocator& paging_alloc = res_grid[pdsch_td_alloc_list[time_res_idx].k0];

  // Verify Paging slot is DL enabled.
  if (not cell_cfg.is_dl_enabled(paging_alloc.slot)) {
    return false;
  }

  // Note: Unable at the moment to multiplex CSI and PDSCH.
  if (not paging_alloc.result.dl.csi_rs.empty()) {
    return false;
  }

  // Verify there is space in PDSCH and PDCCH result lists for new allocations.
  if (paging_alloc.result.dl.paging_grants.full()) {
    return false;
  }
  const auto&                  pdsch_td_cfg = pdsch_td_alloc_list[time_res_idx];
  const coreset_configuration& cs_cfg       = cell_cfg.get_common_coreset(ss_cfg.get_coreset_id());

  // Check whether PDSCH time domain resource does not overlap with CORESET.
  if (pdsch_td_cfg.symbols.start() < ss_cfg.get_first_symbol_index() + cs_cfg.duration()) {
    return false;
  }

  // Check whether PDSCH time domain resource fits in DL symbols of the slot.
  if (pdsch_td_cfg.symbols.stop() > cell_cfg.get_nof_dl_symbol_per_slot(paging_alloc.slot)) {
    return false;
  }

  return true;
}

bool paging_scheduler::is_there_space_available_for_paging(const cell_resource_allocator& res_grid,
                                                           unsigned                       pdsch_time_res,
                                                           unsigned                       msg_size,
                                                           slot_point                     pdcch_slot) const
{
  if (not has_space_for_pdsch(res_grid, *ss_cfg, pdsch_td_alloc_list, pdsch_time_res)) {
    return false;
  }

  // NOTE:
  // - [Implementation defined] Need to take into account PDSCH Time Domain Resource Allocation in UE's active DL BWP,
  //   for now only initial DL BWP is considered for simplification in this function.

  const pdsch_time_domain_resource_allocation& pdsch_td_cfg = pdsch_td_alloc_list[pdsch_time_res];
  const unsigned                               nof_symb_sh  = pdsch_td_cfg.symbols.length();
  static constexpr unsigned                    nof_layers   = 1;
  // As per Section 5.1.3.2, TS 38.214, nof_oh_prb = 0 if PDSCH is scheduled by PDCCH with a CRC scrambled by P-RNTI.
  static constexpr unsigned nof_oh_prb = 0;
  // As per TS 38.214, Table 5.1.3.2-2.
  // TODO: TBS scaling is assumed to be 0. Need to set correct value.
  static constexpr unsigned tbs_scaling = 0;

  // Generate dmrs information to be passed to (i) the fnc that computes number of RE used for DMRS per RB and (ii) to
  // the fnc that fills the DCI.
  const dmrs_information dmrs_info =
      make_dmrs_info_common(pdsch_td_alloc_list, pdsch_time_res, cell_cfg.params.pci, cell_cfg.params.dmrs_typeA_pos);

  const sch_mcs_description mcs_descr = pdsch_mcs_get_config(pdsch_mcs_table::qam64, expert_cfg.pg.paging_mcs_index);
  const sch_prbs_tbs        paging_prbs_tbs = get_nof_prbs(prbs_calculator_sch_config{
      msg_size, nof_symb_sh, calculate_nof_dmrs_per_rb(dmrs_info), nof_oh_prb, mcs_descr, nof_layers, tbs_scaling});

  // Find available RBs in PDSCH for Paging grant.
  crb_interval paging_crbs;
  {
    const unsigned    nof_paging_rbs = paging_prbs_tbs.nof_prbs;
    const crb_bitmap& used_crbs =
        res_grid[pdcch_slot + pdsch_td_cfg.k0].dl_res_grid.used_crbs(bwp_cfg, pdsch_td_cfg.symbols);
    paging_crbs = rb_helper::find_empty_interval_of_length(used_crbs, nof_paging_rbs);
    if (paging_crbs.length() < nof_paging_rbs) {
      return false;
    }
  }

  return true;
}

bool paging_scheduler::allocate_paging(cell_resource_allocator&         res_grid,
                                       slot_point                       pdcch_slot,
                                       unsigned                         pdsch_time_res,
                                       const std::vector<ue_paging_id>& ue_paging_ids,
                                       search_space_id                  ss_id)
{
  // NOTE:
  // - [Implementation defined] Need to take into account PDSCH Time Domain Resource Allocation in UE's active DL BWP,
  //   for now only initial DL BWP is considered for simplification in this function.

  cell_slot_resource_allocator& pdcch_alloc = res_grid[pdcch_slot];
  if (pdcch_alloc.result.dl.dl_pdcchs.full()) {
    logger.warning("Skipping Paging opportunity for id={}. Cause: Not enough PDCCH space for Paging",
                   ue_paging_ids.front());
    return false;
  }

  cell_slot_resource_allocator& pdsch_alloc = res_grid[pdcch_slot + pdsch_td_alloc_list[pdsch_time_res].k0];
  if (pdsch_alloc.result.dl.paging_grants.full()) {
    logger.warning("Skipping Paging opportunity for id={}. Cause: Not enough PDSCH space for Paging",
                   ue_paging_ids.front());
    return false;
  }

  static constexpr unsigned nof_layers = 1;
  // As per Section 5.1.3.2, TS 38.214, nof_oh_prb = 0 if PDSCH is scheduled by PDCCH with a CRC scrambled by P-RNTI.
  static constexpr unsigned nof_oh_prb = 0;
  // As per TS 38.214, Table 5.1.3.2-2.
  // TODO: TBS scaling is assumed to be 0. Need to set correct value.
  static constexpr unsigned tbs_scaling = 0;

  const pdsch_time_domain_resource_allocation& pdsch_td_cfg = pdsch_td_alloc_list[pdsch_time_res];
  const unsigned                               nof_symb_sh  = pdsch_td_cfg.symbols.length();

  // Generate dmrs information to be passed to (i) the fnc that computes number of RE used for DMRS per RB and (ii) to
  // the fnc that fills the DCI.
  const dmrs_information dmrs_info =
      make_dmrs_info_common(pdsch_td_alloc_list, pdsch_time_res, cell_cfg.params.pci, cell_cfg.params.dmrs_typeA_pos);

  const sch_mcs_description mcs_descr = pdsch_mcs_get_config(pdsch_mcs_table::qam64, expert_cfg.pg.paging_mcs_index);
  const sch_prbs_tbs        paging_prbs_tbs =
      get_nof_prbs(prbs_calculator_sch_config{get_accumulated_paging_msg_size(ue_paging_ids),
                                              nof_symb_sh,
                                              calculate_nof_dmrs_per_rb(dmrs_info),
                                              nof_oh_prb,
                                              mcs_descr,
                                              nof_layers,
                                              tbs_scaling});

  // > Find available RBs in PDSCH for Paging grant.
  crb_interval paging_crbs;
  {
    const unsigned    nof_paging_rbs = paging_prbs_tbs.nof_prbs;
    const crb_bitmap& used_crbs      = pdsch_alloc.dl_res_grid.used_crbs(bwp_cfg, pdsch_td_cfg.symbols);
    paging_crbs                      = rb_helper::find_empty_interval_of_length(used_crbs, nof_paging_rbs);
    if (paging_crbs.length() < nof_paging_rbs) {
      logger.warning("Skipping Paging opportunity for id={}. Cause: Not enough RBs available for Paging PDSCH in "
                     "slot={} ({} < {})",
                     ue_paging_ids.front(),
                     pdsch_alloc.slot,
                     paging_crbs.length(),
                     nof_paging_rbs);
      return false;
    }
  }

  // > Allocate DCI_1_0 for Paging on PDCCH.
  pdcch_dl_information* pdcch =
      pdcch_sch.alloc_dl_pdcch_common(pdcch_alloc, rnti_t::P_RNTI, ss_id, expert_cfg.pg.paging_dci_aggr_lev);
  if (pdcch == nullptr) {
    logger.warning("Skipping Paging opportunity for id={}. Cause: Could not allocate PDCCH in slot={}",
                   ue_paging_ids.front(),
                   pdsch_alloc.slot);
    return false;
  }

  // > Now that we are sure there is space in both PDCCH and PDSCH, set Paging CRBs as used.
  pdsch_alloc.dl_res_grid.fill(
      grant_info{cell_cfg.params.dl_cfg_common.init_dl_bwp.generic_params.scs, pdsch_td_cfg.symbols, paging_crbs});

  // > Delegate filling Paging grants to helper function.
  fill_paging_grant(pdsch_alloc.result.dl.paging_grants.emplace_back(),
                    *pdcch,
                    paging_crbs,
                    pdsch_time_res,
                    ue_paging_ids,
                    dmrs_info,
                    paging_prbs_tbs.tbs_bytes);

  return true;
}

void paging_scheduler::fill_paging_grant(dl_paging_allocation&            pg_grant,
                                         pdcch_dl_information&            pdcch,
                                         crb_interval                     crbs_grant,
                                         unsigned                         time_resource,
                                         const std::vector<ue_paging_id>& ue_paging_ids,
                                         const dmrs_information&          dmrs_info,
                                         units::bytes                     tbs)
{
  // Fill Paging DCI.
  build_dci_f1_0_p_rnti(
      pdcch.dci, cell_cfg.params.dl_cfg_common.init_dl_bwp, crbs_grant, time_resource, expert_cfg.pg.paging_mcs_index);

  // Add Paging UE info to list of Paging information to pass to lower layers.
  for (ue_paging_id pg_id : ue_paging_ids) {
    const auto& pg_info = paging_pending_ues.at(pg_id);
    pg_grant.paging_ue_list.emplace_back();
    pg_grant.paging_ue_list.back().paging_type_indicator =
        pg_info.request.paging_type_indicator == paging_identity_type::cn_ue_paging_identity
            ? paging_ue_info::cn_ue_paging_identity
            : paging_ue_info::ran_ue_paging_identity;
    pg_grant.paging_ue_list.back().paging_identity = pg_id;
  }

  // Fill PDSCH configuration.
  pdsch_information& pdsch = pg_grant.pdsch_cfg;
  build_pdsch_f1_0_p_rnti(pdsch,
                          cell_cfg,
                          tbs,
                          pdcch.dci.as_p_rnti_f1_0(),
                          crbs_grant,
                          pdsch_td_alloc_list[pdcch.dci.as_p_rnti_f1_0().time_resource].symbols,
                          dmrs_info);
}
