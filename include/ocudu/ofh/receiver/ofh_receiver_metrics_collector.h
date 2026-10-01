// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

namespace ocudu {
namespace ofh {

struct receiver_metrics;

/// Open Fronthaul metrics collector interface used to collect metrics from a receiver.
class receiver_metrics_collector
{
public:
  virtual ~receiver_metrics_collector() = default;

  /// Collect the metrics of an Open Fronthaul receiver.
  virtual void collect_metrics(receiver_metrics& metric) = 0;
};

} // namespace ofh
} // namespace ocudu
