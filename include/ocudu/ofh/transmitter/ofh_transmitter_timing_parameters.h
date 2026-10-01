// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include <chrono>
#include <cmath>

namespace ocudu {
namespace ofh {

/// Structure storing the transmission window timing parameters expressed in a number of symbols.
struct tx_window_timing_parameters {
  /// Offset from the current OTA symbol to the first symbol at which DL Control-Plane message can be sent, or in
  /// other words it is the offset to the start of DL Control-Plane transmission window. Must be calculated based on
  /// \c T1a_max_cp_dl parameter.
  unsigned sym_cp_dl_start;
  /// Offset from the current OTA symbol to the last symbol at which DL Control-Plane message can be sent within its
  /// transmission window. Must be calculated based on \c T1a_min_cp_dl parameter.
  unsigned sym_cp_dl_end;
  /// Offset from the current OTA symbol to the first symbol at which UL Control-Plane message can be sent within its
  /// transmission window. Must be calculated based on \c T1a_max_cp_ul parameter.
  unsigned sym_cp_ul_start;
  /// Offset from the current OTA symbol to the last symbol at which UL Control-Plane message can be sent within its
  /// transmission window. Must be calculated based on \c T1a_min_cp_ul parameter.
  unsigned sym_cp_ul_end;
  /// Offset from the current OTA symbol to the first symbol at which DL User-Plane message can be sent within its
  /// transmission window. Must be calculated based on \c T1a_max_up parameter.
  unsigned sym_up_dl_start;
  /// Offset from the current OTA symbol to the last symbol at which DL User-Plane message can be sent within its
  /// transmission window. Must be calculated based on \c T1a_min_up parameter.
  unsigned sym_up_dl_end;
};

} // namespace ofh
} // namespace ocudu
