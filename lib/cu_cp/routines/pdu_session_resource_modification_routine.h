// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../cu_cp_impl_interface.h"
#include "../up_resource_manager/up_resource_manager_impl.h"
#include "ocudu/cu_cp/ue_task_scheduler.h"
#include "ocudu/e1ap/cu_cp/e1ap_cu_cp.h"
#include "ocudu/f1ap/cu_cp/f1ap_cu.h"
#include "ocudu/support/async/async_task.h"

namespace ocudu::ocucp {

/// \brief Handles the modification of an existing PDU session resources.
/// TODO Add seqdiag
class pdu_session_resource_modification_routine
{
public:
  pdu_session_resource_modification_routine(const ngap_pdu_session_resource_modify_request& modify_request_,
                                            e1ap_bearer_context_manager&                    e1ap_bearer_ctxt_mng_,
                                            f1ap_ue_context_manager&                        f1ap_ue_ctxt_mng_,
                                            rrc_ue_interface*                               rrc_ue_,
                                            cu_cp_rrc_ue_interface&                         cu_cp_notifier_,
                                            cu_cp_mobility_manager_handler&                 mobility_mng_,
                                            ue_task_scheduler&                              ue_task_sched_,
                                            up_resource_manager&                            up_resource_mng_,
                                            ocudulog::basic_logger&                         logger_);

  void operator()(coro_context<async_task<ngap_pdu_session_resource_modify_response>>& ctx);

  static const char* name() { return "PDU Session Resource Modification Routine"; }

private:
  void fill_e1ap_bearer_context_modification_request(e1ap_bearer_context_modification_request& e1ap_request);
  void fill_initial_e1ap_bearer_context_modification_request(e1ap_bearer_context_modification_request& e1ap_request);

  ngap_pdu_session_resource_modify_response generate_pdu_session_resource_modify_response(bool success);

  const ngap_pdu_session_resource_modify_request modify_request; // the original request

  up_config_update next_config;

  e1ap_bearer_context_manager&    e1ap_bearer_ctxt_mng; // to trigger bearer context setup at CU-UP
  f1ap_ue_context_manager&        f1ap_ue_ctxt_mng;     // to trigger UE context modification at DU
  rrc_ue_interface*               rrc_ue;               // to trigger RRC Reconfiguration at UE
  cu_cp_rrc_ue_interface&         cu_cp_notifier;       // to trigger UE release at CU-CP
  cu_cp_mobility_manager_handler& mobility_mng;         // to trigger intra-cell HO when KgNB key refresh is required
  ue_task_scheduler&              ue_task_sched;        // to schedule UE release request
  up_resource_manager&            up_resource_mng;      // to get RRC DRB config
  ocudulog::basic_logger&         logger;

  // (sub-)routine requests
  e1ap_bearer_context_modification_request bearer_context_modification_request;
  f1ap_ue_context_modification_request     ue_context_mod_request;
  rrc_reconfiguration_procedure_request    rrc_reconfig_args;

  // (sub-)routine results
  ngap_pdu_session_resource_modify_response response_msg;                     // Final routine result.
  f1ap_ue_context_modification_response     ue_context_modification_response; // to inform DU about the new DRBs
  e1ap_bearer_context_modification_response
       bearer_context_modification_response; // to inform CU-UP about the new TEID for UL F1u traffic
  bool rrc_reconfig_result = false;          // the final UE reconfiguration
};

} // namespace ocudu::ocucp
