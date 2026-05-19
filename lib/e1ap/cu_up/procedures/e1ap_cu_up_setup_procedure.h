// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../../common/e1ap_asn1_utils.h"
#include "common/e1ap_logger.h"
#include "e1ap_cu_up_event_manager.h"
#include "ocudu/e1ap/common/e1_setup_messages.h"
#include "ocudu/e1ap/cu_up/e1ap_cu_up.h"
#include "ocudu/ocudulog/logger.h"
#include "ocudu/support/async/async_task.h"

namespace ocudu {
namespace ocuup {

/// E1 Setup Procedure for the gNB-CU-UP as per TS 38.463, 8.2.3.
class e1ap_cu_up_setup_procedure
{
public:
  e1ap_cu_up_setup_procedure(const cu_up_e1_setup_request& request_,
                             e1ap_message_notifier&        cu_cp_notif_,
                             e1ap_event_manager&           ev_mng_,
                             timer_factory                 timers_,
                             e1ap_logger&                  logger_);

  void operator()(coro_context<async_task<cu_up_e1_setup_response>>& ctx);

  static const char* name() { return "E1AP CU-UP Setup Procedure"; }

private:
  /// Send E1 SETUP REQUEST to CU-CP.
  void send_cu_up_e1_setup_request();

  /// Checks whether the CU-UP should attempt again to connect to CU-CP.
  bool retry_required();

  /// Creates procedure result to send back to procedure caller.
  cu_up_e1_setup_response create_e1_setup_result();

  const cu_up_e1_setup_request request;
  e1ap_message_notifier&       cu_cp_notifier;
  e1ap_event_manager&          ev_mng;
  timer_factory                timers;
  e1ap_logger&                 logger;

  unique_timer e1_setup_wait_timer;

  e1ap_transaction     transaction;
  unsigned             e1_setup_retry_no = 0;
  std::chrono::seconds time_to_wait{0};
  byte_buffer          captured_request;
};

} // namespace ocuup
} // namespace ocudu
