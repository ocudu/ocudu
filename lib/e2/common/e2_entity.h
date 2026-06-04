// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/e2/e2_agent_dependencies.h"
#include "ocudu/e2/e2_node_component_config_provider.h"
#include "ocudu/e2/e2ap_configuration.h"
#include "ocudu/e2/e2sm/e2sm_manager.h"
#include "ocudu/e2/subscription/e2_subscription.h"
#include "ocudu/support/async/fifo_async_task_scheduler.h"
#include "ocudu/support/async/manual_event.h"
#include "ocudu/support/timers.h"
#include <atomic>

namespace ocudu {

/// Wrapper for the E2 interface that puts function calls into its own task executor
class e2_entity final : public e2_agent
{
public:
  e2_entity(e2_agent_dependencies&& dependencies);

  // E2 Agent interface.
  void          start() override;
  void          stop() override;
  e2_interface& get_e2_interface() override { return *e2ap; }
  bool          is_ric_connected() const override { return ric_connected.load(); }

  void on_e2_disconnection() override;

private:
  void reconnect_to_ric();

  ocudulog::basic_logger&  logger;
  const e2ap_configuration cfg;

  // Handler for E2AP tasks.
  task_executor&            task_exec;
  timer_factory             timers;
  fifo_async_task_scheduler main_ctrl_loop;

  // Timeout timer that triggers on_timeout() on the aggregator, ensuring the setup coroutine does not block
  // indefinitely if interface-setup bytes are delayed or missing.
  unique_timer                                       node_cfg_timeout;
  std::unique_ptr<e2_node_component_config_provider> node_component_config_provider;

  std::unique_ptr<e2sm_manager>              e2sm_mngr         = nullptr;
  std::unique_ptr<e2_subscription_manager>   subscription_mngr = nullptr;
  std::unique_ptr<e2_interface>              e2ap              = nullptr;
  std::vector<std::unique_ptr<e2sm_handler>> e2sm_handlers;

  // Set to true by stop() before the disconnect coroutine is dispatched.
  // Both ric_connection_setup_routine and ric_reconnection_routine check this flag at
  // each loop iteration to exit promptly when stop() is called.
  std::atomic<bool> stopped{false};

  // Set to true once E2 Setup succeeds; cleared on disconnection.
  // Used to decide whether to send E2 REMOVAL REQUEST during graceful shutdown.
  std::atomic<bool> ric_connected{false};
};

} // namespace ocudu
