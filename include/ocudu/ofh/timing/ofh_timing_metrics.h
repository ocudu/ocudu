// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

namespace ocudu {
namespace ofh {

/// Open Fronthaul timing metrics.
struct timing_metrics {
  /// Number of symbols skipped when the timing worker wakes up late.
  unsigned nof_skipped_symbols;
  /// Max burst of skipped symbols.
  unsigned skipped_symbols_max_burst;
  /// Latency of a symbol notification.
  float notification_min_latency_us;
  float notification_max_latency_us;
  float notification_avg_latency_us;
};

} // namespace ofh
} // namespace ocudu
