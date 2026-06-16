// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ue_repository.h"
#include "../logging/cell_metrics_handler.h"
#include "ocudu/ocudulog/ocudulog.h"
#include "ocudu/scheduler/resource_grid_util.h"

using namespace ocudu;

ue_repository::ue_repository(const scheduler_ue_expert_config& cfg) :
  logger(ocudulog::fetch_basic_logger("SCHED")), ta_mgr_sys(cfg.ta_control)
{
  rnti_to_ue_index_lookup.reserve(MAX_NOF_DU_UES);
}

ue_repository::~ue_repository()
{
  for (auto& u : ues_to_rem) {
    u.second.release();
  }
}

/// \brief This function checks whether it is safe to remove a UE. Currently we verify that the UE has no DL or UL
/// HARQ awaiting an ACK.
static bool is_ue_ready_for_removal(ue& u)
{
  // Ensure that there no currently active HARQs.
  unsigned nof_ue_cells = u.nof_cells();
  for (unsigned cell_idx = 0; cell_idx != nof_ue_cells; ++cell_idx) {
    const ue_cell& c = u.get_cell(static_cast<serv_cell_index_t>(cell_idx));
    for (unsigned i = 0, e = c.harqs.nof_dl_harqs(); i != e; ++i) {
      if (c.harqs.dl_harq(to_harq_id(i)).has_value()) {
        return false;
      }
    }
    for (unsigned i = 0, e = c.harqs.nof_ul_harqs(); i != e; ++i) {
      if (c.harqs.ul_harq(to_harq_id(i)).has_value()) {
        return false;
      }
    }
  }

  return true;
}

void ue_repository::slot_indication(slot_point sl_tx)
{
  last_sl_tx = sl_tx;

  for (std::pair<slot_point, ue_config_delete_event>& p : ues_to_rem) {
    auto& rem_ev = p.second;
    if (not rem_ev.valid()) {
      // Already removed.
      continue;
    }
    if (p.first > sl_tx) {
      // UE is not yet ready to be removed as there may be still pending allocations for it in the resource grid.
      continue;
    }

    const du_ue_index_t ue_idx = rem_ev.ue_index();
    if (not ues.contains(ue_idx)) {
      logger.error("ue={}: Unexpected UE removal from UE repository", fmt::underlying(ue_idx));
      rem_ev.reset();
      continue;
    }
    ue&    u     = ues[ue_idx];
    rnti_t crnti = u.crnti;

    // Check if UEs can be safely removed.
    if (not is_ue_ready_for_removal(u)) {
      continue;
    }

    // Remove UE from the repository.
    rem_ue(u);

    // Marks UE config removal as complete.
    rem_ev.reset();

    logger.debug("ue={} rnti={}: UE has been successfully removed.", fmt::underlying(ue_idx), crnti);
  }

  // In case the elements at the front of the ring has been marked for removal, pop them from the queue.
  while (not ues_to_rem.empty() and not ues_to_rem[0].second.valid()) {
    ues_to_rem.pop();
  }

  // Update state of existing cells.
  for (auto& c : cell_ues) {
    c->slot_indication(sl_tx);
  }

  // Update state of existing UEs.
  lc_ch_sys.slot_indication();
  ta_mgr_sys.slot_indication(sl_tx);
  for (auto& u : ues) {
    u.slot_indication(sl_tx);
  }
}

ue_cell_repository& ue_repository::add_cell(const cell_configuration& cell_cfg, cell_metrics_handler* cell_metrics)
{
  ocudu_sanity_check(
      not cell_ues.contains(cell_cfg.cell_index), "Cell index {} is duplicate", fmt::underlying(cell_cfg.cell_index));
  cell_ues.emplace(cell_cfg.cell_index, std::make_unique<ue_cell_repository>(cell_cfg, cell_metrics));
  return *cell_ues[cell_cfg.cell_index];
}

void ue_repository::rem_cell(du_cell_index_t cell_index)
{
  cell_ues.erase(cell_index);
}

