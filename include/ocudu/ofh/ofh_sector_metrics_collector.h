// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

namespace ocudu {
namespace ofh {

struct sector_metrics;

/// Open Fronthaul sector metrics collector interface used to collect metrics from a sector.
class metrics_collector
{
public:
  virtual ~metrics_collector() = default;

  /// Collect the metrics of an Open Fronthaul sector.
  virtual void collect_metrics(sector_metrics& metric) = 0;
};

} // namespace ofh
} // namespace ocudu
