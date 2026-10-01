// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/support/async/async_task.h"

namespace ocudu {

/// PHY cell operation controller that handles cell activation/deactivation.
class phy_cell_operation_controller
{
public:
  virtual ~phy_cell_operation_controller() = default;

  /// Starts the cell.
  virtual async_task<bool> start() = 0;

  /// Stops the cell.
  virtual async_task<bool> stop() = 0;
};

} // namespace ocudu
