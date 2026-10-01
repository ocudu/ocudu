// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

namespace ocudu {
namespace fapi_adaptor {

/// FAPI adaptor operation controller.
class operation_controller
{
public:
  virtual ~operation_controller() = default;

  /// Starts the controller.
  virtual void start() = 0;

  /// Stops the controller.
  virtual void stop() = 0;
};

} // namespace fapi_adaptor
} // namespace ocudu
