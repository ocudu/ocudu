// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "mac_fapi_p5_start_cell_procedure.h"
#include "p5_transaction_outcome_manager.h"
#include "ocudu/fapi/p5/p5_requests_gateway.h"
#include "ocudu/support/async/execute_on_blocking.h"
#include "ocudu/support/async/protocol_transaction_manager.h"

using namespace ocudu;
using namespace fapi_adaptor;

static fapi::config_request generate_config_request(const fapi::cell_configuration cell_cfg)
{
  return {.cell_cfg = cell_cfg};
}

mac_fapi_start_cell_procedure::mac_fapi_start_cell_procedure(
    const mac_fapi_start_cell_procedure_config&       config,
    const mac_fapi_start_cell_procedure_dependencies& dependencies) :
  is_rt_mode_enabled(config.is_rt_mode_enabled),
  param_req(),
  config_req(generate_config_request(config.cell_cfg)),
  start_req(),
  timeout(config.timeout),
  logger(dependencies.logger),
  p5_gateway(dependencies.p5_gateway),
  transaction_manager(dependencies.transaction_manager),
  mac_ctrl_executor(dependencies.mac_ctrl_executor),
  fapi_ctrl_executor(dependencies.fapi_ctrl_executor),
  timers(dependencies.timers)
{
}

void mac_fapi_start_cell_procedure::operator()(coro_context<async_task<bool>>& ctx)
{
  CORO_BEGIN(ctx);

  // Switch to respective FAPI control executor context.
  CORO_AWAIT(defer_on_blocking(fapi_ctrl_executor, timers));

  // Configure the transaction.
  transaction_param.subscribe_to(transaction_manager.param_response_outcome, timeout);

  p5_gateway.send_param_request(param_req);

  CORO_AWAIT(transaction_param);

  // Failure for some reason. Return.
  if (!handle_param_transaction_result()) {
    // Switch back to MAC control executor context.
    CORO_AWAIT(defer_on_blocking(mac_ctrl_executor, timers));

    CORO_EARLY_RETURN(false);
  }

  transaction_config.subscribe_to(transaction_manager.config_response_outcome, timeout);

  p5_gateway.send_config_request(config_req);

  CORO_AWAIT(transaction_config);

  // Failure for some reason. Return.
  if (!handle_config_transaction_result()) {
    // Switch back to MAC control executor context.
    CORO_AWAIT(defer_on_blocking(mac_ctrl_executor, timers));

    CORO_EARLY_RETURN(false);
  }

  transaction_start.subscribe_to(transaction_manager.start_outcome, timeout);

  p5_gateway.send_start_request(start_req);

  // For non-real time implementations, skip the transaction and checking the outcome of the transaction.
  if (is_rt_mode_enabled) {
    CORO_AWAIT(transaction_start);

    if (!handle_start_transaction_result()) {
      // Switch back to MAC control executor context.
      CORO_AWAIT(defer_on_blocking(mac_ctrl_executor, timers));

      CORO_EARLY_RETURN(false);
    }
  }

  // Switch back to MAC control executor context.
  CORO_AWAIT(defer_on_blocking(mac_ctrl_executor, timers));

  CORO_RETURN(true);
}

bool mac_fapi_start_cell_procedure::handle_param_transaction_result()
{
  if (transaction_param.successful()) {
    if (!transaction_param.response()) {
      logger.warning("PARAM.response contains error code");
    }
    return transaction_param.response();
  }

  if (transaction_param.failed()) {
    logger.warning("PARAM.request failed");
  } else {
    logger.warning("PARAM.request timeout");
  }

  return false;
}

bool mac_fapi_start_cell_procedure::handle_config_transaction_result()
{
  if (transaction_config.successful()) {
    if (!transaction_config.response()) {
      logger.warning("CONFIG.response contains error code");
    }
    return transaction_config.response();
  }

  if (transaction_config.failed()) {
    logger.warning("CONFIG.request failed");
  } else {
    logger.warning("CONFIG.request timeout");
  }

  return false;
}

bool mac_fapi_start_cell_procedure::handle_start_transaction_result()
{
  if (transaction_start.successful()) {
    return transaction_start.response();
  }

  if (transaction_start.failed()) {
    logger.warning("START.request failed");
  } else if (transaction_param.timeout_expired()) {
    logger.warning("START.request timeout");
  }

  return false;
}
