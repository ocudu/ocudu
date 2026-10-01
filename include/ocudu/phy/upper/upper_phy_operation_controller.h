// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

namespace ocudu {

/// Operation controller to start/stop an upper PHY instance.
class upper_phy_operation_controller
{
public:
  virtual ~upper_phy_operation_controller() = default;

  /// Starts the upper PHY.
  virtual void start() = 0;

  /// Stops the upper PHY.
  virtual void stop() = 0;
};

} // namespace ocudu
