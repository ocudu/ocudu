// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "du_setup_procedure.h"
#include "../converters/asn1_sys_info_packer.h"
#include "../converters/f1ap_configuration_helpers.h"
#include "../converters/scheduler_configuration_helpers.h"
#include "../du_cell_manager.h"
#include "../du_manager_context.h"
#include "../ran_resource_management/du_ran_resource_manager.h"
#include "ocudu/mac/config/mac_config_helpers.h"
#include "ocudu/ocudulog/ocudulog.h"
#include "ocudu/scheduler/config/scheduler_cell_config_validator.h"
#include "ocudu/support/async/async_no_op_task.h"
#include "ocudu/support/async/async_timer.h"
#include "ocudu/support/async/coroutine.h"
#include <string>
#include <vector>

using namespace ocudu;
using namespace odu;

/// Derives MAC Cell Configuration from DU Cell Configuration.
static mac_cell_creation_request make_mac_cell_config(du_cell_index_t                                 cell_index,
                                                      const du_cell_config&                           du_cfg,
                                                      const byte_buffer&                              sib1,
                                                      span<const bcch_dl_sch_payload_type>            si_messages,
                                                      const sched_cell_configuration_request_message& sched_cell_cfg,
                                                      unsigned                                        max_nof_setup_ues)
{
  mac_cell_creation_request mac_cfg{};
  mac_cfg.cell_index = cell_index;
  mac_cfg.pci        = du_cfg.ran.pci;
  mac_cfg.scs_common = du_cfg.ran.dl_cfg_common.init_dl_bwp.generic_params.scs;
  mac_cfg.ssb_cfg    = du_cfg.ran.ssb_cfg;
  mac_cfg.dl_carrier = du_cfg.ran.dl_carrier;
  mac_cfg.ul_carrier = du_cfg.ran.ul_carrier;
  const auto cs0_idx = du_cfg.ran.dl_cfg_common.init_dl_bwp.pdcch_common.get_coreset0();
  ocudu_assert(cs0_idx.has_value(), "CORESET#0 index not found in common PDCCH configuration");
  mac_cfg.cs0_index     = cs0_idx.value();
  mac_cfg.ss0_index     = du_cfg.ran.dl_cfg_common.init_dl_bwp.pdcch_common.get_searchspace0().value();
  mac_cfg.sys_info.sib1 = sib1.copy();
  for (auto& msg : si_messages) {
    mac_cfg.sys_info.si_messages.push_back(msg);
  }
  mac_cfg.sys_info.sib1_contains_hypersfn = du_cfg.ran.init_bwp.paging.edrx_enabled;
  mac_cfg.sched_req                       = sched_cell_cfg;
  mac_cfg.cell_barred                     = du_cfg.cell_barred;
  mac_cfg.intra_freq_reselection          = du_cfg.intra_freq_reselection;

  // (Implementation-defined) Number of HARQs needed to account for UEs that the cell cannot support but still require
  // HARQs for sending an RRC Reject. We consider that UEs to be RRC Rejected only need one HARQ.
  static constexpr unsigned harqs_for_rrc_rejects = 64;

  // Dimension the MAC DL HARQ buffer pool based on the number of UEs the cell can actually support (each using the
  // configured number of DL HARQ processes) plus a margin for UEs that only need a single HARQ to be RRC Rejected.
  mac_cfg.max_harq_buffers = du_cfg.ran.init_bwp.pdsch.max_harq_procs * max_nof_setup_ues + harqs_for_rrc_rejects;

  return mac_cfg;
}

static std::string make_sib_mapping_info_str(span<const sib_type> sib_mapping)
{
  std::string out;
  for (sib_type sib : sib_mapping) {
    if (!out.empty()) {
      out += ",";
    }
    out += std::to_string(static_cast<unsigned>(sib));
  }
  return out;
}

static void log_cell_si_messages(ocudulog::log_channel&                    info_logger,
                                 du_cell_index_t                           cell_index,
                                 span<const byte_buffer>                   packed_si_msgs,
                                 const std::vector<si_message_sched_info>* si_sched_info,
                                 span<const std::string>                   si_json_strs)
{
  for (unsigned msg_idx = 0; msg_idx != packed_si_msgs.size(); ++msg_idx) {
    std::string sib_mapping = "unknown";
    if (si_sched_info != nullptr && msg_idx < si_sched_info->size()) {
      sib_mapping = make_sib_mapping_info_str((*si_sched_info)[msg_idx].sib_mapping_info);
    }

    info_logger(packed_si_msgs[msg_idx].begin(),
                packed_si_msgs[msg_idx].end(),
                "SI message #{} cell={}: si_msg_idx={} len={}B sib_mapping=[{}]: {}",
                msg_idx + 1,
                fmt::underlying(cell_index),
                msg_idx,
                packed_si_msgs[msg_idx].length(),
                sib_mapping,
                msg_idx < si_json_strs.size() ? si_json_strs[msg_idx] : "si-json-unavailable");
  }
}