void ue_repository::add_ue(const ue_configuration&   ue_cfg,
                           bool                      starts_in_fallback,
                           std::optional<slot_point> ul_ccch_slot_rx,
                           bool                      cfra_enabled)
{
  ocudu_assert(not ues.contains(ue_cfg.ue_index), "UE with duplicate index being added to the repository");

  // Create UE components.
  const du_ue_index_t ue_index = ue_cfg.ue_index;

  ue_pcell_state st;
  st.msg3_rx_slot = ul_ccch_slot_rx.value_or(slot_point{});
  if (starts_in_fallback) {
    if (st.msg3_rx_slot.valid()) {
      // RACH-created UE: ConRes CE pending + RRC Setup/Reestablishment/Resume pending.
      st.config_st = ue_config_state::pending_initial_conf;
      st.conres_st = ue_conres_state::pending_conres_ce;
    } else if (cfra_enabled) {
      // F1AP-created UE that is expecting a CFRA. Defer UCI/SRS scheduling until Msg3 is ACKed.
      st.conres_st = ue_conres_state::pending_cfra;
    } else {
      // F1AP-created UE: already RRC connected but waiting for C-RNTI CE.
      st.conres_st = ue_conres_state::pending_conres_crnti_ce;
    }
  }
  ue_fsms.emplace(ue_index, st);

  const rnti_t             rnti      = ue_cfg.crnti;
  const auto&              pcell_cmn = ue_cfg.pcell_common_cfg();
  const subcarrier_spacing scs       = pcell_cmn.params.dl_cfg_common.init_dl_bwp.generic_params.scs;
  auto ue_lc_mng = lc_ch_sys.create_ue(ue_index, scs, starts_in_fallback, ue_cfg.logical_channels());
  ue_drx_controllers.emplace(ue_index,
                             pcell_cmn.params.ul_cfg_common.init_ul_bwp.generic_params.scs,
                             pcell_cmn.params.ul_cfg_common.init_ul_bwp.rach_cfg_common->ra_con_res_timer,
                             ue_cfg.drx_cfg(),
                             ue_lc_mng.view(),
                             ul_ccch_slot_rx,
                             logger);
  auto ue_ta_mgr = ta_mgr_sys.add_ue(
      ue_cfg.pcell_cfg().tag_id(), pcell_cmn.params.ul_cfg_common.init_ul_bwp.generic_params.scs, ue_lc_mng.view());

  // Setup UE cells.
  ue_cell_lookups.emplace(ue_index);
  auto& cell_lookup = ue_cell_lookups[ue_index];
  for (unsigned i = 0, sz = ue_cfg.nof_cells(); i != sz; ++i) {
    const auto&           cell_cfg   = ue_cfg.ue_cell_cfg(static_cast<serv_cell_index_t>(i));
    const du_cell_index_t cell_index = cell_cfg.cell_cfg_common.cell_index;
    auto&                 ue_cc      = cell_ues[cell_index]->add_ue(
        ue_cfg, static_cast<serv_cell_index_t>(i), &ue_fsms[ue_index], ue_drx_controllers[ue_index]);
    cell_lookup.du_cells.emplace(cell_index, &ue_cc);
    cell_lookup.ue_cells.push_back(&ue_cc);
  }

  // Add UE in the repository.
  ues.emplace(ue_index, ue_cfg, std::move(ue_lc_mng), ue_drx_controllers[ue_index], std::move(ue_ta_mgr), cell_lookup);

  // Update RNTI -> UE index lookup.
  auto res = rnti_to_ue_index_lookup.insert(std::make_pair(rnti, ue_index));
  ocudu_assert(res.second, "UE with duplicate RNTI being added to the repository");
}

