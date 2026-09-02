// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "du_mac_ntn_param_update_procedure.h"
#include "../converters/f1ap_configuration_helpers.h"
#include "../du_cell_manager.h"
#include "ocudu/adt/format.h"
#include "ocudu/du/du_high/du_manager/du_manager_params.h"
#include "ocudu/ocudulog/ocudulog.h"
#include "ocudu/ran/subcarrier_spacing.h"
#include "ocudu/support/async/async_no_op_task.h"
#include <chrono>

using namespace ocudu;
using namespace odu;

static error_type<std::string> validate_ntn_param_update_request(const du_ntn_param_update_request& req)
{
  if (req.cells.empty()) {
    return make_unexpected(std::string("At least one cell NTN update must be present"));
  }
  for (const auto& cell : req.cells) {
    if (cell.si_messages.empty()) {
      return make_unexpected(std::string("No SI PDUs provided for cell"));
    }
    if (cell.si_messages.size() > 1 and not cell.si_slot_period.has_value()) {
      return make_unexpected(std::string("Nof PDUs > 1, but si_slot_period not set"));
    }
  }
  return {};
}

async_task<du_ntn_param_update_response> ocudu::odu::start_du_ntn_param_update(const du_ntn_param_update_request& req,
                                                                               const du_manager_params& params,
                                                                               du_cell_manager&         cell_mng)
{
  return launch_async<du_mac_ntn_param_update_procedure>(req, params, cell_mng);
}

du_mac_ntn_param_update_procedure::du_mac_ntn_param_update_procedure(const du_ntn_param_update_request& sib_update_,
                                                                     const du_manager_params&           du_params_,
                                                                     du_cell_manager&                   cell_mng_) :
  request(sib_update_),
  du_params(du_params_),
  cell_mng(cell_mng_),
  req_validation_outcome(validate_ntn_param_update_request(request)),
  logger(ocudulog::fetch_basic_logger("DU-MNG"))
{
}

bool du_mac_ntn_param_update_procedure::update_ntn_assistance_info(
    du_cell_index_t                         cell_idx,
    const du_cell_ntn_param_update_request& cell_req) const
{
  if (not cell_req.ntn_assistance_info.has_value()) {
    return true;
  }

  du_cell_config& cell_cfg = cell_mng.get_cell_cfg(cell_idx);

  if (not cell_cfg.ran.ntn_params.has_value()) {
    logger.warning("Cell {} has no NTN params configured, skipping NTN assistance info update", cell_idx);
    return false;
  }

  cell_cfg.ran.ntn_params->ntn_cfg.ephemeris_info           = cell_req.ntn_assistance_info->ephemeris_info;
  cell_cfg.ran.ntn_params->ntn_cfg.ta_info                  = cell_req.ntn_assistance_info->ta_info;
  cell_cfg.ran.ntn_params->ntn_cfg.epoch_time               = cell_req.ntn_assistance_info->epoch_time;
  cell_cfg.ran.ntn_params->ntn_cfg.ntn_ul_sync_validity_dur = cell_req.ntn_assistance_info->ntn_ul_sync_validity_dur;
  cell_cfg.ran.ntn_params->ref_location_ul_ta               = cell_req.ntn_assistance_info->ref_location_ul_ta;

  return true;
}

void du_mac_ntn_param_update_procedure::operator()(coro_context<async_task<du_ntn_param_update_response>>& ctx)
{
  CORO_BEGIN(ctx);

  if (not req_validation_outcome.has_value()) {
    logger.warning("Invalid NTN parameter update request. Cause: {}", req_validation_outcome.error());
    resp.success = false;
    CORO_EARLY_RETURN(resp);
  }

  for (; next_cell_idx != request.cells.size(); ++next_cell_idx) {
    current_cell_req = &request.cells[next_cell_idx];

    cell_index = cell_mng.get_cell_index(current_cell_req->nr_cgi);
    if (cell_index == INVALID_DU_CELL_INDEX) {
      logger.warning("Skipping cell {} NTN update. Cause: Cell not found.", current_cell_req->nr_cgi.nci);
      continue;
    }

    CORO_AWAIT_VALUE(mac_cell_reconfig_response macresp, handle_cell_update(*current_cell_req, cell_index));
    if (not macresp.si_pdus_enqueued) {
      logger.warning("Failed to update NTN params for cell {}", current_cell_req->nr_cgi.nci);
      resp.success = false;
      CORO_EARLY_RETURN(resp);
    }
  }

  // Notify CU via F1AP if any cell requires CU notification.
  gnbdu_config_update_response f1resp;
  CORO_AWAIT_VALUE(f1resp, handle_f1_gnbdu_config_update());
  if (not f1resp.result) {
    logger.warning("F1AP gNB-DU Configuration Update failed");
    resp.success = false;
    CORO_EARLY_RETURN(resp);
  }

  resp.success = true;
  CORO_RETURN(resp);
}

