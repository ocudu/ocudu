// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../cu_cp_impl_interface.h"
#include "../du_processor/du_processor.h"
#include "../up_resource_manager/up_resource_manager_impl.h"
#include "ocudu/cu_cp/ue_task_scheduler.h"
#include "ocudu/e1ap/cu_cp/e1ap_cu_cp.h"
#include "ocudu/support/async/async_task.h"

namespace ocudu {
namespace ocucp {

/// \brief Handles the setup of PDU session resources from the RRC viewpoint.
/// TODO Add seqdiag
class pdu_session_resource_release_routine
{
public:
  pdu_session_resource_release_routine(const ngap_pdu_session_resource_release_command& release_cmd_,
                                       e1ap_bearer_context_manager&                     e1ap_bearer_ctxt_mng_,
                                       f1ap_ue_context_manager&                         f1ap_ue_ctxt_mng_,
                                       rrc_ue_interface*                                rrc_ue_,
                                       cu_cp_rrc_ue_interface&                          cu_cp_notifier_,
                                       ue_task_scheduler&                               task_sched_,
                                       up_resource_manager&                             up_resource_mng_,
                                       ocudulog::basic_logger&                          logger_);

  void operator()(coro_context<async_task<ngap_pdu_session_resource_release_response>>& ctx);

  static const char* name() { return "PDU Session Resource Release Routine"; }

private:
  void fill_e1ap_bearer_context_modification_request(e1ap_bearer_context_modification_request& e1ap_request);

  ngap_pdu_session_resource_release_response handle_pdu_session_resource_release_response(bool success);

  const ngap_pdu_session_resource_release_command release_cmd;

  up_config_update next_config;

  e1ap_bearer_context_manager& e1ap_bearer_ctxt_mng; // to trigger bearer context setup at CU-UP
  f1ap_ue_context_manager&     f1ap_ue_ctxt_mng;     // to trigger UE context modification at DU
  rrc_ue_interface*            rrc_ue;               // to trigger RRC Reconfiguration at UE
  cu_cp_rrc_ue_interface&      cu_cp_notifier;       // to trigger UE release at CU-CP
  ue_task_scheduler&           task_sched;           // to schedule UE release request
  up_resource_manager&         up_resource_mng;      // to get RRC DRB config
  ocudulog::basic_logger&      logger;

  // (sub-)routine requests
  f1ap_ue_context_modification_request     ue_context_mod_request;
  e1ap_bearer_context_modification_request bearer_context_modification_request;
  e1ap_bearer_context_release_command      bearer_context_release_command;
  rrc_reconfiguration_procedure_request    rrc_reconfig_args;
  cu_cp_ue_context_release_request         ue_context_release_request;

  // (sub-)routine results
  ngap_pdu_session_resource_release_response response_msg;
  f1ap_ue_context_modification_response      ue_context_modification_response; // to inform DU about the new DRBs
  e1ap_bearer_context_modification_response
       bearer_context_modification_response; // to inform CU-UP about the new TEID for UL F1u traffic
  bool rrc_reconfig_result = false;          // the final UE reconfiguration
};

} // namespace ocucp
} // namespace ocudu
