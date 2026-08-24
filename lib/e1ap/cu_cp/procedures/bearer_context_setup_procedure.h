// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../e1ap_cu_cp_impl.h"
#include "../ue_context/e1ap_cu_cp_ue_context.h"
#include "ocudu/asn1/e1ap/e1ap.h"
#include "ocudu/e1ap/common/e1ap_message.h"
#include "ocudu/support/async/async_task.h"

namespace ocudu::ocucp {

/// \brief This class handles the E1AP Bearer Context Setup Procedure, as per TS 38.461 8.3.1.
class bearer_context_setup_procedure
{
public:
  bearer_context_setup_procedure(const e1ap_configuration&        e1ap_cfg_,
                                 const e1ap_message&              request_,
                                 e1ap_bearer_transaction_manager& ev_mng_,
                                 e1ap_ue_context_list&            ue_ctxt_list_,
                                 e1ap_message_notifier&           e1ap_notif_,
                                 e1ap_ue_logger&                  logger_);

  void operator()(coro_context<async_task<e1ap_bearer_context_setup_response>>& ctx);

  static const char* name() { return "Bearer Context Setup Procedure"; }

private:
  /// Send Bearer Context Setup Request to DU.
  void send_bearer_context_setup_request();

  /// Handles the E1AP procedure response and forwards the result to the procedure caller.
  e1ap_bearer_context_setup_response handle_bearer_context_setup_response();

  const e1ap_configuration         e1ap_cfg;
  const e1ap_message               request;
  e1ap_bearer_transaction_manager& ev_mng;
  e1ap_ue_context_list&            ue_ctxt_list;
  e1ap_message_notifier&           e1ap_notifier;
  e1ap_ue_logger                   logger;

  protocol_transaction_outcome_observer<asn1::e1ap::bearer_context_setup_resp_s,
                                        asn1::e1ap::bearer_context_setup_fail_s>
      transaction_sink;
};

} // namespace ocudu::ocucp
