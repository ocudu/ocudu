// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "cell_group_event_manager.h"
#include "../cell/resource_grid.h"
#include "../common_scheduling/ra_ue_repository.h"
#include "../config/sched_config_manager.h"
#include "../srs/srs_scheduler.h"
#include "../uci_scheduling/uci_indication_selector.h"
#include "../uci_scheduling/uci_scheduler_impl.h"
#include "../ue_context/ue_cell_repository.h"
#include "ocudu/scheduler/scheduler_feedback_handler.h"
#include "fmt/chrono.h"
#include <memory>

using namespace ocudu;

cell_group_event_handler_impl::cell_group_event_handler_impl(cell_group_event_manager&  parent_,
                                                             const cell_creation_event& cell_ev,
                                                             ue_repository&             ue_db_,
                                                             ocudulog::basic_logger&    logger_) :
  parent(parent_),
  ue_db(ue_db_),
  logger(logger_),
  cfg(cell_ev.cell_res_grid.cfg),
  fallback_sched(cell_ev.fallback_sched),
  uci_sched(cell_ev.uci_sched),
  slice_sched(cell_ev.slice_sched),
  srs_sched(cell_ev.srs_sched),
  cg_sched(cell_ev.cg_sched),
  ra_ue_repo(cell_ev.ra_ue_repo)
{
}

cell_group_event_handler_impl::~cell_group_event_handler_impl()
{
  // Deregister cell from cell_group_event_manager.
  parent.cells[cfg.cell_index] = nullptr;
}

bool cell_group_event_handler_impl::handle_ue_creation(ue_config_update_event ev)
{
  const du_ue_index_t ue_index = ev.get_ue_index();
  const rnti_t        crnti    = ev.next_config().crnti;
  if (ue_db.contains(ue_index)) {
    logger.error(
        "ue={} rnti={}: Discarding UE creation. Cause: A UE with the same index already exists", ue_index, crnti);
    return false;
  }

  // Check if this UE was created via RACH and is still tracked by the RA scheduler, so its PRACH reception slot
  // can be carried over into the UE's PCell context. A 2-step RACH successRAR completion already resolved
  // contention (TS38.321 6.2.3a), so no MAC ConRes CE is needed.
  auto                      ra_it = ra_ue_repo.find(crnti);
  std::optional<slot_point> prach_slot_rx =
      ra_it != ra_ue_repo.end() ? std::optional<slot_point>(ra_it->prach_slot_rx) : std::nullopt;

  bool             is_in_fallback = ev.get_fallback_command().has_value() and ev.get_fallback_command().value();
  ue_creation_mode creation_mode  = ue_creation_mode::skip_fallback;
  if (is_in_fallback) {
    if (ev.get_ul_ccch_slot_rx().has_value()) {
      // RACH-created UE. A 2-step RACH successRAR completion already resolved contention (TS38.321 6.2.3a), so
      // no MAC ConRes CE is needed; any other RACH path (native 4-step, or 2-step fallback) still needs one.
      creation_mode = ra_it != ra_ue_repo.end() and ra_it->is_msgb_success_rar()
                          ? ue_creation_mode::two_step_success_rar
                          : ue_creation_mode::msg3_rach;
    } else if (ev.get_cfra_enabled()) {
      creation_mode = ue_creation_mode::cfra;
    } else {
      creation_mode = ue_creation_mode::high_layers;
    }
  }

  // Insert UE in UE repository.
  ue_db.add_ue(ev.next_config(), {creation_mode, ev.get_ul_ccch_slot_rx(), prach_slot_rx});

  auto& u     = ue_db[ue_index];
  auto& ue_cc = u.get_pcell();
  if (ue_cc.get_pcell_state().conres_st != ue_conres_state::pending_conres_crnti_ce) {
    // Defer UCI/SR scheduling only for UEs awaiting a C-RNTI MAC CE.
    uci_sched.add_ue(ue_cc.cfg());
    srs_sched.add_ue(ue_cc.cfg());
  }

  // Add UE to slice scheduler.
  // Note: This action only has effect when UE is created in non-fallback mode.
  slice_sched.add_ue(ue_index);

  if (ue_cc.get_pcell_state().conres_st == ue_conres_state::pending_conres_ce) {
    // Note: In case of RACH-created UE, auto-inject MAC ConRes CE.

    // Forward CE to ue instance.
    u.handle_dl_mac_ce_indication(dl_mac_ce_indication{ue_index, lcid_dl_sch_t::UE_CON_RES_ID});

    // Notify fallback scheduler of a pending ConRes CE.
    fallback_sched.handle_conres_indication(ue_index);
  }

  // Notify config manager that creation is complete with success.
  ev.notify_completion();

  return true;
}

