// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "e2_entity.h"
#include "../procedures/ric_connection_loss_routine.h"
#include "../procedures/ric_connection_removal_routine.h"
#include "../procedures/ric_connection_setup_routine.h"
#include "../procedures/ric_reconnection_routine.h"
#include "e2_impl.h"
#include "e2_subscription_manager_impl.h"
#include "ocudu/e2/e2.h"
#include "ocudu/support/synchronization/baton.h"
#include <thread>

using namespace ocudu;
using namespace asn1::e2ap;

e2_entity::e2_entity(e2_agent_dependencies&& dependencies) :
  logger(*dependencies.logger),
  cfg(dependencies.cfg),
  task_exec(*dependencies.task_exec),
  timers(*dependencies.timers),
  main_ctrl_loop(128),
  node_cfg_timeout(dependencies.timers->create_timer()),
  node_component_config_provider(std::move(dependencies.node_component_config_provider))
{
  e2sm_mngr         = std::make_unique<e2sm_manager>(logger);
  subscription_mngr = std::make_unique<e2_subscription_manager_impl>(*e2sm_mngr);

  for (auto& e2sm_module : dependencies.e2sm_modules) {
    auto [ran_func_id, oid, packer, interface] = std::move(e2sm_module);
    e2sm_handlers.push_back(std::move(packer));
    e2sm_mngr->add_e2sm_service(oid, std::move(interface));
    subscription_mngr->add_ran_function_oid(ran_func_id, oid);
  }

  e2ap = std::make_unique<e2_impl>(logger,
                                   *this,
                                   *dependencies.timers,
                                   *dependencies.e2_client,
                                   *subscription_mngr,
                                   *e2sm_mngr,
                                   *dependencies.task_exec);
}

void e2_entity::start()
{
  // Start a 5-second timeout so that the setup coroutine is not blocked indefinitely waiting for
  // interface-setup bytes that may never arrive (e.g. if no F1/NG/E1 setup is performed).
  // Dispatch the callback body to task_exec so the aggregator event is only accessed on the E2 thread.
  node_cfg_timeout.set(std::chrono::milliseconds(5000), [this](timer_id_t) {
    if (!task_exec.execute([this]() { node_component_config_provider->on_timeout(); })) {
      logger.warning("Failed to dispatch node config timeout to E2 executor");
    }
  });
  node_cfg_timeout.run();

  if (not task_exec.execute([this]() {
        main_ctrl_loop.schedule([this](coro_context<async_task<void>>& ctx) {
          CORO_BEGIN(ctx);
          CORO_AWAIT(launch_async<ric_connection_setup_routine>(
              cfg, *node_component_config_provider, *e2sm_mngr, *e2ap, timers, logger, stopped, ric_connected));
          CORO_RETURN();
        });
      })) {
    report_fatal_error("Unable to dispatch E2AP setup procedure");
  }
}

void e2_entity::stop()
{
  baton               stop_baton;
  scoped_baton_sender signal_stop{stop_baton};

  stopped = true;

  // Stop E2 interface, so the procedures exits promptly.
  while (not task_exec.execute([this]() { e2ap->stop(); })) {
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

  // Stop and delete RIC connection.
  while (not task_exec.defer([this, signal_stop = std::move(signal_stop)]() mutable {
    main_ctrl_loop.schedule([this, signal_stop = std::move(signal_stop)](coro_context<async_task<void>>& ctx) mutable {
      CORO_BEGIN(ctx);
      CORO_AWAIT(disconnect_ric());

      // RIC disconnection successfully finished. Stop the main task loop.
      // Dispatch main async task loop destruction via defer so that the current coroutine ends successfully.
      while (not task_exec.defer([signal_stop = std::move(signal_stop)]() mutable { signal_stop.post(); })) {
        logger.warning("Unable to stop E2 Agent. Retrying...");
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
      }
      CORO_RETURN();
    });
  })) {
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

  stop_baton.wait();
}

void e2_entity::on_e2_disconnection()
{
  if (stopped) {
    return;
  }
  ric_connected = false;
  if (not main_ctrl_loop.schedule([this](coro_context<async_task<void>>& ctx) {
        CORO_BEGIN(ctx);
        CORO_AWAIT(launch_async<ric_connection_loss_routine>(*subscription_mngr, logger));
        reconnect_to_ric();
        CORO_RETURN();
      })) {
    logger.error("Failed to schedule RIC connection loss handling. Stopping subscriptions.");
    subscription_mngr->stop();
  }
}

async_task<void> e2_entity::disconnect_ric()
{
  return launch_async<ric_connection_removal_routine>(*e2ap, *subscription_mngr, ric_connected, logger);
}

void e2_entity::reconnect_to_ric()
{
  if (stopped) {
    return;
  }
  if (not main_ctrl_loop.schedule([this, success = false](coro_context<async_task<void>>& ctx) mutable {
        CORO_BEGIN(ctx);
        CORO_AWAIT_VALUE(
            success,
            launch_async<ric_reconnection_routine>(
                cfg, *node_component_config_provider, *e2sm_mngr, *e2ap, timers, logger, stopped, ric_connected));
        if (success) {
          logger.info("RIC reconnection successful.");
        } else {
          logger.info("RIC reconnection failed - E2 Setup rejected.");
        }
        CORO_RETURN();
      })) {
    logger.error("Failed to schedule RIC reconnection.");
  }
}
