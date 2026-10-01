// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

namespace ocudu {
namespace ofh {

/// Open Fronthaul control interface.
class operation_controller
{
public:
  virtual ~operation_controller() = default;

  /// \brief Starts the Open Fronthaul operation.
  /// \note Caller will be blocked until the controller is fully started.
  virtual void start() = 0;

  /// \brief Stops the Open Fronthaul operation.
  /// \note Caller will be blocked until the controller is fully stopped.
  virtual void stop() = 0;
};

} // namespace ofh
} // namespace ocudu
