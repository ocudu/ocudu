// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include <cstdint>

namespace ocudu {
namespace ether {

/// Open Fronthaul Ethernet receiver metrics.
struct receiver_metrics {
  /// Average latency of packet reception in microseconds.
  float avg_packet_rx_latency_us;
  /// Maximum latency of packet reception in microseconds.
  float max_packet_rx_latency_us;
  /// Total number of bytes received during metrics period.
  uint64_t total_nof_bytes;
  /// CPU usage in microseconds of the Ethernet receiver.
  float cpu_usage_us;
};

} // namespace ether
} // namespace ocudu
