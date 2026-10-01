// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include <chrono>
#include <cmath>

namespace ocudu {
namespace ofh {

/// \brief Structure storing the reception window timing parameters expressed in a number of symbols.
struct rx_window_timing_parameters {
  /// Offset from the current OTA symbol to the first symbol at which UL User-Plane message can be received within its
  /// reception window. Must be calculated based on \c Ta4_min parameter.
  unsigned sym_start;
  /// Offset from the current OTA symbol to the last symbol at which UL User-Plane message can be received within its
  /// reception window. Must be calculated based on \c Ta4_max parameter.
  unsigned sym_end;
};

} // namespace ofh
} // namespace ocudu
