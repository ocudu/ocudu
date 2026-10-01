// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

namespace ocudu {
namespace ofh {

struct timing_metrics;

/// Open Fronthaul timing metrics collector.
class timing_metrics_collector
{
public:
  virtual ~timing_metrics_collector() = default;

  /// Collects the timing metrics and writes them into the given metrics structure.
  virtual void collect_metrics(timing_metrics& metrics) = 0;
};

} // namespace ofh
} // namespace ocudu
