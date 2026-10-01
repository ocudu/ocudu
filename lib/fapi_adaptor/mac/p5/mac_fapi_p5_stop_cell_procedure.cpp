// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "mac_fapi_p5_stop_cell_procedure.h"
#include "p5_transaction_outcome_manager.h"
#include "ocudu/fapi/p5/p5_requests_gateway.h"
#include "ocudu/support/async/execute_on_blocking.h"

using namespace ocudu;
using namespace fapi_adaptor;

mac_fapi_stop_cell_procedure::mac_fapi_stop_cell_procedure(
    std::chrono::milliseconds                        timeout_,
    const mac_fapi_stop_cell_procedure_dependencies& dependencies) :
  timeout(timeout_),
  stop_req(),
  logger(dependencies.logger),
  p5_gateway(dependencies.p5_gateway),
  transaction_manager(dependencies.transaction_manager),
  mac_ctrl_executor(dependencies.mac_ctrl_executor),
  fapi_ctrl_executor(dependencies.fapi_ctrl_executor),
  timers(dependencies.timers)
{
}

void mac_fapi_stop_cell_procedure::operator()(coro_context<async_task<bool>>& ctx)
{
  CORO_BEGIN(ctx);

  // Switch to respective FAPI control executor context.
  CORO_AWAIT(defer_on_blocking(fapi_ctrl_executor, timers));

  // Configure the transaction.
  transaction_sink.subscribe_to(transaction_manager.stop_response_outcome, timeout);

  // Send the STOP.request.
  p5_gateway.send_stop_request(stop_req);

  // Wait for transaction to finish.
  CORO_AWAIT(transaction_sink);

  // Process transaction result.
  if (!handle_stop_transaction_result()) {
    // Switch back to MAC control executor context.
    CORO_AWAIT(defer_on_blocking(mac_ctrl_executor, timers));

    CORO_EARLY_RETURN(false);
  }

  // Switch back to MAC control executor context.
  CORO_AWAIT(defer_on_blocking(mac_ctrl_executor, timers));

  CORO_RETURN(true);
}

bool mac_fapi_stop_cell_procedure::handle_stop_transaction_result()
{
  if (transaction_sink.successful()) {
    return true;
  }

  if (transaction_sink.failed()) {
    logger.warning("STOP.indication failed");
  } else {
    logger.warning("STOP.indication timeout");
  }

  return false;
}
