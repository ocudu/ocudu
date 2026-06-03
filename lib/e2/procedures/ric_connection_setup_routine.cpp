// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ric_connection_setup_routine.h"
#include "e2_setup_routine.h"
#include "ocudu/support/async/async_timer.h"

using namespace ocudu;

ric_connection_setup_routine::ric_connection_setup_routine(const e2ap_configuration&          cfg_,
                                                           e2_node_component_config_provider& node_cfg_provider_,
                                                           e2sm_manager&                      e2sm_mngr_,
                                                           e2_connection_manager&             e2_conn_mng_,
                                                           timer_factory                      timers_,
                                                           ocudulog::basic_logger&            logger_,
                                                           const std::atomic<bool>&           stopped_) :
  cfg(cfg_),
  node_cfg_provider(node_cfg_provider_),
  e2sm_mngr(e2sm_mngr_),
  e2_conn_mng(e2_conn_mng_),
  timers(timers_),
  logger(logger_),
  stopped(stopped_),
  retry_timer(timers_.create_timer())
{
}

void ric_connection_setup_routine::operator()(coro_context<async_task<void>>& ctx)
{
  CORO_BEGIN(ctx);

  logger.info("\"{}\" started.", name());

  while (not stopped.load()) {
    if (e2_conn_mng.handle_e2_tnl_connection_request()) {
      CORO_AWAIT_VALUE(setup_ok,
                       launch_async<e2_setup_routine>(cfg, node_cfg_provider, e2sm_mngr, e2_conn_mng, timers, logger));
      break;
    }

    logger.info("TNL connection to RIC failed. Retrying in {}ms.", cfg.ric_reconnection_retry_time.count());
    CORO_AWAIT(async_wait_for(retry_timer, cfg.ric_reconnection_retry_time));
  }

  if (setup_ok) {
    logger.info("\"{}\" finished successfully.", name());
  }

  CORO_RETURN();
}