bool cell_group_event_handler_impl::handle_ue_reconfiguration(ue_config_update_event ev)
{
  const du_ue_index_t ue_idx = ev.get_ue_index();
  if (not ue_db.contains(ue_idx)) {
    log_invalid_ue_index(ue_idx, "ue_reconf");
    return false;
  }
  auto& u = ue_db[ue_idx];

  // Reconfigure PCell
  // Note: Carrier aggregation not yet supported.
  auto& ue_cc = u.get_cell(SERVING_PCELL_IDX);

  if (ue_cc.get_pcell_state().conres_st != ue_conres_state::pending_conres_crnti_ce) {
    uci_sched.reconf_ue(ev.next_config().ue_cell_cfg(ue_cc.cell_index), ue_cc.cfg());
    srs_sched.reconf_ue(ev.next_config().ue_cell_cfg(ue_cc.cell_index), ue_cc.cfg());
    if (cg_sched != nullptr) {
      cg_sched->add_reconf_ue(ev.next_config().ue_cell_cfg(ue_cc.cell_index), &ue_cc.cfg());
    }
  }

  // Configure existing UE.
  ue_db.reconfigure_ue(ev.next_config(), ev.get_cause());

  // Update slice scheduler.
  slice_sched.reconf_ue(u.ue_index);

  // Notify config manager that reconfiguration is complete with success.
  ev.notify_completion();

  return true;
}

bool cell_group_event_handler_impl::handle_ue_deletion(ue_config_delete_event ev)
{
  const du_ue_index_t ue_idx = ev.ue_index();
  if (not ue_db.contains(ue_idx)) {
    log_invalid_ue_index(ue_idx, "ue_rem");
    return false;
  }
  const auto& u     = ue_db[ue_idx];
  const auto& ue_cc = u.get_pcell();
  if (ue_cc.get_pcell_state().conres_st != ue_conres_state::pending_conres_crnti_ce) {
    // A UE awaiting a C-RNTI CE was not added to UCI/SRS scheduling yet (deferred until the CE is received), so
    // it must not be removed either. All other UEs (including CFRA) were added at creation.
    uci_sched.rem_ue(u.get_pcell().cfg());
    srs_sched.rem_ue(u.get_pcell().cfg());
    if (cg_sched != nullptr) {
      cg_sched->rem_ue(u.get_pcell().cfg());
    }
  }
  // Schedule removal of UE from slice scheduler.
  slice_sched.rem_ue(ue_idx);

  // Schedule UE removal from repository.
  ue_db.schedule_ue_rem(std::move(ev));

  return true;
}

bool cell_group_event_handler_impl::handle_ue_config_applied(du_ue_index_t ue_idx)
{
  // Confirm that UE applied new config.
  ue_db.ue_config_applied(ue_idx);

  // Notify slice scheduler only when the UE fully exits fallback (conres also done).
  if (not ue_db[ue_idx].get_pcell().is_in_fallback_mode()) {
    slice_sched.config_applied(ue_idx);
  }

  return true;
}

bool cell_group_event_handler_impl::handle_ue_deactivation_request(du_ue_index_t ue_idx)
{
  if (not ue_db.contains(ue_idx)) {
    log_invalid_ue_index(ue_idx, "ue_deactivation");
    return false;
  }
  auto& u = ue_db[ue_idx];

  // Deactivate the UE (no more grants after this point).
  u.deactivate();

  // Schedule removal of UE from slice scheduler so it doesn't get scheduled PDSCH/PUSCH.
  slice_sched.rem_ue(ue_idx);

  return true;
}

bool cell_group_event_handler_impl::handle_ul_bsr_indication(const ul_bsr_indication_message& bsr_ind)
{
  if (not ue_db.contains(bsr_ind.ue_index)) {
    log_invalid_ue_index(bsr_ind.ue_index, "BSR");
    return false;
  }
  auto& u = ue_db[bsr_ind.ue_index];

  // Handle event.
  u.handle_bsr_indication(bsr_ind);

  if (u.get_pcell().is_in_fallback_mode()) {
    // Signal SRB fallback scheduler with the new SRB0/SRB1 buffer state.
    fallback_sched.handle_ul_bsr_indication(bsr_ind.ue_index, bsr_ind);
  }

  return true;
}

