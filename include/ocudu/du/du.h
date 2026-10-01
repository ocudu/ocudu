// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

namespace ocudu {

class du_operation_controller;

namespace odu {

/// Public DU interface.
class du
{
public:
  virtual ~du() = default;

  /// Returns the operation controller of this DU.
  virtual du_operation_controller& get_operation_controller() = 0;
};

} // namespace odu
} // namespace ocudu
