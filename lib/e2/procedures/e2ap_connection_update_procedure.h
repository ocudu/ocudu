// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/asn1/e2ap/e2ap.h"
#include "ocudu/e2/e2.h"
#include "ocudu/e2/e2_event_manager.h"
#include "ocudu/support/async/async_task.h"
#include "ocudu/support/timers.h"

namespace ocudu {

class e2ap_connection_update_procedure
{
public:
  e2ap_connection_update_procedure(const asn1::e2ap::e2conn_upd_s& request_,
                                   e2_message_notifier&            ric_notif_,
                                   timer_factory                   timers_,
                                   ocudulog::basic_logger&         logger_);

  void operator()(coro_context<async_task<void>>& ctx);

  static const char* name() { return "E2AP Connection Update Procedure"; }

private:
  // results senders
  void send_e2_connection_update_ack();
  void send_e2_connection_update_failure();

  const asn1::e2ap::e2conn_upd_s request;
  ocudulog::basic_logger&        logger;
  e2_message_notifier&           ric_notif;
  timer_factory                  timers;
};

} // namespace ocudu
