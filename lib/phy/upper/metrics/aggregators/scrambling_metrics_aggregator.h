// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/phy/metrics/phy_metrics_notifiers.h"
#include "ocudu/phy/metrics/phy_metrics_reports.h"
#include "ocudu/ran/subcarrier_spacing.h"
#include "ocudu/support/math/math_utils.h"
#include <atomic>

namespace ocudu {

/// Pseudo-random sequence generator metrics aggregator.
class scrambling_metrics_aggregator : public pseudo_random_sequence_generator_metric_notifier
{
public:
  /// Gets the average initialization time in microseconds.
  double get_average_init_time_us() const
  {
    return init_count.load(std::memory_order_relaxed)
               ? static_cast<double>(sum_init_elapsed_ns.load(std::memory_order_relaxed)) /
                     static_cast<double>(init_count.load(std::memory_order_relaxed)) / 1000.0
               : 0;
  }

  /// Gets the average advance rate in Mbps.
  double get_advance_rate_Mbps() const
  {
    return sum_advance_elapsed_ns.load(std::memory_order_relaxed)
               ? static_cast<double>(sum_advance_nof_bits.load(std::memory_order_relaxed)) /
                     static_cast<double>(sum_advance_elapsed_ns.load(std::memory_order_relaxed)) * 1000.0
               : 0;
  }

  /// Gets the average generation rate in Mbps.
  double get_generate_rate_Mbps() const
  {
    return sum_elapsed_ns.load(std::memory_order_relaxed)
               ? static_cast<double>(sum_nof_bits.load(std::memory_order_relaxed)) /
                     static_cast<double>(sum_elapsed_ns.load(std::memory_order_relaxed)) * 1000.0
               : 0;
  }

  /// Gets the total time spent by the scrambler.
  std::chrono::nanoseconds get_total_time() const
  {
    return std::chrono::nanoseconds(sum_elapsed_ns.load(std::memory_order_relaxed));
  }

  /// Gets the CPU usage in microseconds of scrambling operations.
  double get_cpu_usage_us() const
  {
    return static_cast<double>(sum_init_elapsed_ns.load(std::memory_order_relaxed) +
                               sum_advance_elapsed_ns.load(std::memory_order_relaxed) +
                               sum_elapsed_ns.load(std::memory_order_relaxed)) /
           1000.0;
  }

  /// Resets values of all internal counters.
  void reset()
  {
    sum_init_elapsed_ns.store(0, std::memory_order_relaxed);
    init_count.store(0, std::memory_order_relaxed);
    sum_advance_nof_bits.store(0, std::memory_order_relaxed);
    sum_advance_elapsed_ns.store(0, std::memory_order_relaxed);
    sum_nof_bits.store(0, std::memory_order_relaxed);
    sum_elapsed_ns.store(0, std::memory_order_relaxed);
    count.store(0, std::memory_order_relaxed);
  }

private:
  // See interface for documentation.
  void on_new_metric(const pseudo_random_sequence_generator_metrics& metrics) override
  {
    switch (metrics.method) {
      case pseudo_random_sequence_generator_metrics::methods::init:
        sum_init_elapsed_ns.fetch_add(metrics.measurements.duration.count(), std::memory_order_relaxed);
        init_count.fetch_add(1, std::memory_order_relaxed);
        break;
      case pseudo_random_sequence_generator_metrics::methods::advance:
        sum_advance_nof_bits.fetch_add(metrics.nof_bits, std::memory_order_relaxed);
        sum_advance_elapsed_ns.fetch_add(metrics.measurements.duration.count(), std::memory_order_relaxed);
        break;
      case pseudo_random_sequence_generator_metrics::methods::apply_xor_soft_bit:
      case pseudo_random_sequence_generator_metrics::methods::apply_xor_unpacked:
      case pseudo_random_sequence_generator_metrics::methods::apply_xor_packed:
      case pseudo_random_sequence_generator_metrics::methods::generate_bit_packed:
      case pseudo_random_sequence_generator_metrics::methods::generate_float:
        sum_nof_bits.fetch_add(metrics.nof_bits, std::memory_order_relaxed);
        sum_elapsed_ns.fetch_add(metrics.measurements.duration.count(), std::memory_order_relaxed);
        break;
    }
    count.fetch_add(1, std::memory_order_relaxed);
  }

  std::atomic<uint64_t> sum_init_elapsed_ns    = {};
  std::atomic<uint64_t> init_count             = {};
  std::atomic<uint64_t> sum_advance_nof_bits   = {};
  std::atomic<uint64_t> sum_advance_elapsed_ns = {};
  std::atomic<uint64_t> sum_nof_bits           = {};
  std::atomic<uint64_t> sum_elapsed_ns         = {};
  std::atomic<uint64_t> count                  = {};
};

} // namespace ocudu
