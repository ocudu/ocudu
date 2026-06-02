// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../cu_cp_impl_interface.h"
#include "../ue_location_manager/ue_location_manager.h"
#include "../ue_security_manager/ue_security_manager_impl.h"
#include "ocudu/f1ap/cu_cp/f1ap_cu.h"
#include "ocudu/ngap/ngap.h"
#include "ocudu/ngap/ngap_init_context_setup.h"
#include "ocudu/ngap/ngap_ue_radio_capability_management.h"
#include "ocudu/rrc/rrc_ue.h"

namespace ocudu::ocucp {

/// \brief Handles the setup of PDU session resources from the RRC viewpoint.
class initial_context_setup_routine
{
public:
  initial_context_setup_routine(const ngap_init_context_setup_request&       request_,
                                rrc_ue_interface&                            rrc_ue_,
                                ngap_ue_radio_capability_management_handler& ngap_ue_radio_cap_handler_,
                                ue_security_manager&                         security_mng_,
                                ue_location_manager&                         loc_mng_,
                                f1ap_ue_context_manager&                     f1ap_ue_ctxt_mng_,
                                cu_cp_ngap_handler&                          pdu_session_setup_handler_,
                                ocudulog::basic_logger&                      logger_);

  void operator()(
      coro_context<async_task<expected<ngap_init_context_setup_response, ngap_init_context_setup_failure>>>& ctx);

  static const char* name() { return "Initial Context Setup Routine"; }

  void handle_failure(ngap_cause_t cause);
  void handle_nas_pdu(byte_buffer nas_pdu);
  void send_ue_radio_capability_info_indication();

private:
  ngap_init_context_setup_request request;

  rrc_ue_interface&                            rrc_ue;
  ngap_ue_radio_capability_management_handler& ngap_ue_radio_cap_handler;
  ue_security_manager&                         security_mng;
  ue_location_manager&                         loc_mng;
  f1ap_ue_context_manager&                     f1ap_ue_ctxt_mng;          // to trigger UE context setup at F1AP
  cu_cp_ngap_handler&                          pdu_session_setup_handler; // to setup PDU sessions
  ocudulog::basic_logger&                      logger;

  // (sub-)routine requests
  rrc_ue_security_mode_command_context     rrc_smc_ctxt;
  f1ap_ue_context_setup_request            ue_context_setup_request;
  rrc_ue_capability_transfer_request       ue_capability_transfer_request;
  ngap_ue_radio_capability_info_indication ue_radio_cap_info_indication;
  rrc_reconfiguration_procedure_request    rrc_reconfig_args;

  // (sub-)routine results
  f1ap_ue_context_setup_response           ue_context_setup_response;
  bool                                     ue_capability_transfer_result = false; // to query the UE capabilities
  ngap_pdu_session_resource_setup_response pdu_session_setup_response;
  bool                                     security_mode_command_result = false;
  ngap_init_context_setup_failure          fail_msg;
  ngap_init_context_setup_response         resp_msg;
};

} // namespace ocudu::ocucp
