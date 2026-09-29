// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "cu_up_e1_connection_loss_routine.h"
#include "cu_up_setup_routine.h"
#include "ocudu/support/async/async_timer.h"

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
      // The CU-CP may accept the CU-UP later. Back off exponentially to spare the CU-CP, and throttle the warnings so a
      // lasting misconfiguration stays visible without flooding the log.
      ++nof_setup_failures;
      nof_retry_waits =
          1U << (nof_setup_failures < max_setup_backoff_exponent ? nof_setup_failures : max_setup_backoff_exponent);
      if (nof_setup_failures == 1 or nof_setup_failures % setup_failure_log_period == 0) {
        logger.warning("e1={}: E1 Setup attempt {} failed. Cause: {}. Retrying in {}...",
                       fmt::underlying(e1ap.get_e1_index()),
                       nof_setup_failures,
                       setup_result.e1_setup_failure_cause,
                       nof_retry_waits * retry_period);
      } else {
        logger.debug("e1={}: E1 Setup attempt {} failed. Cause: {}. Retrying in {}...",
                     fmt::underlying(e1ap.get_e1_index()),
                     nof_setup_failures,
                     setup_result.e1_setup_failure_cause,
                     nof_retry_waits * retry_period);
      }
    }

    // Wait in steps, so a stop request does not wait for the whole backoff.
    for (; nof_retry_waits != 0 and not stop_command; --nof_retry_waits) {
      CORO_AWAIT(async_wait_for(retry_timer, retry_period));
    }
  }
  CORO_RETURN();
}
