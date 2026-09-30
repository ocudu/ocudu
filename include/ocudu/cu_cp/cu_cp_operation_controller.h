// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

namespace ocudu::ocucp {

/// CU-CP operation controller interface that allows to start/stop a CU-CP.
class cu_cp_operation_controller
{
public:
  virtual ~cu_cp_operation_controller() = default;

  /// Starts the CU-CP.
  virtual void start() = 0;

  /// Stops the CU-CP.
  virtual void stop() = 0;
};

} // namespace ocudu::ocucp
