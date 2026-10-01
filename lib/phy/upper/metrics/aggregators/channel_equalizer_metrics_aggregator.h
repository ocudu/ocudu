// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/phy/metrics/phy_metrics_notifiers.h"
#include "ocudu/phy/metrics/phy_metrics_reports.h"
#include "ocudu/ran/pusch/pusch_constants.h"
#include <atomic>

namespace ocudu {

/// Channel equalizer metrics aggregator.
class channel_equalizer_metrics_aggregator : public channel_equalizer_metric_notifier
{
public:
  /// Gets the channel equalizer average processing rate in MREps (Millions of RE per second).
  double get_average_rate_MRes(unsigned nof_layers) const
  {
    if (nof_layers < 1) {
      return std::numeric_limits<double>::quiet_NaN();
    }
    const metrics_per_layer& temp = metrics_collection[nof_layers - 1];
    return static_cast<double>(temp.sum_nof_re_layer.load(std::memory_order_relaxed)) /
           static_cast<double>(temp.sum_elapsed_ns.load(std::memory_order_relaxed)) * 1000.0;
  }

  /// Gets the total time spent by the channel equalizer.
  std::chrono::nanoseconds get_total_time() const
  {
    uint64_t sum_ns = std::accumulate(
        metrics_collection.begin(), metrics_collection.end(), 0UL, [](uint64_t sum, const metrics_per_layer& m) {
          return sum + m.sum_elapsed_ns.load(std::memory_order_relaxed);
        });
    return std::chrono::nanoseconds(sum_ns);
  }

  /// Gets the CPU usage in microseconds of the channel equalizer.
  double get_cpu_usage_us() const
  {
    uint64_t sum = 0;
    for (const auto& m : metrics_collection) {
      sum += m.sum_elapsed_ns.load(std::memory_order_relaxed);
    }
    return static_cast<double>(sum) / 1000.0;
  }

  /// Resets values of all internal counters.
  void reset()
  {
    for (auto& metrics : metrics_collection) {
      metrics.reset();
    }
  }

private:
  // See interface for documentation.
  void on_new_metric(const channel_equalizer_metrics& metrics) override
  {
    if (metrics.nof_layers < 1) {
      return;
    }
    metrics_per_layer& temp = metrics_collection[metrics.nof_layers - 1];
    temp.sum_nof_re_layer.fetch_add(metrics.nof_re, std::memory_order_relaxed);
    temp.sum_elapsed_ns.fetch_add(metrics.measurements.duration.count(), std::memory_order_relaxed);
  }

  struct metrics_per_layer {
    std::atomic<uint64_t> sum_nof_re_layer = {};
    std::atomic<uint64_t> sum_elapsed_ns   = {};

    void reset()
    {
      sum_nof_re_layer.store(0, std::memory_order_relaxed);
      sum_elapsed_ns.store(0, std::memory_order_relaxed);
    }
  };

  std::array<metrics_per_layer, pusch_constants::MAX_NOF_LAYERS> metrics_collection;
};

} // namespace ocudu
