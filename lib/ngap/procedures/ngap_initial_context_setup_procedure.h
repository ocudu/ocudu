// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../ue_context/ngap_ue_context.h"
#include "metrics/ngap_metrics_aggregator.h"
#include "ocudu/ngap/ngap.h"
#include "ocudu/ngap/ngap_init_context_setup.h"
#include "ocudu/support/async/async_task.h"

namespace ocudu {
namespace ocucp {

class ngap_initial_context_setup_procedure
{
public:
  ngap_initial_context_setup_procedure(const ngap_init_context_setup_request& request_,
                                       const ngap_ue_ids&                     ue_ids_,
                                       ngap_cu_cp_notifier&                   cu_cp_notifier_,
                                       ngap_metrics_aggregator&               metrics_handler_,
                                       ngap_message_notifier&                 amf_notifier_,
                                       ngap_ue_logger&                        logger_);

  void operator()(coro_context<async_task<void>>& ctx);

  static const char* name() { return "Initial Context Setup Procedure"; }

private:
  // results senders
  void send_initial_context_setup_response(const ngap_init_context_setup_response& msg,
                                           const amf_ue_id_t&                      amf_ue_id,
                                           const ran_ue_id_t&                      ran_ue_id);
  void send_initial_context_setup_failure(const ngap_init_context_setup_failure& msg,
                                          const amf_ue_id_t&                     amf_ue_id,
                                          const ran_ue_id_t&                     ran_ue_id);

  ngap_init_context_setup_request request;
  // Copies of the UE identifiers and logger. The NGAP UE context can be removed while this procedure is suspended, so
  // the procedure must not hold references into it.
  const ngap_ue_ids        ue_ids;
  ngap_cu_cp_notifier&     cu_cp_notifier;
  ngap_metrics_aggregator& metrics_handler;
  ngap_message_notifier&   amf_notifier;
  ngap_ue_logger           logger;

  // Maps PDU session ID to S-NSSAI for metrics.
  std::map<pdu_session_id_t, s_nssai_t> pdu_session_id_to_snssai;

  // routine result
  expected<ngap_init_context_setup_response, ngap_init_context_setup_failure> init_ctxt_setup_routine_outcome;
};

} // namespace ocucp
} // namespace ocudu
