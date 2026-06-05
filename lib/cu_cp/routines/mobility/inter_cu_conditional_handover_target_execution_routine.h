// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../../cu_cp_impl_interface.h"
#include "../../mobility_manager/mobility_manager_impl.h"
#include "../../ue_manager/cu_cp_ue_impl.h"
#include "ocudu/e1ap/cu_cp/e1ap_cu_cp.h"
#include "ocudu/e1ap/cu_cp/e1ap_cu_cp_bearer_context_update.h"
#include "ocudu/f1ap/cu_cp/f1ap_cu.h"
#include "ocudu/f1ap/cu_cp/f1ap_cu_ue_context_update.h"
#include "ocudu/ngap/ngap.h"
#include "ocudu/support/async/async_task.h"
#include "ocudu/xnap/xnap.h"
#include "ocudu/xnap/xnap_handover.h"
#include <chrono>

namespace ocudu::ocucp {

/// \brief Target-side CHO execution routine (TS 38.423 section 8.2.4).
///
/// Triggered after CHO target preparation completes (HO Request Ack sent to source).
/// Awaits UE arrival (RRCReconfigComplete), sends HandoverSuccess to source,
/// awaits SN Status Transfer, applies PDCP state to CU-UP, performs Path Switch,
/// and sends UE Context Release to source.
class inter_cu_conditional_handover_target_execution_routine
{
public:
  inter_cu_conditional_handover_target_execution_routine(cu_cp_ue*                                     ue_,
                                                         const xnap_handover_target_execution_context& execution_ctxt_,
                                                         e1ap_bearer_context_manager&                  e1ap_,
                                                         ngap_interface&                               ngap_,
                                                         xnap_interface*                               xnap_,
                                                         f1ap_ue_context_manager&                      f1ap_,
                                                         cu_cp_ue_context_release_handler& ue_ctx_release_handler_,
                                                         mobility_manager&                 mobility_mng_,
                                                         ocudulog::basic_logger&           logger_);

  void operator()(coro_context<async_task<void>>& ctx);

  static const char* name() { return "Inter-CU CHO Target Execution Routine"; }

private:
  bool                      fill_e1ap_bearer_context_modification_request();
  void                      fill_e1ap_bearer_context_tunnel_update_request(const cu_cp_path_switch_request_ack& ack);
  bool                      initialize_reconfiguration_timeout();
  cu_cp_path_switch_request fill_path_switch_request();

  cu_cp_ue*                                    ue;
  const xnap_handover_target_execution_context execution_ctxt;
  e1ap_bearer_context_manager&                 e1ap;
  ngap_interface&                              ngap;
  xnap_interface*                              xnap;
  f1ap_ue_context_manager&                     f1ap;
  cu_cp_ue_context_release_handler&            ue_ctx_release_handler;
  mobility_manager&                            mobility_mng;
  ocudulog::basic_logger&                      logger;

  // (sub-)routine results
  expected<cu_cp_status_transfer>           sn_status;
  e1ap_bearer_context_modification_request  bearer_context_modification_request;
  e1ap_bearer_context_modification_response bearer_context_modification_response;
  e1ap_bearer_context_modification_request  tunnel_context_modification_request;
  f1ap_ue_context_modification_request      ue_context_mod_request;
  cu_cp_ue_context_release_request          ue_context_release_request;
  cu_cp_path_switch_request                 path_switch_req;
  cu_cp_path_switch_response                path_switch_response;
  cu_cp_ue_context_release_command          release_cmd;
  cu_cp_ue_context_release_complete         release_complete;

  std::chrono::milliseconds reconf_timeout{};
  bool                      reconf_result = false;
};

} // namespace ocudu::ocucp
