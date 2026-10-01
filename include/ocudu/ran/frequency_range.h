// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

namespace ocudu {

/// Labels for the frequency ranges described in TS38.104 Table 5.1-1.
enum class frequency_range {
  /// Frequency range 1, from 410 to 7125 MHz.
  FR1 = 0,
  /// Frequency range 2, from 24250 to 52600 MHz.
  FR2
};

constexpr const char* to_string(frequency_range freq_range)
{
  switch (freq_range) {
    case frequency_range::FR1:
      return "FR1";
    case frequency_range::FR2:
      return "FR2";
    default:
      return "invalid";
  }
}

} // namespace ocudu
