// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/support/ocudu_assert.h"

namespace ocudu {
namespace fapi {

/// Power control offset SS defined as 'ratio of NZP CSI-RS EPRE to SSB/PBCH block EPRE' as per SCF-222 v4.0
/// section 2.2.4.5.
enum class power_control_offset_ss : uint8_t { dB_minus_3, dB0, dB3, dB6 };

/// Converts the given value to a power control offset SS value.
inline power_control_offset_ss to_power_control_offset_ss(int value)
{
  switch (value) {
    case -3:
      return power_control_offset_ss::dB_minus_3;
    case 0:
      return power_control_offset_ss::dB0;
    case 3:
      return power_control_offset_ss::dB3;
    case 6:
      return power_control_offset_ss::dB6;
    default:
      ocudu_assert(0, "Invalid power control offset SS '{}'", value);
      break;
  }
  return power_control_offset_ss::dB0;
}

/// Converts power control offset SS value to an integer.
inline int to_int(power_control_offset_ss value)
{
  switch (value) {
    case power_control_offset_ss::dB_minus_3:
      return -3;
    case power_control_offset_ss::dB0:
      return 0;
    case power_control_offset_ss::dB3:
      return 3;
    case power_control_offset_ss::dB6:
      break;
  }
  return 6;
}

} // namespace fapi
} // namespace ocudu
