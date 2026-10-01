// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/ofh/timing/ofh_timing_metrics_collector.h"
#include <atomic>
#include <chrono>

namespace ocudu {
namespace ofh {

/// Open Fronthaul timing metrics collector implementation.
class timing_metrics_collector_impl : public timing_metrics_collector
{
public:
  // See interface for documentation.
  void collect_metrics(timing_metrics& metrics) override;

  /// Updates the skipped symbols statistics.
  void update_skipped_symbols(unsigned num_skipped_symbols);

  /// Updates symbol notification latency statistics.
  void update_symbol_notification_latency(std::chrono::nanoseconds notifier_latency);

private:
  /// Default values for min and max latencies.
  static constexpr uint32_t default_min_latency_ns = std::numeric_limits<uint32_t>::max();
  static constexpr uint32_t default_max_latency_ns = std::numeric_limits<uint32_t>::min();

  std::atomic<uint64_t> nof_skipped_symbols       = {0};
  std::atomic<unsigned> skipped_symbols_max_burst = {0};

  std::atomic<uint32_t> count          = {};
  std::atomic<uint64_t> sum_elapsed_ns = {};
  std::atomic<uint32_t> min_latency_ns = default_min_latency_ns;
  std::atomic<uint32_t> max_latency_ns = default_max_latency_ns;
};

} // namespace ofh
} // namespace ocudu
