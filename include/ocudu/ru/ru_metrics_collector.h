// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

namespace ocudu {

struct ru_metrics;

/// Radio Unit metrics collector that collects metrics from the RU.
class ru_metrics_collector
{
public:
  virtual ~ru_metrics_collector() = default;

  /// Collect the metrics of this RU.
  virtual void collect_metrics(ru_metrics& metrics) = 0;
};

} // namespace ocudu
