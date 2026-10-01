// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "si_message_controller.h"
#include "ocudu/mac/mac_cell_manager.h"

namespace ocudu {

class mac_dl_cell_controller;

/// Handler of the state of a MAC cell.
class mac_cell_controller_impl final : public mac_cell_controller
{
public:
  mac_cell_controller_impl(const mac_cell_creation_request& cell_cfg,
                           timer_factory                    timers,
                           task_executor&                   ctrl_exec,
                           mac_dl_cell_controller&          dl_cell_);

  async_task<void> start() override;

  async_task<void> stop() override;

  async_task<mac_cell_reconfig_response> reconfigure(const mac_cell_reconfig_request& request) override;

private:
  /// Handler of the System Information broadcast by the cell.
  si_message_controller si_mng;

  /// Controller of the respective cell in the MAC DL processor.
  mac_dl_cell_controller& dl_cell;
};

} // namespace ocudu
