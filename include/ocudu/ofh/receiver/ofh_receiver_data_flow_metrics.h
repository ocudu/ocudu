// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

namespace ocudu {
namespace ofh {

/// Open Fronthaul receiver message processing metrics.
struct rx_data_flow_perf_metrics {
  /// Received message unpacking statistics.
  float message_unpacking_min_latency_us;
  float message_unpacking_max_latency_us;
  float message_unpacking_avg_latency_us;

  /// Number of dropped messages detected by the data flow.
  unsigned nof_dropped_messages;

  /// Number of corrupted messages by the data flow.
  uint64_t nof_corrupted_messages;

  /// CPU usage in microseconds of the message processing in the receiver.
  float cpu_usage_us;
};

} // namespace ofh
} // namespace ocudu
