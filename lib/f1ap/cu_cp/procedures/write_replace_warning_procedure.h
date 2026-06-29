// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "f1ap_cu_event_manager.h"
#include "ocudu/f1ap/cu_cp/f1ap_configuration.h"
#include "ocudu/f1ap/cu_cp/f1ap_warning.h"
#include "ocudu/f1ap/f1ap_message_notifier.h"
#include "ocudu/support/async/async_task.h"

namespace ocudu::ocucp {

class write_replace_warning_procedure
{
public:
  write_replace_warning_procedure(const f1ap_configuration&          f1ap_cfg_,
                                  f1ap_write_replace_warning_request request_,
                                  f1ap_message_notifier&             f1ap_notif_,
                                  f1ap_event_manager&                ev_mng_,
                                  ocudulog::basic_logger&            logger_);

  void operator()(coro_context<async_task<f1ap_write_replace_warning_response>>& ctx);

  static const char* name() { return "Write-Replace Warning Procedure"; }

private:
  void                                send_write_replace_warning_request();
  f1ap_write_replace_warning_response handle_procedure_result();

  const f1ap_configuration&                f1ap_cfg;
  const f1ap_write_replace_warning_request request;
  f1ap_message_notifier&                   f1ap_notifier;
  f1ap_event_manager&                      ev_mng;
  ocudulog::basic_logger&                  logger;

  f1ap_transaction                    transaction;
  f1ap_write_replace_warning_response response;
};

} // namespace ocudu::ocucp
