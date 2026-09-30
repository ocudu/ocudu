// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

namespace ocudu::ocucp {

class cu_cp;
class cu_cp_operation_controller;

/// O-RAN CU-CP interface.
class o_cu_cp
{
public:
  virtual ~o_cu_cp() = default;

  /// Returns the CU-CP of this O-RAN CU-CP.
  virtual cu_cp& get_cu_cp() = 0;

  /// Returns the operation controller of this O-RAN CU-CP.
  virtual cu_cp_operation_controller& get_operation_controller() = 0;
};

} // namespace ocudu::ocucp