void ue_repository::reconfigure_ue(const ue_configuration& new_cfg, sched_ue_config_request::causes cause)
{
  ocudu_assert(
      ues.contains(new_cfg.ue_index), "ue={} : UE not found in the repository", fmt::underlying(new_cfg.ue_index));
  ocudu_sanity_check(new_cfg.nof_cells() > 0, "Creation of a UE requires at least PCell configuration.");
  auto& u      = ues[new_cfg.ue_index];
  auto& lc_mng = u.logical_channels();

  // UE enters fallback mode when a RRC Reconfiguration takes place.
  if (cause != sched_ue_config_request::causes::not_rrc_proc) {
    update_ue_fsm(new_cfg.ue_index,
                  cause == sched_ue_config_request::causes::rrc_reconf_after_reest
                      ? ue_fsm_config_event::reest_reconf_initiated
                      : ue_fsm_config_event::ue_ctx_setup_received);
  }

  // Configure Logical Channels.
  lc_mng.configure(new_cfg.logical_channels());

  // DRX config.
  if (new_cfg.drx_cfg().has_value()) {
    u.drx_controller().reconfigure(new_cfg.drx_cfg());
  }

  // Update UE cells.
  auto& prev_cell_lookup = ue_cell_lookups[new_cfg.ue_index];
  for (unsigned i = 0, sz = prev_cell_lookup.ue_cells.size(); i != sz; ++i) {
    const ue_cell&        ue_cc      = *prev_cell_lookup.ue_cells[i];
    const du_cell_index_t cell_index = ue_cc.cell_index;
    if (not new_cfg.contains(cell_index) or
        new_cfg.ue_cell_cfg(static_cast<serv_cell_index_t>(i)).cell_cfg_common.cell_index != cell_index) {
      ocudu_assert(i != 0, "PCell removal is not supported in UE reconfiguration.");
      // Cell has been removed from the UE configuration.
      cell_ues[cell_index]->rem_ue(new_cfg.ue_index);
    }
  }
  ue_cell_lookup new_lookup;
  for (unsigned i = 0, sz = new_cfg.nof_cells(); i != sz; ++i) {
    const auto&           ue_cc_cfg  = new_cfg.ue_cell_cfg(static_cast<serv_cell_index_t>(i));
    const du_cell_index_t cell_index = ue_cc_cfg.cell_cfg_common.cell_index;
    if (not prev_cell_lookup.du_cells.contains(cell_index)) {
      // New cell being instantiated.
      auto& ue_cc = cell_ues[cell_index]->add_ue(new_cfg,
                                                 static_cast<serv_cell_index_t>(i),
                                                 i == 0 ? &ue_fsms[new_cfg.ue_index] : nullptr,
                                                 ue_drx_controllers[new_cfg.ue_index]);
      new_lookup.du_cells.emplace(cell_index, &ue_cc);
      new_lookup.ue_cells.push_back(&ue_cc);
    } else {
      // Existing cell being reconfigured.
      ue_cell& ue_cc = *prev_cell_lookup.du_cells[cell_index];
      ue_cc.handle_reconfiguration_request(ue_cc_cfg);
      new_lookup.du_cells.emplace(cell_index, &ue_cc);
      new_lookup.ue_cells.push_back(&ue_cc);
    }
  }
  ue_cell_lookups[new_cfg.ue_index] = std::move(new_lookup);

  // Handle UE reconfiguration.
  u.handle_reconfiguration_request(new_cfg);
}

bool ue_repository::ue_config_applied(du_ue_index_t ue_index)
{
  return update_ue_fsm(ue_index, ue_fsm_config_event::config_applied);
}

bool ue_repository::crnti_ce_received(du_ue_index_t ue_index)
{
  return update_ue_fsm(ue_index, ue_fsm_config_event::crnti_ce_received);
}

bool ue_repository::cfra_msg3_acked(du_ue_index_t ue_index)
{
  return update_ue_fsm(ue_index, ue_fsm_config_event::cfra_msg3_acked);
}

bool ue_repository::handle_conres_ce_outcome(du_ue_index_t ue_index, bool success)
{
  return update_ue_fsm(ue_index,
                       success ? ue_fsm_config_event::conres_ce_acked : ue_fsm_config_event::conres_ce_timeout);
}

void ue_repository::schedule_ue_rem(ue_config_delete_event ev)
{
  if (contains(ev.ue_index())) {
    // Start deactivation of UE bearers.
    auto& u = ues[ev.ue_index()];
    u.deactivate();

    // Register UE for later removal.
    // We define a time window when the UE removal is not allowed, as there are pending CSI/SR PDUs in the resource
    // grid ready to be sent to the PHY. Removing the UE earlier would mean that its PUCCH resources would become
    // available to a newly created UE and there could be a PUCCH collision.
    slot_point rem_slot =
        last_sl_tx + get_max_slot_ul_alloc_delay(u.get_pcell().cfg().cell_cfg_common.ntn_cs_koffset) + 1;
    ues_to_rem.push(std::make_pair(rem_slot, std::move(ev)));
  }
}

