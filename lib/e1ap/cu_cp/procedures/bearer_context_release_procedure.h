// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../ue_context/e1ap_cu_cp_ue_context.h"
#include "ocudu/e1ap/common/e1ap_common.h"
#include "ocudu/e1ap/common/e1ap_message.h"
#include "ocudu/e1ap/cu_cp/e1ap_configuration.h"
#include "ocudu/support/async/async_task.h"

namespace ocudu::ocucp {

class bearer_context_release_procedure
{
public:
  bearer_context_release_procedure(const e1ap_configuration& e1ap_cfg_,
                                   const e1ap_message&       command_,
                                   cu_cp_ue_index_t          ue_index_,
                                   e1ap_ue_context_list&     ue_ctxt_list_,
                                   e1ap_message_notifier&    e1ap_notif_);

  void operator()(coro_context<async_task<void>>& ctx);

  static const char* name() { return "Bearer Context Release Procedure"; }

private:
  /// Send Bearer Context Release Command to CU-UP.
  void send_bearer_context_release_command();

  /// Handles procedure result and returns back to procedure caller.
  void handle_bearer_context_release_complete();

  const e1ap_configuration e1ap_cfg;
  const e1ap_message       command;
  cu_cp_ue_index_t         ue_index;
  e1ap_ue_context_list&    ue_ctxt_list;
  e1ap_message_notifier&   e1ap_notifier;

  protocol_transaction_outcome_observer<asn1::e1ap::bearer_context_release_complete_s> transaction_sink;
};

} // namespace ocudu::ocucp
