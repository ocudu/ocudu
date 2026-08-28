// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "cu_up_e1_connection_loss_routine.h"
#include "cu_up_setup_routine.h"
#include "ocudu/support/async/async_timer.h"
#include <utility>

using namespace ocudu;
using namespace ocuup;

cu_up_e1_connection_loss_routine::cu_up_e1_connection_loss_routine(
    cu_up_e1_connection_loss_routine_config              cfg,
    const cu_up_e1_connection_loss_routine_dependencies& dependencies) :
  cu_up_id(cfg.cu_up_id),
  cu_up_name(std::move(cfg.cu_up_name)),
  plmns(std::move(cfg.plmns)),
  stop_command(dependencies.stop_command),
  retry_timer(dependencies.timers.create_unique_timer(dependencies.ctrl_exec)),
  e1ap(dependencies.e1ap),
  ue_mng(dependencies.ue_mng),
  logger(dependencies.logger)
{
}

void cu_up_e1_connection_loss_routine::operator()(coro_context<async_task<void>>& ctx)
{
  CORO_BEGIN(ctx);

  logger.debug("\"{}\" initialized.", name());

  CORO_AWAIT(ue_mng.remove_e1_ues(e1ap.get_e1_index()));

  // Attempt a new E1 setup connection.
  for (;;) {
    CORO_AWAIT_VALUE(
        reconnected,
        launch_async<cu_up_setup_routine>(
            cu_up_setup_routine_config{.cu_up_id = cu_up_id, .cu_up_name = cu_up_name, .plmns = plmns},
            cu_up_setup_routine_dependencies{.logger = logger, .e1ap_conn_mng = e1ap, .e1_setup_notifier = nullptr}));
    if (reconnected || stop_command) {
      break;
    }
    CORO_AWAIT(async_wait_for(retry_timer, std::chrono::milliseconds{1000}));
  }
  CORO_RETURN();
}
