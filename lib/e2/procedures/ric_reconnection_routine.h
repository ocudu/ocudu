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

/// Recovers an E2 TNL connection after loss.
///
/// Waits ric_reconnection_retry_time, then loops handle_e2_tnl_connection_request() until
/// the transport layer is up (checking stopped at each iteration), then runs one
/// e2_setup_routine and returns its bool result.
class ric_reconnection_routine
{
public:
  ric_reconnection_routine(const e2ap_configuration&          cfg,
                           e2_node_component_config_provider& node_cfg_provider,
                           e2sm_manager&                      e2sm_mngr,
                           e2_connection_manager&             e2_conn_mng,
                           timer_factory                      timers,
                           ocudulog::basic_logger&            logger,
                           const std::atomic<bool>&           stopped,
                           std::atomic<bool>&                 ric_connected);

  void operator()(coro_context<async_task<bool>>& ctx);

  static const char* name() { return "RIC Reconnection Routine"; }

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
  bool         reconnected = false;
};

} // namespace ocudu