bounded_bitset<MAX_NOF_DU_UES> ue_repository::get_ues_with_pending_newtx_data(ran_slice_id_t slice_id, bool is_dl) const
{
  if (is_dl) {
    return lc_ch_sys.get_ues_with_dl_pending_data(slice_id);
  }
  return lc_ch_sys.get_ues_with_ul_pending_data(slice_id);
}

ue* ue_repository::find_by_rnti(rnti_t rnti)
{
  auto it = rnti_to_ue_index_lookup.find(rnti);
  return it != rnti_to_ue_index_lookup.end() ? &ues[it->second] : nullptr;
}

const ue* ue_repository::find_by_rnti(rnti_t rnti) const
{
  auto it = rnti_to_ue_index_lookup.find(rnti);
  return it != rnti_to_ue_index_lookup.end() ? &ues[it->second] : nullptr;
}

void ue_repository::rem_ue(const ue& u)
{
  const rnti_t        crnti  = u.crnti;
  const du_ue_index_t ue_idx = u.ue_index;

  // Remove UE from the cell metrics on its primary cell.
  if (cell_metrics_handler* cell_metrics = cell_ues[u.get_pcell().cell_index]->get_metrics()) {
    cell_metrics->handle_ue_deletion(ue_idx);
  }

  // Remove UE from the cell-specific repositories.
  for (auto& ue_cc : ue_cell_lookups[ue_idx].ue_cells) {
    // Reset HARQ processes.
    ue_cc->harqs.reset();
    // Remove UE carrier.
    cell_ues[ue_cc->cell_index]->rem_ue(ue_idx);
  }
  ue_cell_lookups.erase(ue_idx);

  // Remove UE components.
  ue_drx_controllers.erase(ue_idx);
  ue_fsms.erase(ue_idx);

  // Remove UE from RNTI->UE lookup.
  auto it = rnti_to_ue_index_lookup.find(crnti);
  if (it != rnti_to_ue_index_lookup.end()) {
    rnti_to_ue_index_lookup.erase(it);
  } else {
    logger.error("ue={} rnti={}: UE with provided c-rnti not found in RNTI-to-UE-index lookup table.",
                 fmt::underlying(ue_idx),
                 crnti);
  }

  // Finally, remove UE from the repository.
  ues.erase(ue_idx);
}

void ue_repository::handle_cell_deactivation(du_cell_index_t cell_index)
{
  for (ue& u : ues) {
    ue_cell* ue_cc = u.find_cell(cell_index);
    if (ue_cc == nullptr) {
      // UE does not have this cell, so we can skip it.
      continue;
    }

    // Note: We now remove the UE from the repository, indepedently of whether it is a PCell or SCell. It would be
    // very hard to handle a UE that has a config for a cell that is not active.
    rem_ue(u);
  }

  // We may have removed UEs that were scheduled for removal in an earlier slot. We need to clean up the ues_to_rem.
  for (std::pair<slot_point, ue_config_delete_event>& p : ues_to_rem) {
    auto& rem_ev = p.second;
    if (rem_ev.valid()) {
      const du_ue_index_t ue_idx = rem_ev.ue_index();
      if (not ues.contains(ue_idx)) {
        // UE removed in the previous loop, so we need to clear this event.
        rem_ev.reset();
      }
    }
  }

  // Stop cell activity.
  cell_ues[cell_index]->deactivate();
}

