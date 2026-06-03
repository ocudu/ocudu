// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/e2/subscription/e2_subscription.h"
#include "ocudu/support/async/async_task.h"

namespace ocudu {

/// Handles the side-effects of an E2 connection loss.
///
/// Stops all active E2 subscriptions. Structured as a coroutine to allow future async
/// cleanup (e.g. draining in-flight indication procedures).
class ric_connection_loss_routine
{
public:
  ric_connection_loss_routine(e2_subscription_manager& subscription_mngr, ocudulog::basic_logger& logger);

  void operator()(coro_context<async_task<void>>& ctx);

  static const char* name() { return "RIC Connection Loss Routine"; }

private:
  e2_subscription_manager& subscription_mngr;
  ocudulog::basic_logger&  logger;
};

} // namespace ocudu