du_setup_procedure::du_setup_procedure(const du_proc_context_view& ctxt_, const du_start_request& request_) :
  ctxt(ctxt_),
  request(request_),
  proc_logger(ctxt.logger, "DU setup"),
  timer(ctxt.params.services.timers.create_unique_timer(ctxt.params.services.du_mng_exec))
{
}

void du_setup_procedure::operator()(coro_context<async_task<void>>& ctx)
{
  CORO_BEGIN(ctx);

  proc_logger.log_proc_started();

  // Establish TNL association with the CU-CP.
  for (; count != request.max_f1c_tnl_connection_retries and not ctxt.ctxt.stop_command_received and
         not ctxt.params.f1ap.conn_mng.connect_to_cu_cp();
       ++count) {
    ctxt.logger.warning("F1-C TNL association with CU-CP attempt {}/{} failed. Retrying in {} ms...",
                        count + 1,
                        request.max_f1c_tnl_connection_retries,
                        request.f1c_tnl_connection_retry_wait.count());
    CORO_AWAIT(async_wait_for(timer, request.f1c_tnl_connection_retry_wait));
  }
  if (count == request.max_f1c_tnl_connection_retries) {
    report_error("F1 Setup failed. Cause: F1-C TNL connection failed");
  }
  if (ctxt.ctxt.stop_command_received) {
    // DU is being shutdown.
    CORO_EARLY_RETURN();
  }

  // Configure cells.
  configure_du_cells();

  // Initiate F1 Setup.
  CORO_AWAIT_VALUE(response_msg, start_f1_setup_request());

  // Handle F1 setup result and activate cells.
  CORO_AWAIT(handle_f1_setup_response(response_msg));

  // Notify successful setup and deliver packed F1 setup PDU bytes via notifier.
  if (ctxt.params.f1ap.f1_setup_complete_notifier != nullptr) {
    ctxt.params.f1ap.f1_setup_complete_notifier->on_f1_setup_complete(
        std::move(response_msg.value().packed_f1_setup_request),
        std::move(response_msg.value().packed_f1_setup_response),
        ctxt.params.ran.gnb_du_id);
  }

  proc_logger.log_proc_completed();

  CORO_RETURN();
}

void du_setup_procedure::configure_du_cells()
{
  if (not request.configure_cells) {
    // No need to reconfigure cells.
    return;
  }
  ctxt.cell_mng.remove_all_cells();

  // Save cell configurations.
  for (const du_cell_config& cell : ctxt.params.ran.cells) {
    ctxt.cell_mng.add_cell(cell);
  }

  // Configure MAC Cells (without activating them).
  for (unsigned idx = 0, e = ctxt.cell_mng.nof_cells(); idx != e; ++idx) {
    du_cell_index_t                 cell_index = to_du_cell_index(idx);
    const du_cell_config&           du_cfg     = ctxt.cell_mng.get_cell_cfg(cell_index);
    const mac_cell_sys_info_config& sys_info   = ctxt.cell_mng.get_sys_info(cell_index);

    auto sched_cfg = make_sched_cell_config_req(cell_index, du_cfg, sys_info.si_sched_cfg.si_sched_cfg);
    error_type<std::string> result =
        config_validators::validate_sched_cell_configuration_request_message(sched_cfg, ctxt.params.mac.sched_cfg);
    if (not result.has_value()) {
      report_error("Invalid cell={} configuration. Cause: {}", fmt::underlying(cell_index), result.error());
    }

    // Forward config to MAC.
    ctxt.params.mac.mgr.get_cell_manager().add_cell(
        make_mac_cell_config(cell_index,
                             du_cfg,
                             sys_info.sib1,
                             sys_info.si_messages,
                             sched_cfg,
                             ctxt.res_mng.get_max_nof_setup_ues(cell_index)));
  }
}

