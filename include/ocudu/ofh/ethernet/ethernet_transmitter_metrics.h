// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include <cstdint>

namespace ocudu {
namespace ether {

/// Open Fronthaul Ethernet transmitter metrics.
struct transmitter_metrics {
  /// Average instantaneous transmission throughput in Megabits per second.
  float avg_rate_Mbps;
  /// Average latency of packet transmission in microseconds.
  float avg_packet_tx_latency_us;
  /// Maximum latency of packet transmission in microseconds.
  float max_packet_tx_latency_us;
  /// Minimum latency of packet transmission in microseconds.
  float min_packet_tx_latency_us;
  /// Total number of bytes transmitted during metrics period.
  uint64_t total_nof_bytes;
  /// CPU usage in microseconds of the Ethernet transmitter.
  float cpu_usage_us;
};

} // namespace ether
} // namespace ocudu
