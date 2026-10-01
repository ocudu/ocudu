// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "cu_cp/procedures/f1ap_cu_event_manager.h"
#include "ocudu/f1ap/cu_cp/f1ap_configuration.h"
#include "ocudu/f1ap/cu_cp/f1ap_cu_configuration_update.h"
#include "ocudu/f1ap/f1ap_message_notifier.h"
#include "ocudu/support/async/async_task.h"

namespace ocudu {
namespace ocucp {

class gnb_cu_configuration_update_procedure
{
public:
  gnb_cu_configuration_update_procedure(const f1ap_configuration&               f1ap_cfg_,
                                        const f1ap_gnb_cu_configuration_update& request_,
                                        f1ap_message_notifier&                  f1ap_notif_,
                                        f1ap_event_manager&                     ev_mng_,
                                        ocudulog::basic_logger&                 logger_);

  void operator()(coro_context<async_task<f1ap_gnb_cu_configuration_update_response>>& ctx);

  static const char* name() { return "GNB CU Configuration Update Procedure"; }

private:
  /// Send F1 GNB CU Configuration Update to DU.
  void send_gnb_cu_configuration_update();

  /// Creates procedure result to send back to procedure caller.
  f1ap_gnb_cu_configuration_update_response handle_procedure_result();

  const f1ap_configuration&              f1ap_cfg;
  const f1ap_gnb_cu_configuration_update request;
  f1ap_message_notifier&                 f1ap_notifier;
  f1ap_event_manager&                    ev_mng;
  ocudulog::basic_logger&                logger;

  f1ap_transaction transaction;

  f1ap_gnb_cu_configuration_update_response response;
};

} // namespace ocucp
} // namespace ocudu
