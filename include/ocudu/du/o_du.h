// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/du/du.h"

namespace ocudu {

class du_operation_controller;

namespace odu {

class o_du_high;
class o_du_low;

/// O-RAN Distributed Unit interface. Includes the O-RAN DU high, O-RAN DU low and a DU operation controller in an
/// object.
class o_du
{
public:
  virtual ~o_du() = default;

  /// Returns the operation controller of this O-RAN DU.
  virtual du_operation_controller& get_operation_controller() = 0;

  /// Returns the O-RAN DU high of this O-RAN DU.
  virtual o_du_high& get_o_du_high() = 0;

  /// Returns the O-RAN DU low of this O-RAN DU.
  virtual o_du_low& get_o_du_low() = 0;
};

} // namespace odu
} // namespace ocudu
