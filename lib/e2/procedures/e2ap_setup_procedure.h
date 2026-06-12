// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/e2/e2.h"
#include "ocudu/e2/e2_event_manager.h"
#include "ocudu/support/async/async_event_source.h"
#include "ocudu/support/async/async_task.h"

namespace ocudu {

class e2ap_setup_procedure
{
public:
  e2ap_setup_procedure(const e2_setup_request_message& request_,
                       e2_message_notifier&            notif_,
                       e2_event_manager&               ev_mng_,
                       ocudulog::basic_logger&         logger,
                       async_event_source<bool>&       cancel_event_);

  void operator()(coro_context<async_task<e2_setup_response_message>>& ctx);

  static const char* name() { return "E2AP Setup Procedure"; }

private:
  /// Send E2 SETUP REQUEST.
  void send_e2_setup_request();

  /// Checks whether we should attempt again to connect to E2.
  bool retry_required();

  /// Creates procedure result to send back to procedure caller.
  e2_setup_response_message create_e2_setup_result();

  const e2_setup_request_message request;
  e2_message_notifier&           notifier;
  e2_event_manager&              ev_mng;
  ocudulog::basic_logger&        logger;
  async_event_source<bool>&      cancel_event;

  async_single_event_observer<bool> wait_observer;
  e2ap_transaction                  transaction;
  unsigned                          e2_setup_retry_no = 0;
  std::chrono::seconds              time_to_wait{0};
  bool                              timer_expired = false;
};

} // namespace ocudu
