// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

namespace ocudu {
namespace odu {

struct du_low_metrics;

/// DU low metrics collector.
class du_low_metrics_collector
{
public:
  virtual ~du_low_metrics_collector() = default;

  /// Collect the metrics of this DU low.
  virtual void collect_metrics(odu::du_low_metrics& metrics) = 0;
};

} // namespace odu
} // namespace ocudu
