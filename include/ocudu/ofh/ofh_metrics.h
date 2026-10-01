// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/adt/static_vector.h"
#include "ocudu/ofh/receiver/ofh_receiver_metrics.h"
#include "ocudu/ofh/timing/ofh_timing_metrics.h"
#include "ocudu/ofh/transmitter/ofh_transmitter_metrics.h"
#include "ocudu/ran/gnb_constants.h"
#include <chrono>

namespace ocudu {
namespace ofh {

/// Open Fronthaul sector metrics.
struct sector_metrics {
  /// Sector identifier.
  unsigned sector_id;
  /// Receiver metrics.
  receiver_metrics rx_metrics;
  /// Transmitter metrics.
  transmitter_metrics tx_metrics;
  /// Metrics period.
  std::chrono::milliseconds metrics_period_ms;
};

/// Open Fronthaul metrics.
struct metrics {
  /// Timestamp of the OFH RU metrics.
  std::chrono::time_point<std::chrono::system_clock> timestamp;
  /// Timing metrics.
  timing_metrics timing;
  /// Metrics for every sector.
  static_vector<sector_metrics, MAX_CELLS_PER_DU> sectors;
};

} // namespace ofh
} // namespace ocudu
