// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

namespace ocudu {
namespace ofh {

/// Open Fronthaul transmitter message processing metrics.
struct tx_data_flow_perf_metrics {
  /// Message packing statistics.
  float message_packing_min_latency_us;
  float message_packing_max_latency_us;
  float message_packing_avg_latency_us;
  /// CPU usage in microseconds of the message processing in the transmitter.
  float cpu_usage_us;
  /// Number of processing requests dropped because they could not be dispatched to the processing executor.
  unsigned nof_dispatch_failures;
};

} // namespace ofh
} // namespace ocudu
