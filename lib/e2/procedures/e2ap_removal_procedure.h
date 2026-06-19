// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/e2/e2.h"
#include "ocudu/e2/e2_event_manager.h"
#include "ocudu/support/async/async_task.h"

namespace ocudu {

/// Sends E2 REMOVAL REQUEST to the Near-RT RIC and awaits the response.
/// After this procedure completes the caller must drop the TNL association.
class e2ap_removal_procedure
{
public:
  e2ap_removal_procedure(e2_message_notifier& notif_, e2_event_manager& ev_mng_, ocudulog::basic_logger& logger_);

  void operator()(coro_context<async_task<void>>& ctx);

  static const char* name() { return "E2AP Removal Procedure"; }

private:
  void send_e2_removal_request();

  e2_message_notifier&    notifier;
  e2_event_manager&       ev_mng;
  ocudulog::basic_logger& logger;

  protocol_transaction_outcome_observer<asn1::e2ap::e2_removal_resp_s, asn1::e2ap::e2_removal_fail_s> transaction_sink;
};

} // namespace ocudu
