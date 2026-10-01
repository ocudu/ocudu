// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

namespace ocudu {

class du_operation_controller;
class upper_phy;

namespace odu {

class du_low_metrics_collector;

/// DU low interface.
class du_low
{
public:
  /// Default destructor.
  virtual ~du_low() = default;

  /// Returns the operation controller of this DU low.
  virtual du_operation_controller& get_operation_controller() = 0;

  /// Returns the upper PHY for the given cell of this DU low.
  virtual upper_phy& get_upper_phy(unsigned cell_id) = 0;

  /// Returns the metrics collector of this DU low or nullptr if metrics are disabled.
  virtual du_low_metrics_collector* get_metrics_collector() = 0;
};

} // namespace odu
} // namespace ocudu
