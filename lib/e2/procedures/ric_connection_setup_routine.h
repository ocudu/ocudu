// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/e2/e2.h"
#include "ocudu/e2/e2_node_component_config_provider.h"
#include "ocudu/e2/e2ap_configuration.h"
#include "ocudu/e2/e2sm/e2sm_manager.h"
#include "ocudu/support/async/async_task.h"
#include "ocudu/support/timers.h"
#include <atomic>

namespace ocudu {

/// Establishes the initial E2 connection to the RIC, retrying persistently.
///
/// Unlike ric_reconnection_routine (which is used post-loss), this routine loops on BOTH
/// TNL failure and E2 Setup rejection - the RIC may not be available at gnb startup.
/// Checks stopped at each iteration so e2_entity::stop() can unblock promptly.
class ric_connection_setup_routine
{
public:
  ric_connection_setup_routine(const e2ap_configuration&          cfg,
                               e2_node_component_config_provider& node_cfg_provider,
                               e2sm_manager&                      e2sm_mngr,
                               e2_connection_manager&             e2_conn_mng,
                               timer_factory                      timers,
                               ocudulog::basic_logger&            logger,
                               const std::atomic<bool>&           stopped,
                               std::atomic<bool>&                 ric_connected);

  void operator()(coro_context<async_task<void>>& ctx);

  static const char* name() { return "RIC Connection Setup Routine"; }

private:
  const e2ap_configuration&          cfg;
  e2_node_component_config_provider& node_cfg_provider;
  e2sm_manager&                      e2sm_mngr;
  e2_connection_manager&             e2_conn_mng;
  timer_factory                      timers;
  ocudulog::basic_logger&            logger;
  const std::atomic<bool>&           stopped;
  std::atomic<bool>&                 ric_connected;

  unique_timer retry_timer;
  bool         setup_ok = false;
};

} // namespace ocudu
