// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../ngap_validators/ngap_validators.h"
#include "../ue_context/ngap_ue_context.h"
#include "ocudu/ngap/ngap.h"
#include "ocudu/support/async/async_task.h"

namespace ocudu {
namespace ocucp {

class ngap_pdu_session_resource_modify_procedure
{
public:
  ngap_pdu_session_resource_modify_procedure(const ngap_pdu_session_resource_modify_request&     request_,
                                             const asn1::ngap::pdu_session_res_modify_request_s& asn1_request_,
                                             const ngap_ue_ids&                                  ue_ids_,
                                             ngap_cu_cp_notifier&                                cu_cp_notifier_,
                                             ngap_message_notifier&                              amf_notifier_,
                                             ngap_control_message_handler&                       ngap_ctrl_handler_,
                                             ngap_ue_logger&                                     logger_);

  void operator()(coro_context<async_task<void>>& ctx);

  static const char* name() { return "PDU Session Resource Modify Procedure"; }

private:
  void combine_pdu_session_resource_modify_response();

  // Result senders.
  bool send_pdu_session_resource_modify_response();

  const ngap_pdu_session_resource_modify_request     request;
  const asn1::ngap::pdu_session_res_modify_request_s asn1_request;
  const ngap_ue_ids                                  ue_ids;
  ngap_pdu_session_resource_modify_response          response;
  ngap_cu_cp_notifier&                               cu_cp_notifier;
  ngap_message_notifier&                             amf_notifier;
  ngap_control_message_handler&                      ngap_ctrl_handler;
  ngap_ue_logger&                                    logger;

  cu_cp_ue_context_release_request ue_context_release_request;

  // Procedure outcomes.
  pdu_session_resource_modify_validation_outcome verification_outcome;
};

} // namespace ocucp
} // namespace ocudu
