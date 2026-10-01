// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

namespace ocudu {

struct upper_phy_metrics;

/// Upper PHY metrics collector.
class upper_phy_metrics_collector
{
public:
  virtual ~upper_phy_metrics_collector() = default;

  /// Collects the metrics of this upper PHY.
  virtual void collect_metrics(upper_phy_metrics& metrics) = 0;
};

} // namespace ocudu
