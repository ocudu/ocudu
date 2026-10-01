// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/phy/metrics/phy_metrics_notifiers.h"
#include "ocudu/phy/metrics/phy_metrics_reports.h"
#include <atomic>

namespace ocudu {

/// PUSCH demodulator metrics aggregator.
class pusch_demodulator_metrics_aggregator : public pusch_demodulator_metric_notifier
{
public:
  /// Gets the PUSCH demodulator average processing time in us.
  double get_average_processing_time_us() const
  {
    return count.load(std::memory_order_relaxed)
               ? static_cast<double>(sum_elapsed_ns.load(std::memory_order_relaxed)) /
                     static_cast<double>(count.load(std::memory_order_relaxed)) / 1000.0
               : 0;
  }

  /// Gets the PUSCH demodulator total processing time excluding buffer operations in us.
  std::chrono::nanoseconds get_total_time() const
  {
    return std::chrono::nanoseconds(sum_elapsed_ns.load(std::memory_order_relaxed) -
                                    sum_elapsed_buffer_ns.load(std::memory_order_relaxed));
  }

  /// Gets the CPU usage in microseconds of the PUSCH demodulator.
  double get_cpu_usage_us() const
  {
    return static_cast<double>(sum_elapsed_ns.load(std::memory_order_relaxed)) / 1000.0;
  }

  /// Resets values of all internal counters.
  void reset()
  {
    count.store(0, std::memory_order_relaxed);
    sum_elapsed_ns.store(0, std::memory_order_relaxed);
    sum_elapsed_buffer_ns.store(0, std::memory_order_relaxed);
  }

private:
  // See interface for documentation.
  void on_new_metric(const pusch_demodulator_metrics& metrics) override
  {
    sum_elapsed_ns.fetch_add(metrics.measurements.duration.count(), std::memory_order_relaxed);
    sum_elapsed_buffer_ns.fetch_add(metrics.elapsed_buffer.count(), std::memory_order_relaxed);
    count.fetch_add(1, std::memory_order_relaxed);
  }

  std::atomic<uint64_t> sum_elapsed_ns        = {};
  std::atomic<uint64_t> sum_elapsed_buffer_ns = {};
  std::atomic<uint64_t> count                 = {};
};

} // namespace ocudu
