// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

namespace ocudu {
namespace ocuup {

class cu_up_interface;
class cu_up_operation_controller;

/// O-RAN CU-UP interface.
class o_cu_up
{
public:
  virtual ~o_cu_up() = default;

  /// Returns the CU-UP of this O-RAN CU-UP.
  virtual cu_up_interface& get_cu_up() = 0;

  /// Returns the operation controller of this O-RAN CU-UP.
  virtual cu_up_operation_controller& get_operation_controller() = 0;
};

} // namespace ocuup
} // namespace ocudu
