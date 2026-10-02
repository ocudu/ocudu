// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../e1ap_cu_up_connection_handler.h"
#include "e1ap_cu_up_event_manager.h"

namespace ocudu::ocuup {

/// Holds the E1AP CU-UP release procedure dependencies.
struct e1ap_cu_up_release_procedure_dependencies {
  e1ap_cu_up_connection_handler& cu_up_conn_handler;
  e1ap_message_notifier&         tx_pdu_notifier;
  e1ap_event_manager&            ev_mng;
  e1ap_logger&                   logger;
};

/// These coroutines handles the E1AP CU UP release procedure as per TS 37.483, 8.2.7.2.2.
class e1ap_cu_up_release_procedure
{
public:
  explicit e1ap_cu_up_release_procedure(const e1ap_cu_up_release_procedure_dependencies& dependencies);

  void operator()(coro_context<async_task<void>>& ctx);

private:
  const char* name() const { return "E1AP CU-UP Release Procedure"; }

  void send_e1ap_release_request();

  void handle_e1ap_release_response();

  e1ap_cu_up_connection_handler& cu_up_conn_handler;
  e1ap_message_notifier&         cu_notifier;
  e1ap_event_manager&            ev_mng;
  e1ap_logger&                   logger;

  e1ap_transaction transaction;
};

} // namespace ocudu::ocuup
