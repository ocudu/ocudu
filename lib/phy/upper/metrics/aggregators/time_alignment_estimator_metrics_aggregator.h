// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/phy/metrics/phy_metrics_notifiers.h"
#include "ocudu/phy/metrics/phy_metrics_reports.h"
#include <atomic>

namespace ocudu {

/// Time alignment estimator metrics aggregator.
class time_alignment_estimator_metrics_aggregator : public time_alignment_estimator_metric_notifier
{
public:
  /// Gets the average processing latency in microseconds.
  double get_avg_latency_us() const
  {
    return count.load(std::memory_order_relaxed) ? static_cast<double>(sum_elapsed_ns.load(std::memory_order_relaxed)) /
                                                       static_cast<double>(count.load(std::memory_order_relaxed)) * 1e-3
                                                 : 0;
  }

  /// Gets the average processing rate in MREps (millions of resource elements per second).
  double get_avg_rate_MREps() const
  {
    return sum_elapsed_ns.load(std::memory_order_relaxed)
               ? static_cast<double>(sum_nof_re.load(std::memory_order_relaxed)) /
                     static_cast<double>(sum_elapsed_ns.load(std::memory_order_relaxed)) * 1000
               : 0;
  }

  /// Gets the total amount of time the TA estimator spent calculating.
  std::chrono::nanoseconds get_total_time() const
  {
    return std::chrono::nanoseconds(sum_elapsed_ns.load(std::memory_order_relaxed));
  }

  /// Gets the CPU usage in microseconds of TA estimator.
  double get_cpu_usage_us() const
  {
    return static_cast<double>(sum_elapsed_ns.load(std::memory_order_relaxed)) / 1000.0;
  }

  /// Resets values of all internal counters.
  void reset()
  {
    sum_nof_re.store(0, std::memory_order_relaxed);
    sum_elapsed_ns.store(0, std::memory_order_relaxed);
    count.store(0, std::memory_order_relaxed);
  }

private:
  // See interface for documentation.
  void on_new_metric(const time_alignment_estimator_metrics& metrics) override
  {
    sum_nof_re.fetch_add(metrics.nof_re, std::memory_order_relaxed);
    sum_elapsed_ns.fetch_add(metrics.measurements.duration.count(), std::memory_order_relaxed);
    count.fetch_add(1, std::memory_order_relaxed);
  }

  std::atomic<uint64_t> sum_nof_re     = {};
  std::atomic<uint64_t> sum_elapsed_ns = {};
  std::atomic<uint64_t> count          = {};
};

} // namespace ocudu
