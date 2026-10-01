// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../ue_context/f1ap_cu_ue_context.h"
#include "ocudu/asn1/f1ap/f1ap.h"
#include "ocudu/f1ap/cu_cp/f1ap_configuration.h"
#include "ocudu/f1ap/cu_cp/f1ap_cu.h"
#include "ocudu/support/async/async_task.h"

namespace ocudu::ocucp {

class ue_context_release_procedure
{
public:
  ue_context_release_procedure(const f1ap_configuration&              f1ap_cfg_,
                               const f1ap_ue_context_release_command& cmd_,
                               f1ap_ue_context&                       ue_ctxt_,
                               f1ap_message_notifier&                 f1ap_notif_);

  void operator()(coro_context<async_task<cu_cp_ue_index_t>>& ctx);

  static const char* name() { return "UE Context Release Procedure"; }

private:
  /// Send F1 UE Context Release Command to DU.
  void send_ue_context_release_command();

  /// Creates procedure result to send back to procedure caller.
  cu_cp_ue_index_t create_ue_context_release_complete();

  const f1ap_configuration&            f1ap_cfg;
  f1ap_ue_context&                     ue_ctxt;
  asn1::f1ap::ue_context_release_cmd_s command;
  f1ap_message_notifier&               f1ap_notifier;
  ocudulog::basic_logger&              logger;

  cu_cp_ue_index_t release_result = cu_cp_ue_index_t::invalid;

  protocol_transaction_outcome_observer<asn1::f1ap::ue_context_release_complete_s> transaction_sink;
};

} // namespace ocudu::ocucp
