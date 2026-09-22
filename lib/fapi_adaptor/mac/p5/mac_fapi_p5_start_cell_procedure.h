// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/fapi/cell_config.h"
#include "ocudu/fapi/p5/p5_messages.h"
#include "ocudu/support/async/async_task.h"
#include "ocudu/support/async/protocol_transaction_manager.h"

namespace ocudu {

namespace fapi {
class p5_requests_gateway;
} // namespace fapi

namespace fapi_adaptor {

class operation_controller;
struct p5_transaction_outcome_manager;

/// MAC-FAPI start procedure configuration.
struct mac_fapi_start_cell_procedure_config {
  bool                            is_rt_mode_enabled;
  const fapi::cell_configuration& cell_cfg;
  std::chrono::milliseconds       timeout;
};

/// MAC-FAPI start procedure dependencies.
struct mac_fapi_start_cell_procedure_dependencies {
  ocudulog::basic_logger&         logger;
  fapi::p5_requests_gateway&      p5_gateway;
  p5_transaction_outcome_manager& transaction_manager;
  task_executor&                  mac_ctrl_executor;
  task_executor&                  fapi_ctrl_executor;
  timer_manager&                  timers;
};

/// \brief Procedure to start L1 processing.
///
/// When the L1 processing is requested to start, the procedure performs the following steps in order:
///   - Generate and send the PARAM.request FAPI message.
///   - Wait for the PARAM.response FAPI message or until a timeout occurs, then process the response when received.
///   - Generate and send the CONFIG.request FAPI message.
///   - Wait for the CONFIG.response FAPI message or until a timeout occurs, then process the response when received.
///   - Generate and send the START.request FAPI message.
///   - Wait for the SLOT.indication message or until a timeout occurs.
class mac_fapi_start_cell_procedure
{
public:
  mac_fapi_start_cell_procedure(const mac_fapi_start_cell_procedure_config&       config,
                                const mac_fapi_start_cell_procedure_dependencies& dependencies);

  void operator()(coro_context<async_task<bool>>& ctx);

private:
  /// Handles the param transaction result. Returns true on success, otherwise false.
  bool handle_param_transaction_result();

  /// Handles the config transaction result. Returns true on success, otherwise false.
  bool handle_config_transaction_result();

  /// Handles the start transaction result. Returns true on success, otherwise false.
  bool handle_start_transaction_result();

private:
  const bool                                  is_rt_mode_enabled;
  const fapi::param_request                   param_req;
  const fapi::config_request                  config_req;
  const fapi::start_request                   start_req;
  const std::chrono::milliseconds             timeout;
  ocudulog::basic_logger&                     logger;
  fapi::p5_requests_gateway&                  p5_gateway;
  p5_transaction_outcome_manager&             transaction_manager;
  task_executor&                              mac_ctrl_executor;
  task_executor&                              fapi_ctrl_executor;
  timer_manager&                              timers;
  protocol_transaction_outcome_observer<bool> transaction_param;
  protocol_transaction_outcome_observer<bool> transaction_config;
  protocol_transaction_outcome_observer<bool> transaction_start;
};

} // namespace fapi_adaptor
} // namespace ocudu
