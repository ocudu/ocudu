// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

namespace ocudu {
namespace ether {

struct transmitter_metrics;

/// Metrics collector interface used to collect metrics from an Ethernet transmitter.
class transmitter_metrics_collector
{
public:
  virtual ~transmitter_metrics_collector() = default;

  /// Collect the metrics of an Ethernet transmitter.
  virtual void collect_metrics(transmitter_metrics& metric) = 0;
};

} // namespace ether
} // namespace ocudu
