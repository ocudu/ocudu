// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../../ue_manager/cu_cp_ue_impl.h"
#include "ocudu/e1ap/cu_cp/e1ap_cu_cp.h"
#include "ocudu/e1ap/cu_cp/e1ap_cu_cp_bearer_context_update.h"
#include "ocudu/f1ap/cu_cp/f1ap_cu.h"
#include "ocudu/ngap/ngap.h"
#include "ocudu/support/async/async_task.h"
#include "ocudu/xnap/xnap.h"
#include <chrono>
#include <vector>

namespace ocudu::ocucp {

/// Routine for the target gNB execution phase of the handover,
/// as specified in TS 23.502, section 4.9.1.3.3.
class inter_cu_handover_execution_target_routine
{
public:
  inter_cu_handover_execution_target_routine(
      cu_cp_ue*                                                    ue_,
      const std::optional<xnap_handover_target_execution_context>& xnap_ho_target_execution_ctxt_,
      e1ap_bearer_context_manager&                                 e1ap_,
      ngap_interface&                                              ngap_,
      xnap_interface*                                              xnap_,
      f1ap_ue_context_manager&                                     f1ap_,
      ocudulog::basic_logger&                                      logger_);

  void operator()(coro_context<async_task<void>>& ctx);

  static const char* name() { return "Inter CU Handover Execution Target Routine"; }

private:
  void fill_e1ap_bearer_context_modification_request(cu_cp_ue_index_t ue_index);
  void fill_e1ap_bearer_context_tunnel_update_request(const cu_cp_path_switch_request_ack& ack);
  std::vector<async_task<bool>> build_parallel_wait_tasks();
  bool                          initialize_reconfiguration_timeout();
  static cu_cp_path_switch_request
       fill_path_switch_request(const xnap_handover_target_execution_context& target_execution_ctxt,
                                const rrc_cell_context&                       cell_context,
                                const plmn_identity&                          selected_plmn,
                                const security::security_context&             security_context);
  bool is_xn_handover() const { return xnap_ho_target_execution_ctxt.has_value(); }

  // (sub-)routine requests
  e1ap_bearer_context_modification_request bearer_context_modification_request;
  e1ap_bearer_context_modification_request tunnel_context_modification_request;
  cu_cp_path_switch_request                path_switch_request;
  cu_cp_ue_context_release_request         ue_context_release_request;
  f1ap_ue_context_modification_request     ue_context_mod_request;

  // (sub-)routine results
  expected<cu_cp_status_transfer>           sn_status;
  std::vector<bool>                         completed_events;
  e1ap_bearer_context_modification_response bearer_context_modification_response;
  cu_cp_path_switch_response                path_switch_response;

  cu_cp_ue*                                                   ue = nullptr;
  const std::optional<xnap_handover_target_execution_context> xnap_ho_target_execution_ctxt;
  e1ap_bearer_context_manager&                                e1ap;
  ngap_interface&                                             ngap;
  xnap_interface*                                             xnap = nullptr;
  f1ap_ue_context_manager&                                    f1ap;
  ocudulog::basic_logger&                                     logger;

  std::chrono::milliseconds reconf_timeout;
  bool                      reconf_result = false;
};

} // namespace ocudu::ocucp