void cell_group_event_handler_impl::handle_conres_ce_acked(du_ue_index_t ue_index)
{
  if (not ue_db.contains(ue_index)) {
    log_invalid_ue_index(ue_index, "ConRes CE ACKed");
    return;
  }
  ue_db.handle_conres_ce_outcome(ue_index, true);
}

void cell_group_event_handler_impl::handle_sr_detected(du_ue_index_t ue_index, slot_point uci_slot)
{
  // Note: Not warned about when the UE is gone, for the same reason as the UCI indication it comes from.
  if (not ue_db.contains(ue_index)) {
    log_invalid_ue_index(ue_index, "SR", false);
    return;
  }
  ue&            u     = ue_db[ue_index];
  const ue_cell* ue_cc = u.find_cell(cfg.cell_index);
  if (ue_cc == nullptr) {
    log_invalid_cc(ue_index, "SR", false);
    return;
  }

  u.handle_sr_indication(uci_slot);
  if (ue_cc->is_in_fallback_mode()) {
    fallback_sched.handle_sr_indication(ue_index);
  }
}

void cell_group_event_handler_impl::handle_ul_n_ta_update(du_ue_index_t              ue_index,
                                                          time_alignment_group::id_t tag_id,
                                                          phy_time_unit              n_ta_diff,
                                                          float                      ul_sinr)
{
  if (not ue_db.contains(ue_index)) {
    log_invalid_ue_index(ue_index, "N_TA update");
    return;
  }
  ue_db[ue_index].handle_ul_n_ta_update_indication(tag_id, n_ta_diff, ul_sinr);
}

void cell_group_event_handler_impl::handle_cfra_msg3_acked(du_ue_index_t ue_index)
{
  if (not ue_db.contains(ue_index)) {
    log_invalid_ue_index(ue_index, "CFRA Msg3 ACKed");
    return;
  }
  const ue_cell* ue_cc = ue_db[ue_index].find_cell(cfg.cell_index);
  if (ue_cc == nullptr) {
    log_invalid_cc(ue_index, "CFRA Msg3 ACKed");
    return;
  }

  if (ue_db.cfra_msg3_acked(ue_index) and not ue_cc->is_in_fallback_mode()) {
    // CFRA Msg3 ACKed. UE is directly added to slice scheduling. It doesn't need to be in fallback mode.
    slice_sched.config_applied(ue_index);
  }
}

bool cell_group_event_handler_impl::handle_ul_phr_indication(const ul_phr_indication_message& phr_ind)
{
  // Fetch UE objects.
  if (not ue_db.contains(phr_ind.ue_index)) {
    log_invalid_ue_index(phr_ind.ue_index, "PHR");
    return false;
  }
  auto& u = ue_db[phr_ind.ue_index];

  for (const cell_ph_report& cell_phr : phr_ind.phr.get_phr()) {
    ocudu_sanity_check(cell_phr.serv_cell_id < u.nof_cells(), "Invalid serving cell index={}", cell_phr.serv_cell_id);
    auto& ue_cc = u.get_cell(cell_phr.serv_cell_id);

    ue_cc.get_pusch_power_controller().handle_phr(cell_phr, phr_ind.slot_rx, phr_ind.rnti);
  }

  return true;
}

bool cell_group_event_handler_impl::handle_ul_ta_report_indication(const ul_ta_report_indication_message& ta_report)
{
  if (not ue_db.contains(ta_report.ue_index)) {
    log_invalid_ue_index(ta_report.ue_index, "TA report");
    return false;
  }

  ue_db[ta_report.ue_index].ta_report_tracker().handle_ta_report(ta_report.ul_ta);

  return true;
}

bool cell_group_event_handler_impl::handle_dl_mac_ce_indication(const dl_mac_ce_indication& ce)
{
  if (not ue_db.contains(ce.ue_index)) {
    log_invalid_ue_index(ce.ue_index, "DL MAC CE");
    return false;
  }
  // Forward CE to UE instance.
  ue_db[ce.ue_index].handle_dl_mac_ce_indication(ce);

  return true;
}

