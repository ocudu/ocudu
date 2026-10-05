// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "cu_up_e1_connection_loss_routine.h"
#include "cu_up_setup_routine.h"
#include "ocudu/support/async/async_timer.h"
#include <algorithm>

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
  e1_setup_notifier(dependencies.e1_setup_notifier),
  logger(dependencies.logger)
{
}

void cu_up_e1_connection_loss_routine::operator()(coro_context<async_task<void>>& ctx)
{
  CORO_BEGIN(ctx);

  logger.debug("\"{}\" initialized.", name());

  CORO_AWAIT(ue_mng.remove_e1_ues(e1ap.get_e1_index()));

  // Attempt a new E1 setup connection.
  while (not stop_command) {
    CORO_AWAIT_VALUE(setup_result,
                     launch_async<cu_up_setup_routine>(
                         cu_up_setup_routine_config{.cu_up_id = cu_up_id, .cu_up_name = cu_up_name, .plmns = plmns},
                         cu_up_setup_routine_dependencies{
                             .logger = logger, .e1ap_conn_mng = e1ap, .e1_setup_notifier = e1_setup_notifier}));
    if (setup_result.result == cu_up_setup_result::outcome::success) {
      break;
    }

    nof_retry_waits = 1;
    if (setup_result.result == cu_up_setup_result::outcome::e1_setup_failure) {
      // The CU-CP may accept the CU-UP later, e.g. once its configuration is fixed. Wait at least the Time to Wait, if
      // the CU-CP commanded one, see TS 38.463, Section 8.2.3.3.
      // Note: The retry period divides any Time to Wait value, which is a whole number of seconds.
      nof_retry_waits =
          std::max<unsigned>(1, setup_result.time_to_wait.value_or(std::chrono::seconds{0}) / retry_period);
      logger.warning("e1={}: E1 Setup failed. Cause: {}. Retrying in {}...",
                     fmt::underlying(e1ap.get_e1_index()),
                     setup_result.cause,
                     nof_retry_waits * retry_period);
      if (not setup_failure_reported) {
        // Only the first failure is announced in STDOUT, so that a lasting rejection does not flood it.
        fmt::print("E1 Setup failed. Cause: {}. Retrying in the background\n", setup_result.cause);
        setup_failure_reported = true;
      }
    }

    // Wait in steps, so a stop request does not wait for the whole Time to Wait.
    for (; nof_retry_waits != 0 and not stop_command; --nof_retry_waits) {
      CORO_AWAIT(async_wait_for(retry_timer, retry_period));
    }
  }
  CORO_RETURN();
}
