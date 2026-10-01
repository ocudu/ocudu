// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "du_param_config_procedure.h"
#include "../converters/f1ap_configuration_helpers.h"
#include "ocudu/adt/format.h"
#include "ocudu/ocudulog/ocudulog.h"
#include "ocudu/support/async/async_no_op_task.h"

using namespace ocudu;
using namespace odu;

/// Validates that a SIB update, if present, does not exceed the ASN.1 cardinality limits of its lists, so that
/// invalid content is rejected here rather than being committed to the live cell config and only failing later
/// during ASN.1 encoding.
static error_type<std::string> validate_new_sys_info(const sib_info& new_sys_info)
{
  if (const auto* sib3 = std::get_if<sib3_info>(&new_sys_info)) {
    if (sib3->intra_freq_neigh_cell_list.size() > MAX_NOF_SIB3_INTRA_FREQ_CELLS) {
      return make_unexpected(
          fmt::format("SIB3 'intra_freq_neigh_cell_list' contains {} entries, exceeding the maximum of {}",
                      sib3->intra_freq_neigh_cell_list.size(),
                      MAX_NOF_SIB3_INTRA_FREQ_CELLS));
    }
    if (sib3->intra_freq_excluded_cell_list.size() > MAX_NOF_SIB3_INTRA_FREQ_CELLS) {
      return make_unexpected(
          fmt::format("SIB3 'intra_freq_excluded_cell_list' contains {} entries, exceeding the maximum of {}",
                      sib3->intra_freq_excluded_cell_list.size(),
                      MAX_NOF_SIB3_INTRA_FREQ_CELLS));
    }
  } else if (const auto* sib4 = std::get_if<sib4_info>(&new_sys_info)) {
    if (sib4->inter_freq_carrier_freq_list.size() > MAX_NOF_SIB4_INTER_FREQ_CARRIERS) {
      return make_unexpected(
          fmt::format("SIB4 'inter_freq_carrier_freq_list' contains {} entries, exceeding the maximum of {}",
                      sib4->inter_freq_carrier_freq_list.size(),
                      MAX_NOF_SIB4_INTER_FREQ_CARRIERS));
    }
  }
  return {};
}

static error_type<std::string> validate_du_param_config_request(const du_param_config_request& req)
{
  // There must be at least a cell config request.
  if (req.cells.empty()) {
    return make_unexpected("At least one cell configuration must be present");
  }

  for (const auto& cell : req.cells) {
    if (not cell.ssb_pwr_mod.has_value() and cell.rrm_policy_ratio_list.empty() and not cell.new_sys_info.has_value()) {
      return make_unexpected(
          "Cell config requests must specify either SSB power mod or rrm_policy_ratio_list or new_sys_info.");
    }
    if (cell.new_sys_info.has_value()) {
      if (auto outcome = validate_new_sys_info(cell.new_sys_info.value()); not outcome.has_value()) {
        return make_unexpected(outcome.error());
      }
    }
  }

  if (req.cells.size() == 1) {
    if (not req.cells.front().nr_cgi.has_value()) {
      // When the NR-CGI is not specified, then the request cannot be for SSB pwr modification.
      if (req.cells.front().ssb_pwr_mod.has_value()) {
        return make_unexpected("SSB power modification requires NR-CGI");
      }
    }
  }
  // If there are config requests for 2 or more cells, then each of them must specify the NR-CGI.
  else {
    for (const auto& cell : req.cells) {
      if (not cell.nr_cgi.has_value()) {
        return make_unexpected("Missing NR-CGI for one of the cell config requests");
      }
    }
  }

  return {};
}

du_param_config_procedure::du_param_config_procedure(const du_param_config_request& request_,
                                                     const du_manager_params&       du_params_,
                                                     du_cell_manager&               du_cells_) :
  request(request_), du_params(du_params_), du_cells(du_cells_), logger(ocudulog::fetch_basic_logger("DU-MNG"))
{
}

