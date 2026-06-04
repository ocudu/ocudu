// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/e2/e2.h"
#include "ocudu/e2/subscription/e2_subscription.h"
#include "ocudu/support/async/async_task.h"
#include <atomic>

namespace ocudu {

/// Stops active subscriptions, sends E2 REMOVAL REQUEST, and tears down the TNL association.
class ric_connection_removal_routine
{
public:
  ric_connection_removal_routine(e2_connection_manager&   conn_mng,
                                 e2_subscription_manager& subscription_mngr,
                                 std::atomic<bool>&       ric_connected,
                                 ocudulog::basic_logger&  logger);

  void operator()(coro_context<async_task<void>>& ctx);

  static const char* name() { return "RIC Connection Removal Routine"; }

private:
  e2_connection_manager&   conn_mng;
  e2_subscription_manager& subscription_mngr;
  std::atomic<bool>&       ric_connected;
  ocudulog::basic_logger&  logger;
};

} // namespace ocudu