bool ue_repository::update_ue_fsm(du_ue_index_t ue_index, ue_fsm_config_event ev)
{
  using events = ue_fsm_config_event;
  ocudu_assert(ues.contains(ue_index), "ue={} : UE not found in the repository", ue_index);
  auto& fsm    = ue_fsms[ue_index];
  auto& u      = ues[ue_index];
  auto& ue_cc  = u.get_pcell();
  auto& cfg_st = fsm.config_st;
  auto& con_st = fsm.conres_st;

  const bool was_in_fallback = is_in_fallback(cfg_st, con_st);

  switch (ev) {
    case events::conres_ce_acked:
      if (con_st != ue_conres_state::pending_conres_ce) {
        logger.warning("ue={} rnti={}: Invalid event={} in conres_state={}",
                       ue_index,
                       ue_cc.rnti(),
                       to_string(ev),
                       to_string(con_st));
        return false;
      }
      con_st = ue_conres_state::conres_completed;
      logger.debug("ue={} rnti={}: ConRes CE ACKed", ue_index, ue_cc.rnti());
      break;
    case events::conres_ce_timeout:
      if (con_st != ue_conres_state::pending_conres_ce) {
        logger.warning("ue={} rnti={}: Invalid event={} in conres_state={}",
                       ue_index,
                       ue_cc.rnti(),
                       to_string(ev),
                       to_string(con_st));
        return false;
      }
      // Timeout before ConRes CE could be ACKed: deactivate UE.
      con_st = ue_conres_state::conres_completed;
      u.deactivate();
      return true;
    case events::crnti_ce_received:
      if (con_st != ue_conres_state::pending_conres_crnti_ce) {
        // C-RNTI CE not relevant for contention resolution in this context.
        return false;
      }
      con_st = ue_conres_state::conres_completed;
      logger.debug("ue={} rnti={}: C-RNTI CE received, contention resolution completed", ue_index, ue_cc.rnti());
      break;
    case events::cfra_msg3_acked:
      if (con_st != ue_conres_state::pending_cfra) {
        return false;
      }
      con_st = ue_conres_state::conres_completed;
      logger.debug("ue={} rnti={}: CFRA Msg3 ACKed, UCI/SRS scheduling activated", ue_index, ue_cc.rnti());
      break;
    case events::ue_ctx_setup_received:
      if (cfg_st == ue_config_state::pending_reconf) {
        // Already waiting for reconf complete: no-op.
        return false;
      }
      if (cfg_st == ue_config_state::pending_initial_conf) {
        // During RRC Resume, UEContextSetup is triggered before the DL
        return false;
      }
      if (cfg_st != ue_config_state::config_applied) {
        logger.warning("ue={} rnti={}: Invalid event={} in config_state={}",
                       ue_index,
                       ue_cc.rnti(),
                       to_string(ev),
                       to_string(cfg_st));
        return false;
      }
      cfg_st = ue_config_state::pending_reconf;
      if (not was_in_fallback) {
        u.logical_channels().set_fallback_state(true);
        ue_cc.harqs.cancel_retxs();
        logger.debug("ue={} rnti={}: Entering fallback mode", ue_index, ue_cc.rnti());
      }
      return true;
    case events::reest_reconf_initiated:
      if (cfg_st != ue_config_state::pending_initial_conf and cfg_st != ue_config_state::config_applied) {
        logger.warning("ue={} rnti={}: Invalid event={} in config_state={}",
                       ue_index,
                       ue_cc.rnti(),
                       to_string(ev),
                       to_string(cfg_st));
        return false;
      }
      cfg_st = ue_config_state::pending_reest_reconf;
      if (not was_in_fallback) {
        u.logical_channels().set_fallback_state(true);
        ue_cc.harqs.cancel_retxs();
        logger.debug("ue={} rnti={}: Entering fallback mode", ue_index, ue_cc.rnti());
      }
      return true;
    case events::config_applied:
      if (cfg_st == ue_config_state::config_applied) {
        // No pending config: no-op.
        return false;
      }
      cfg_st = ue_config_state::config_applied;
      logger.debug("ue={} rnti={}: Config applied", ue_index, ue_cc.rnti());
      break;
  }

  // Check whether the UE just exited fallback mode (both states at their terminal values).
  if (was_in_fallback and not is_in_fallback(cfg_st, con_st)) {
    ue_cc.harqs.cancel_retxs();
    u.logical_channels().set_fallback_state(false);
    logger.debug("ue={} rnti={}: Leaving fallback mode", ue_index, ue_cc.rnti());
  }

  return true;
}