void du_param_config_procedure::operator()(coro_context<async_task<du_param_config_response>>& ctx)
{
  CORO_BEGIN(ctx);

  // Verify that the request for configuration is valid.
  if (auto validation_outcome = validate_du_param_config_request(request); not validation_outcome.has_value()) {
    logger.warning("Invalid DU parameters config request. Cause: {}", validation_outcome.error());
    resp.success = false;
    CORO_EARLY_RETURN(resp);
  }

  // Update DU cell configs.
  if (not handle_cell_config_updates()) {
    resp.success = false;
    CORO_EARLY_RETURN(resp);
  }

  for (; next_cell_idx != changed_cells.size(); ++next_cell_idx) {
    // Reconfigure cell in the MAC.
    // TODO: Use when_all.
    CORO_AWAIT_VALUE(mac_cell_reconfig_response macresp, handle_mac_cell_update(changed_cells[next_cell_idx]));
    if (changed_cells[next_cell_idx].sched_notif_required and not macresp.si_updated) {
      resp.success = false;
      CORO_EARLY_RETURN(resp);
    }
  }

  // Initiate F1AP gNB-DU Configuration Update.
  CORO_AWAIT_VALUE(gnbdu_config_update_response f1resp, handle_f1_gnbdu_config_update());

  resp.success = f1resp.result;
  if (resp.success) {
    logger.info("DU reconfiguration procedure for {} completed",
                request.cells.front().ssb_pwr_mod.has_value() ? "SSB power modification" : "RRM policy update");
  }
  CORO_RETURN(resp);
}

bool du_param_config_procedure::handle_cell_config_updates()
{
  // When the request has no NR-CGI set, then it is for the RRC policy update to be propagated to all cells.
  // NOTE: the request has been validated by one of the callers.
  const bool broadcast_change_to_all_cells = not request.cells.front().nr_cgi.has_value();
  if (broadcast_change_to_all_cells) {
    for (unsigned cell_idx = MIN_DU_CELL_INDEX; cell_idx != du_cells.nof_cells(); ++cell_idx) {
      if (const auto cell_index = to_du_cell_index(cell_idx); du_cells.is_cell_active(cell_index)) {
        const nr_cell_global_id_t nr_cgi = du_cells.get_cell_cfg(cell_index).nr_cgi;
        auto                      result = du_cells.handle_cell_reconf_request(
            du_cell_param_config_request(nr_cgi, std::nullopt, request.cells.front().rrm_policy_ratio_list));
        if (not result.has_value()) {
          continue;
        }
        // Mark that cell was changed.
        changed_cells.push_back(result.value());
      }
    }
  } else {
    for (const du_cell_param_config_request& cell_to_update : request.cells) {
      auto result = du_cells.handle_cell_reconf_request(cell_to_update);
      if (not result.has_value()) {
        continue;
      }

      // Mark that cell was changed.
      changed_cells.push_back(result.value());
    }
  }

  return not changed_cells.empty();
}

async_task<gnbdu_config_update_response> du_param_config_procedure::handle_f1_gnbdu_config_update()
{
  gnbdu_config_update_request f1_req{};

  f1_req.cells_to_mod.reserve(changed_cells.size());
  for (const auto& changed_cell : changed_cells) {
    if (changed_cell.cu_notif_required) {
      const du_cell_config& cell_cfg = du_cells.get_cell_cfg(changed_cell.cell_index);
      auto&                 cell     = f1_req.cells_to_mod.emplace_back();
      cell.cell_info                 = make_f1ap_du_cell_info(cell_cfg);
      cell.du_sys_info               = make_f1ap_du_sys_info(cell_cfg);
    }
  }

  if (f1_req.cells_to_mod.empty()) {
    return launch_no_op_task(gnbdu_config_update_response{true});
  }

  return du_params.f1ap.conn_mng.handle_du_config_update(f1_req);
}

async_task<mac_cell_reconfig_response>
du_param_config_procedure::handle_mac_cell_update(const du_cell_reconfig_result& changed_cell) const
{
  if (not changed_cell.sched_notif_required and not changed_cell.slice_reconf_req.has_value()) {
    return launch_no_op_task(mac_cell_reconfig_response{});
  }

  mac_cell_reconfig_request req;

  if (changed_cell.sched_notif_required) {
    // Update SIB1 and SI message content.
    req.new_sys_info = du_cells.get_sys_info(changed_cell.cell_index);
  }

  if (changed_cell.slice_reconf_req.has_value()) {
    req.slice_reconf_req.emplace(changed_cell.slice_reconf_req.value());
  }

  return du_params.mac.mgr.get_cell_manager().get_cell_controller(changed_cell.cell_index).reconfigure(req);
}
