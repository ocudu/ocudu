// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "common/e1ap_logger.h"
#include "e1ap_cu_up_event_manager.h"
#include "ocudu/e1ap/cu_up/e1ap_cu_up.h"

namespace ocudu::ocuup {

/// Holds the E1AP CU-UP setup procedure configuration parameters.
struct e1ap_cu_up_setup_procedure_configuration {
  const cu_up_e1_setup_request& request;
};

/// Holds the E1AP CU-UP setup procedure dependencies.
struct e1ap_cu_up_setup_procedure_dependencies {
  e1ap_message_notifier& cu_cp_notif;
  e1ap_event_manager&    ev_mng;
  e1ap_logger&           logger;
};

/// E1 Setup Procedure for the gNB-CU-UP as per TS 38.463, 8.2.3.
class e1ap_cu_up_setup_procedure
{
public:
  e1ap_cu_up_setup_procedure(const e1ap_cu_up_setup_procedure_configuration& cfg,
                             const e1ap_cu_up_setup_procedure_dependencies&  dependencies);

  void operator()(coro_context<async_task<cu_up_e1_setup_response>>& ctx);

  static const char* name() { return "E1AP CU-UP Setup Procedure"; }

private:
  /// Send E1 SETUP REQUEST to CU-CP.
  void send_cu_up_e1_setup_request();

  /// Creates procedure result to send back to procedure caller.
  cu_up_e1_setup_response create_e1_setup_result();

  const cu_up_e1_setup_request request;
  e1ap_message_notifier&       cu_cp_notifier;
  e1ap_event_manager&          ev_mng;
  e1ap_logger&                 logger;

  e1ap_transaction transaction;
  byte_buffer      captured_request;
};

} // namespace ocudu::ocuup