async_task<mac_cell_reconfig_response>
du_mac_ntn_param_update_procedure::handle_cell_update(const du_cell_ntn_param_update_request& cell_req,
                                                      du_cell_index_t                         cell_idx)
{
  const du_cell_config&                          cell_cfg   = cell_mng.get_cell_cfg(cell_idx);
  const std::optional<std::chrono::microseconds> prev_ul_ta = get_ref_location_ul_ta(cell_cfg.ran.ntn_params);

  // Update NTN assistance info to be used during HO (inside RRC reconfiguration msg).
  update_ntn_assistance_info(cell_idx, cell_req);

  // Build MAC reconfiguration request.
  mac_cell_reconfig_request req;

  // The scheduler places the window at slot granularity, so it only needs to hear about T_TA crossing a slot boundary.
  // Compare the quantised values, not their difference: T_TA drifts a few microseconds per update, so a difference
  // would never reach a slot and the first value would stick forever.
  const std::optional<std::chrono::microseconds> new_ul_ta = get_ref_location_ul_ta(cell_cfg.ran.ntn_params);
  if (new_ul_ta.has_value()) {
    const int64_t slot_dur_us =
        1000 / get_nof_slots_per_subframe(cell_cfg.ran.dl_cfg_common.init_dl_bwp.generic_params.scs);
    if (not prev_ul_ta.has_value() or (new_ul_ta->count() / slot_dur_us) != (prev_ul_ta->count() / slot_dur_us)) {
      req.ntn_ul_ta_update.emplace(sched_cell_ntn_ul_ta_update{cell_idx, *new_ul_ta});
    }
  }

  // If SIB19 source is provided, bump SI version (valuetag change).
  if (cell_req.sib19.has_value()) {
    logger.debug("NTN update for cell {} with valuetag change (SIB19 source provided)", cell_idx);

    // Build cell reconfig request.
    du_cell_param_config_request reconf_req;
    reconf_req.nr_cgi       = cell_req.nr_cgi;
    reconf_req.new_sys_info = *cell_req.sib19;

    auto result = cell_mng.handle_cell_reconf_request(reconf_req);
    if (not result.has_value()) {
      logger.warning("Failed to reconf cell {} for NTN update", cell_req.nr_cgi.nci);
      return launch_no_op_task(mac_cell_reconfig_response{});
    }

    logger.debug("SI version bumped and SIB19 source stored for cell {}", cell_req.nr_cgi.nci);

    // Add SI version update (SIB1 valuetag) to MAC request.
    if (result.value().sched_notif_required) {
      req.new_sys_info = cell_mng.get_sys_info(cell_idx);
    }

    // Track this cell for CU notification.
    if (result.value().cu_notif_required) {
      cells_requiring_cu_notif.push_back(cell_idx);
    }
  }

  // Add SIB19 content update.
  req.new_si_pdu_info = {.sib_idx        = sib_type::sib19,
                         .slot           = cell_req.slot,
                         .si_slot_period = cell_req.si_slot_period,
                         .si_messages    = cell_req.si_messages};

  return du_params.mac.mgr.get_cell_manager().get_cell_controller(cell_idx).reconfigure(req);
}

async_task<gnbdu_config_update_response> du_mac_ntn_param_update_procedure::handle_f1_gnbdu_config_update()
{
  if (cells_requiring_cu_notif.empty()) {
    // No CU notification needed.
    return launch_no_op_task(gnbdu_config_update_response{true});
  }

  // Build F1AP gNB-DU Configuration Update request.
  gnbdu_config_update_request f1_req{};
  f1_req.cells_to_mod.reserve(cells_requiring_cu_notif.size());

  for (du_cell_index_t cell_idx : cells_requiring_cu_notif) {
    const du_cell_config& cell_cfg = cell_mng.get_cell_cfg(cell_idx);
    auto&                 cell     = f1_req.cells_to_mod.emplace_back();
    cell.cell_info                 = make_f1ap_du_cell_info(cell_cfg);
    cell.du_sys_info               = make_f1ap_du_sys_info(cell_cfg);
  }

  logger.debug("Sending F1AP gNB-DU Configuration Update for {} cell(s)", f1_req.cells_to_mod.size());
  return du_params.f1ap.conn_mng.handle_du_config_update(f1_req);
}
