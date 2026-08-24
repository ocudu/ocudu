// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../e1ap_cu_cp_impl.h"
#include "../ue_context/e1ap_cu_cp_ue_context.h"
#include "common/e1ap_asn1_utils.h"
#include "ocudu/e1ap/common/e1ap_message.h"
#include "ocudu/e1ap/cu_cp/e1ap_cu_cp.h"
#include "ocudu/support/async/async_task.h"

namespace ocudu::ocucp {

class bearer_context_modification_procedure
{
public:
  bearer_context_modification_procedure(const e1ap_configuration&        e1ap_cfg_,
                                        const e1ap_message&              request_,
                                        e1ap_bearer_transaction_manager& ev_mng_,
                                        e1ap_message_notifier&           e1ap_notif_,
                                        e1ap_ue_logger&                  logger_);

  void operator()(coro_context<async_task<e1ap_bearer_context_modification_response>>& ctx);

  static const char* name() { return "Bearer Context Modification Procedure"; }

private:
  /// Send Bearer Context Modification Request to CU-UP.
  void send_bearer_context_modification_request();

  /// Creates procedure result to send back to procedure caller.
  e1ap_bearer_context_modification_response create_bearer_context_modification_result();

  const e1ap_configuration         e1ap_cfg;
  const e1ap_message               request;
  e1ap_bearer_transaction_manager& ev_mng;
  e1ap_message_notifier&           e1ap_notifier;
  e1ap_ue_logger                   logger;

  protocol_transaction_outcome_observer<asn1::e1ap::bearer_context_mod_resp_s, asn1::e1ap::bearer_context_mod_fail_s>
      transaction_sink;
};

} // namespace ocudu::ocucp
