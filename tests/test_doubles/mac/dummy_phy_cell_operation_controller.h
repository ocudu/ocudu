// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/mac/phy_cell_operation_controller.h"
#include "ocudu/support/async/async_no_op_task.h"

namespace ocudu {

/// PHY cell operation controller that accepts every start and stop without doing anything.
class dummy_phy_cell_operation_controller : public phy_cell_operation_controller
{
public:
  async_task<bool> start() override { return launch_no_op_task(true); }
  async_task<bool> stop() override { return launch_no_op_task(true); }
};

/// PHY cell operation controller that counts the starts and stops it accepts.
class phy_cell_operation_controller_spy : public dummy_phy_cell_operation_controller
{
  unsigned start_count = 0;
  unsigned stop_count  = 0;

public:
  async_task<bool> start() override
  {
    ++start_count;
    return dummy_phy_cell_operation_controller::start();
  }

  async_task<bool> stop() override
  {
    ++stop_count;
    return dummy_phy_cell_operation_controller::stop();
  }

  unsigned get_start_count() const { return start_count; }
  unsigned get_stop_count() const { return stop_count; }
};

/// Controller for tests that need one to build a cell but never start or stop the cell through it.
inline phy_cell_operation_controller& default_dummy_phy_cell_op_controller()
{
  static dummy_phy_cell_operation_controller dummy;
  return dummy;
}

} // namespace ocudu
