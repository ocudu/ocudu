// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../cu_cp_impl_interface.h"
#include "ocudu/e1ap/cu_cp/e1ap_cu_cp.h"
#include "ocudu/f1ap/cu_cp/f1ap_cu.h"
#include "ocudu/ngap/ngap_rrc_inactive_transition.h"
#include "ocudu/rrc/rrc_du.h"
#include "ocudu/rrc/rrc_ue.h"
#include "ocudu/support/async/async_task.h"

namespace ocudu::ocucp {

class ue_suspend_routine
{
public:
  ue_suspend_routine(cu_cp_ue_index_t                       ue_index_,
                     const rrc_ue_release_context&          release_context_,
                     e1ap_bearer_context_manager&           e1ap_bearer_ctxt_mng_,
                     f1ap_ue_context_manager&               f1ap_ue_ctxt_mng_,
                     cu_cp_ue_context_release_handler&      ue_context_release_handler_,
                     rrc_du_connection_event_handler&       rrc_du_metrics_handler_,
                     ngap_control_message_handler&          ng_control_handler_,
                     cu_cp_ue_context_manipulation_handler& ue_context_handler_,
                     ocudulog::basic_logger&                logger_);

  void operator()(coro_context<async_task<void>>& ctx);

  static const char* name() { return "RRC Inactive Routine"; }

private:
  const cu_cp_ue_index_t       ue_index;
  const rrc_ue_release_context release_context;

  e1ap_bearer_context_manager&           e1ap_bearer_ctxt_mng;       // to trigger bearer context modification at CU-UP
  f1ap_ue_context_manager&               f1ap_ue_ctxt_mng;           // to trigger UE context release at DU
  cu_cp_ue_context_release_handler&      ue_context_release_handler; // to release UE contexts
  rrc_du_connection_event_handler&       rrc_du_metrics_handler;     // to notify DU RRC about RRC Inactive transition
  ngap_control_message_handler&          ng_control_handler;         // to notify AMF about RRC Inactive transition
  cu_cp_ue_context_manipulation_handler& ue_context_handler;         // to start RNA update timer
  ocudulog::basic_logger&                logger;

  // (sub-)routine requests.
  e1ap_bearer_context_modification_request bearer_context_modification_request;
  f1ap_ue_context_release_command          du_ue_context_release_command;
  cu_cp_ue_context_release_command         ue_context_release_command;
  ngap_rrc_inactive_transition_report      rrc_inactive_transition_report;

  // (sub-)routine results.
  e1ap_bearer_context_modification_response bearer_context_modification_response;
  cu_cp_ue_index_t                          released_ue_index;
  bool                                      ngap_rrc_inactive_report_sent = false;
};

} // namespace ocudu::ocucp
