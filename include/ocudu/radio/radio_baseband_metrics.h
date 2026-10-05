// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include <cstdint>

namespace ocudu {

/// \brief Clipping event counters.
///
/// It comprises the number of clipped samples and the total number of processed samples.
struct clipping_counters {
  uint64_t nof_clipped_samples;
  uint64_t nof_processed_samples;

  bool operator==(const clipping_counters& other) const
  {
    return (nof_clipped_samples == other.nof_clipped_samples) && (nof_processed_samples == other.nof_processed_samples);
  }
  bool operator!=(const clipping_counters& other) const { return !operator==(other); }
};

/// Collects transmit or receive signal statistics.
struct radio_baseband_metrics {
  /// Linear average power.
  float avg_power;
  /// Linear peak power.
  float peak_power;
  /// Clipping counters.
  clipping_counters clipping;
  /// Radio call duration in nanoseconds.
  uint64_t call_duration_ns;
};

} // namespace ocudu
