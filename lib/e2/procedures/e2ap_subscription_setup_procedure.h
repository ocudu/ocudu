// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../common/e2ap_asn1_utils.h"
#include "ocudu/asn1/e2ap/e2ap.h"
#include "ocudu/e2/e2.h"
#include "ocudu/e2/e2_event_manager.h"
#include "ocudu/e2/subscription/e2_subscription.h"
#include "ocudu/support/async/async_task.h"
#include "ocudu/support/timers.h"

class e2_subsciption_notifier;
namespace ocudu {

class e2ap_subscription_setup_procedure
{
public:
  e2ap_subscription_setup_procedure(const asn1::e2ap::ric_sub_request_s& request_,
                                    e2_event_manager&                    event_manager_,
                                    e2_message_notifier&                 ric_notif_,
                                    e2_subscription_proc&                subscription_mngr_,
                                    timer_factory                        timers_,
                                    ocudulog::basic_logger&              logger_);

  void operator()(coro_context<async_task<void>>& ctx);

  static const char* name() { return "E2AP Subscription Setup Procedure"; }

private:
  // results senders
  void send_e2_subscription_setup_response(const e2_subscribe_reponse_message& response);
  void send_e2_subscription_setup_failure(const e2_subscribe_reponse_message& failure);

  const asn1::e2ap::ric_sub_request_s request;
  e2_event_manager&                   event_manager;
  ocudulog::basic_logger&             logger;
  e2_message_notifier&                ric_notif;
  e2_subscription_proc&               subscription_mngr;
  timer_factory                       timers;
};

} // namespace ocudu
