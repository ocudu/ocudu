// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

namespace ocudu {
namespace ether {

struct receiver_metrics;

/// Metrics collector interface used to collect metrics from an Ethernet receiver.
class receiver_metrics_collector
{
public:
  virtual ~receiver_metrics_collector() = default;

  /// Collect the metrics of an Ethernet receiver.
  virtual void collect_metrics(receiver_metrics& metric) = 0;
};

} // namespace ether
} // namespace ocudu