async_task<f1_setup_result> du_setup_procedure::start_f1_setup_request()
{
  // Prepare request to send to F1.
  f1_setup_request_message req = {};

  req.gnb_du_id   = ctxt.params.ran.gnb_du_id;
  req.gnb_du_name = ctxt.params.ran.gnb_du_name;
  req.rrc_version = ctxt.params.ran.rrc_version;

  const bool log_si_info = ctxt.logger.info.enabled();

  req.served_cells.reserve(ctxt.cell_mng.nof_cells());
  for (unsigned i = 0, e = ctxt.cell_mng.nof_cells(); i != e; ++i) {
    du_cell_index_t       cell_index  = to_du_cell_index(i);
    const du_cell_config& du_cell_cfg = ctxt.cell_mng.get_cell_cfg(cell_index);
    if (not du_cell_cfg.enabled) {
      // Served cell is disabled. Do not forward it to the CU-CP.
      continue;
    }
    auto& serv_cell = req.served_cells.emplace_back();

    // Fill serving cell info.
    serv_cell.cell_index = cell_index;
    serv_cell.cell_info  = make_f1ap_du_cell_info(du_cell_cfg);
    for (const auto& slice : du_cell_cfg.rrm_policy_members) {
      serv_cell.slices.push_back(slice.rrc_member.s_nssai);
    }

    // Pack RRC ASN.1 Serving Cell system info.
    std::string              js_str;
    std::vector<std::string> si_json_strs;
    serv_cell.du_sys_info =
        make_f1ap_du_sys_info(du_cell_cfg, log_si_info ? &js_str : nullptr, log_si_info ? &si_json_strs : nullptr);

    const auto* si_sched_info =
        du_cell_cfg.si.si_config.has_value() ? &du_cell_cfg.si.si_config->si_sched_info : nullptr;

    // Log RRC ASN.1 SIB1 json.
    if (log_si_info) {
      ctxt.logger.info(serv_cell.du_sys_info.packed_sib1.begin(),
                       serv_cell.du_sys_info.packed_sib1.end(),
                       "SIB1 cell={}: {}",
                       fmt::underlying(to_du_cell_index(i)),
                       js_str);

      log_cell_si_messages(
          ctxt.logger.info, cell_index, serv_cell.du_sys_info.packed_si_msgs, si_sched_info, si_json_strs);
    }
  }

  // Initiate F1 Setup Request.
  return ctxt.params.f1ap.conn_mng.handle_f1_setup_request(req);
}

async_task<void> du_setup_procedure::handle_f1_setup_response(const f1_setup_result& resp)
{
  if (not resp.has_value()) {
    switch (resp.error().result) {
      case f1_setup_failure::result_code::f1_setup_failure:
        failure_cause = "CU-CP responded with \"F1 Setup Failure\"";
        if (resp.error().f1_setup_failure_cause != "unspecified") {
          failure_cause += fmt::format(" with F1AP cause \"{}\"", resp.error().f1_setup_failure_cause);
        }
        break;
      case f1_setup_failure::result_code::invalid_response:
        failure_cause = "CU-CP response to F1 Setup Request is invalid";
        break;
      case f1_setup_failure::result_code::timeout:
        failure_cause = "CU-CP did not respond to F1 Setup Request";
        break;
      case f1_setup_failure::result_code::proc_failure:
        failure_cause = "DU failed to run F1 Setup Procedure";
      default:
        report_fatal_error("Invalid F1 Setup Response");
    }

    // Trigger SCTP association shutdown after failed F1 Setup procedure.
    ctxt.logger.error("F1 Setup procedure failed, triggering SCTP association shutdown. Cause: {}.", failure_cause);
    return ctxt.params.f1ap.conn_mng.disconnect_from_cu_cp();
  }

  // Success case.

  // Activate respective cells.
  return launch_async(
      [this, cells_to_activ = resp.value().cells_to_activate, i = 0U](coro_context<async_task<void>>& ctx) mutable {
        CORO_BEGIN(ctx);

        for (; i != cells_to_activ.size(); ++i) {
          // Start MAC cell.
          CORO_AWAIT(ctxt.cell_mng.start(ctxt.cell_mng.get_cell_index(cells_to_activ[i].cgi)));

          // Add cell to metrics.
          ctxt.metrics.handle_cell_start(ctxt.cell_mng.get_cell_index(cells_to_activ[i].cgi));
        }

        CORO_RETURN();
      });
}
