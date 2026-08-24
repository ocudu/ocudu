// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../ngap_validators/ngap_validators.h"
#include "../ue_context/ngap_ue_context.h"
#include "metrics/ngap_metrics_aggregator.h"
#include "ocudu/ngap/ngap.h"
#include "ocudu/support/async/async_task.h"

namespace ocudu::ocucp {

class ngap_pdu_session_resource_setup_procedure
{
public:
  ngap_pdu_session_resource_setup_procedure(const ngap_pdu_session_resource_setup_request&     request_,
                                            const asn1::ngap::pdu_session_res_setup_request_s& asn1_request_,
                                            const ngap_ue_ids&                                 ue_ids_,
                                            ngap_cu_cp_notifier&                               cu_cp_notifier_,
                                            ngap_metrics_aggregator&                           metrics_handler_,
                                            ngap_message_notifier&                             amf_notifier_,
                                            ngap_ue_logger&                                    logger_);

  void operator()(coro_context<async_task<void>>& ctx);

  static const char* name() { return "PDU Session Resource Setup Procedure"; }

private:
  void combine_pdu_session_resource_setup_response();

  // Result senders.
  bool send_pdu_session_resource_setup_response();

  const ngap_pdu_session_resource_setup_request     request;
  const asn1::ngap::pdu_session_res_setup_request_s asn1_request;
  ngap_pdu_session_resource_setup_response          validation_response;
  byte_buffer                                       nas_pdu;
  ngap_pdu_session_resource_setup_response          response;
  ngap_cu_cp_notifier&                              cu_cp_notifier;
  ngap_metrics_aggregator&                          metrics_handler;
  ngap_message_notifier&                            amf_notifier;
  // Copies of the UE identifiers and logger. The NGAP UE context can be removed while this procedure is suspended, so
  // the procedure must not hold references into it.
  const ngap_ue_ids ue_ids;
  ngap_ue_logger    logger;

  cu_cp_ue_context_release_request ue_context_release_request;

  // Maps PDU session ID to S-NSSAI for metrics.
  std::map<pdu_session_id_t, s_nssai_t> pdu_session_id_to_snssai;

  // Procedure outcomes.
  pdu_session_resource_setup_validation_outcome verification_outcome;
};

} // namespace ocudu::ocucp
