// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/fapi/p5/p5_messages.h"
#include "ocudu/support/async/async_task.h"
#include "ocudu/support/async/protocol_transaction_manager.h"

namespace ocudu {

namespace fapi {
class p5_requests_gateway;
} // namespace fapi

namespace fapi_adaptor {

struct p5_transaction_outcome_manager;
class operation_controller;

/// MAC-FAPI stop procedure dependencies.
struct mac_fapi_stop_cell_procedure_dependencies {
  ocudulog::basic_logger&         logger;
  fapi::p5_requests_gateway&      p5_gateway;
  p5_transaction_outcome_manager& transaction_manager;
  task_executor&                  mac_ctrl_executor;
  task_executor&                  fapi_ctrl_executor;
  timer_manager&                  timers;
};

/// \brief Procedure for transitioning a cell from an active to inactive.
///
/// Stopping a cell guarantees that all ongoing operations within the cell are brought to a safe halt and that no new
/// work is initiated.
/// When the L1 processing is requested to stop, this procedure performs the following steps in order:
///   - Generate and send the STOP.request FAPI message.
///   - Wait for the STOP.response FAPI message or timeout. Process the response when received.
class mac_fapi_stop_cell_procedure
{
public:
  mac_fapi_stop_cell_procedure(std::chrono::milliseconds                        timeout_,
                               const mac_fapi_stop_cell_procedure_dependencies& dependencies);

  void operator()(coro_context<async_task<bool>>& ctx);

private:
  /// Handles the stop transaction result. Returns true if the result of the transaction was a success, otherwise false.
  bool handle_stop_transaction_result();

private:
  const std::chrono::milliseconds             timeout;
  const fapi::stop_request                    stop_req;
  ocudulog::basic_logger&                     logger;
  fapi::p5_requests_gateway&                  p5_gateway;
  p5_transaction_outcome_manager&             transaction_manager;
  task_executor&                              mac_ctrl_executor;
  task_executor&                              fapi_ctrl_executor;
  timer_manager&                              timers;
  protocol_transaction_outcome_observer<bool> transaction_sink;
};

} // namespace fapi_adaptor
} // namespace ocudu
