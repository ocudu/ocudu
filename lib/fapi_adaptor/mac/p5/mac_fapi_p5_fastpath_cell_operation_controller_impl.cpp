// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "mac_fapi_p5_fastpath_cell_operation_controller_impl.h"
#include "mac_fapi_p5_start_cell_procedure.h"
#include "mac_fapi_p5_stop_cell_procedure.h"

using namespace ocudu;
using namespace fapi_adaptor;

mac_fapi_p5_fastpath_cell_operation_controller_impl::mac_fapi_p5_fastpath_cell_operation_controller_impl(
    const mac_fapi_p5_fastpath_cell_operation_controller_impl_config&       config,
    const mac_fapi_p5_fastpath_cell_operation_controller_impl_dependencies& dependencies) :
  is_rt_mode_enabled(config.is_rt_mode_enabled),
  cell_cfg(config.cell_cfg),
  logger(dependencies.logger),
  p5_gateway(dependencies.p5_gateway),
  transaction_manager(dependencies.transaction_manager),
  timers(dependencies.timers),
  fapi_ctrl_executor(dependencies.fapi_ctrl_executor),
  mac_ctrl_executor(dependencies.mac_ctrl_executor)
{
}

async_task<bool> mac_fapi_p5_fastpath_cell_operation_controller_impl::start()
{
  mac_fapi_start_cell_procedure_config config = {
      .is_rt_mode_enabled = is_rt_mode_enabled, .cell_cfg = cell_cfg, .timeout = TIMEOUT_IN_MILLISECONDS};
  mac_fapi_start_cell_procedure_dependencies dependencies = {.logger              = logger,
                                                             .p5_gateway          = p5_gateway,
                                                             .transaction_manager = transaction_manager,
                                                             .mac_ctrl_executor   = mac_ctrl_executor,
                                                             .fapi_ctrl_executor  = fapi_ctrl_executor,
                                                             .timers              = timers};
  return launch_async<mac_fapi_start_cell_procedure>(config, dependencies);
}

async_task<bool> mac_fapi_p5_fastpath_cell_operation_controller_impl::stop()
{
  mac_fapi_stop_cell_procedure_dependencies dependencies = {.logger              = logger,
                                                            .p5_gateway          = p5_gateway,
                                                            .transaction_manager = transaction_manager,
                                                            .mac_ctrl_executor   = mac_ctrl_executor,
                                                            .fapi_ctrl_executor  = fapi_ctrl_executor,
                                                            .timers              = timers};

  return launch_async<mac_fapi_stop_cell_procedure>(TIMEOUT_IN_MILLISECONDS, dependencies);
}
