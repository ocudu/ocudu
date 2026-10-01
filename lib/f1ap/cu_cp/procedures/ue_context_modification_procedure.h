// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../f1ap_cu_impl.h"
#include "../ue_context/f1ap_cu_ue_context.h"
#include "cu_cp/ue_context/f1ap_cu_ue_transaction_manager.h"
#include "f1ap_asn1_utils.h"
#include "ocudu/asn1/f1ap/f1ap.h"
#include "ocudu/f1ap/cu_cp/f1ap_cu.h"
#include "ocudu/support/async/async_task.h"

namespace ocudu {
namespace ocucp {

class ue_context_modification_procedure
{
public:
  ue_context_modification_procedure(const f1ap_configuration&                   f1ap_cfg_,
                                    const f1ap_ue_context_modification_request& request_,
                                    f1ap_ue_context&                            ue_ctxt_,
                                    f1ap_message_notifier&                      f1ap_notif_);

  void operator()(coro_context<async_task<f1ap_ue_context_modification_response>>& ctx);

  static const char* name() { return "UE Context Modification Procedure"; }

private:
  /// Send F1 UE Context Modification Request to DU.
  void send_ue_context_modification_request();

  /// Creates procedure result to send back to procedure caller.
  f1ap_ue_context_modification_response create_ue_context_modification_result();

  const f1ap_configuration&                  f1ap_cfg;
  const f1ap_ue_context_modification_request request;
  f1ap_ue_context&                           ue_ctxt;
  f1ap_message_notifier&                     f1ap_notifier;
  ocudulog::basic_logger&                    logger;

  protocol_transaction_outcome_observer<asn1::f1ap::ue_context_mod_resp_s, asn1::f1ap::ue_context_mod_fail_s>
      transaction_sink;
};

} // namespace ocucp
} // namespace ocudu
