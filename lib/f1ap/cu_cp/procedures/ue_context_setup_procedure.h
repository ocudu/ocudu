// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "cu_cp/ue_context/f1ap_cu_ue_context.h"
#include "ocudu/asn1/f1ap/f1ap.h"
#include "ocudu/f1ap/cu_cp/f1ap_configuration.h"
#include "ocudu/f1ap/cu_cp/f1ap_cu.h"
#include "ocudu/support/async/async_task.h"

namespace ocudu::ocucp {

class ue_context_setup_procedure
{
public:
  ue_context_setup_procedure(const f1ap_configuration&              f1ap_cfg,
                             const f1ap_ue_context_setup_request&   request_,
                             f1ap_ue_context_list&                  ue_ctxt_list_,
                             f1ap_du_processor_notifier&            du_processor_notifier_,
                             f1ap_message_notifier&                 f1ap_notif_,
                             ocudulog::basic_logger&                logger_,
                             std::optional<rrc_ue_transfer_context> rrc_context);

  void operator()(coro_context<async_task<f1ap_ue_context_setup_response>>& ctx);

  static const char* name() { return "UE Context Setup Procedure"; }

private:
  bool find_or_create_f1ap_ue_context();

  bool create_ue_rrc_context(const f1ap_ue_context_setup_response& ue_ctxt_setup_resp);

  /// Send F1 UE Context Setup Request to DU.
  void send_ue_context_setup_request();

  /// Creates procedure result to send back to procedure caller.
  f1ap_ue_context_setup_response handle_procedure_result();

  const f1ap_configuration&              f1ap_cfg;
  const f1ap_ue_context_setup_request    request;
  f1ap_ue_context_list&                  ue_ctxt_list;
  f1ap_du_processor_notifier&            du_processor_notifier;
  f1ap_message_notifier&                 f1ap_notifier;
  ocudulog::basic_logger&                logger;
  std::optional<rrc_ue_transfer_context> rrc_context; // Initialize new RRC with existing context.

  // Context of the created UE.
  f1ap_ue_context* ue_ctxt = nullptr;
  // True if this procedure created the F1AP UE context.
  bool ue_ctxt_created = false;

  protocol_transaction_outcome_observer<asn1::f1ap::ue_context_setup_resp_s, asn1::f1ap::ue_context_setup_fail_s>
      transaction_sink;
};

} // namespace ocudu::ocucp
