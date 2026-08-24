// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../ue_context/ngap_ue_context.h"
#include "ocudu/ngap/ngap.h"
#include "ocudu/ngap/ngap_ue_context_mod.h"
#include "ocudu/support/async/async_task.h"

namespace ocudu::ocucp {

class ngap_ue_context_modification_procedure
{
public:
  ngap_ue_context_modification_procedure(const ngap_ue_context_modification_request& request_,
                                         const ngap_ue_ids&                          ue_ids_,
                                         ngap_cu_cp_notifier&                        cu_cp_notifier_,
                                         ngap_message_notifier&                      amf_notifier_,
                                         ngap_ue_logger&                             logger_);

  void operator()(coro_context<async_task<void>>& ctx);

  static const char* name() { return "UE Context Modification Procedure"; }

private:
  // results senders
  void send_ue_context_modification_response(const ngap_ue_context_modification_response& msg,
                                             const amf_ue_id_t&                           amf_ue_id,
                                             const ran_ue_id_t&                           ran_ue_id);
  void send_ue_context_modification_failure(const ngap_ue_context_modification_failure& msg,
                                            const amf_ue_id_t&                          amf_ue_id,
                                            const ran_ue_id_t&                          ran_ue_id);

  ngap_ue_context_modification_request request;
  // Copies of the UE identifiers and logger. The NGAP UE context can be removed while this procedure is suspended, so
  // the procedure must not hold references into it.
  const ngap_ue_ids      ue_ids;
  ngap_cu_cp_notifier&   cu_cp_notifier;
  ngap_message_notifier& amf_notifier;
  ngap_ue_logger         logger;

  // routine result
  expected<ngap_ue_context_modification_response, ngap_ue_context_modification_failure>
      ue_ctxt_modification_routine_outcome;
};

} // namespace ocudu::ocucp