bool cell_group_event_handler_impl::handle_crnti_ce_received(du_ue_index_t ue_index)
{
  if (not ue_db.contains(ue_index)) {
    log_invalid_ue_index(ue_index, "C-RNTI CE received");
    return false;
  }
  auto& u     = ue_db[ue_index];
  auto& ue_cc = u.get_pcell();
  if (ue_cc.cell_index != cfg.cell_index) {
    logger.warning("cell={} ue={} rnti={}: Discarding C-RNTI CE. It was received in cell that is not PCell ({})",
                   cfg.cell_index,
                   ue_index,
                   u.crnti,
                   ue_cc.cell_index);
    return false;
  }

  if (ue_db.crnti_ce_received(ue_index)) {
    // C-RNTI CE received: contention resolution completed for this F1AP-created UE.

    // Initiate UCI and SRS schedulers with the confirmed UE resources.
    uci_sched.add_ue(ue_cc.cfg());
    srs_sched.add_ue(ue_cc.cfg());

    if (cg_sched != nullptr) {
      cg_sched->add_reconf_ue(ue_cc.cfg(), nullptr);
    }

    // Notify slice scheduler only when the UE fully exits fallback (config also applied).
    if (not ue_cc.is_in_fallback_mode()) {
      slice_sched.config_applied(ue_index);
    }
  }

  return true;
}

bool cell_group_event_handler_impl::handle_dl_buffer_state_indication(const dl_buffer_state_indication_message& dl_bo)
{
  if (not ue_db.contains(dl_bo.ue_index)) {
    log_invalid_ue_index(dl_bo.ue_index, "DL buffer occupancy update");
    return false;
  }
  ue& u = ue_db[dl_bo.ue_index];

  // Forward DL BO update to UE.
  u.handle_dl_buffer_state_indication(dl_bo.lcid, dl_bo.bs, dl_bo.hol_toa);
  if (u.get_pcell().is_in_fallback_mode()) {
    // Signal SRB fallback scheduler with the new SRB0/SRB1 buffer state.
    fallback_sched.handle_dl_buffer_state_indication(dl_bo.ue_index);
  }

  return true;
}

void cell_group_event_handler_impl::handle_slice_reconfiguration(const du_cell_slice_reconfig_request& req)
{
  slice_sched.handle_slice_reconfiguration_request(req);
}

void cell_group_event_handler_impl::log_invalid_ue_index(du_ue_index_t ue_index,
                                                         const char*   event_name,
                                                         bool          warn_if_ignored) const
{
  ocudulog::log_channel& log_channel = warn_if_ignored ? logger.warning : logger.info;
  log_channel("cell={} ue={}: Discarding {} event. Cause: UE with provided Id does not exist",
              cfg.cell_index,
              ue_index,
              event_name);
}

void cell_group_event_handler_impl::log_invalid_cc(du_ue_index_t ue_idx,
                                                   const char*   event_name,
                                                   bool          warn_if_ignored) const
{
  ocudulog::log_channel& log_channel = warn_if_ignored ? logger.warning : logger.info;
  log_channel("cell={} ue={}: Discarding {} event. Cause: UE is not configured in this cell",
              cfg.cell_index,
              ue_idx,
              event_name);
}

cell_group_event_manager::cell_group_event_manager(ue_repository& ue_db_) :
  ue_db(ue_db_), logger(ocudulog::fetch_basic_logger("SCHED"))
{
  std::fill(cells.begin(), cells.end(), nullptr);
}

std::unique_ptr<cell_group_event_handler_impl> cell_group_event_manager::add_cell(const cell_creation_event& cell_ev)
{
  const du_cell_index_t cell_index = cell_ev.cell_res_grid.cell_index();
  ocudu_assert(not cell_exists(cell_index), "Overwriting cell configurations not supported");

  // Create cell_group_event_handler_impl.
  auto cell = std::make_unique<cell_group_event_handler_impl>(*this, cell_ev, ue_db, logger);

  // Register cell.
  cells[cell_index] = cell.get();

  return cell;
}

bool cell_group_event_manager::cell_exists(du_cell_index_t cell_index) const
{
  return cell_index < MAX_NOF_DU_CELLS and cells[cell_index] != nullptr;
}
