// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

namespace ocudu {

/// Phase Tracking Reference Signals (PT-RS) frequency domain density.
enum class ptrs_frequency_density : uint8_t { two = 2, four = 4 };

/// Converts PT-RS frequency domain density to a string.
inline const char* to_string(ptrs_frequency_density freq_density)
{
  switch (freq_density) {
    case ptrs_frequency_density::two:
      return "two";
    case ptrs_frequency_density::four:
      return "four";
  }
  return "invalid";
}

/// Converts PT-RS frequency domain density to an integer.
inline unsigned to_underlying(ptrs_frequency_density freq_density)
{
  return static_cast<unsigned>(freq_density);
}

/// Phase Tracking Reference Signals (PT-RS) time domain density.
enum class ptrs_time_density : uint8_t { one = 1, two = 2, four = 4 };

/// Converts PT-RS time domain density to a string.
inline const char* to_string(ptrs_time_density time_density)
{
  switch (time_density) {
    case ptrs_time_density::one:
      return "one";
    case ptrs_time_density::two:
      return "two";
    case ptrs_time_density::four:
      return "four";
  }
  return "invalid";
}

/// Converts PT-RS time domain density to an integer.
inline unsigned to_underlying(ptrs_time_density time_density)
{
  return static_cast<unsigned>(time_density);
}

/// Phase Tracking Reference Signals (PT-RS) RE offset.
enum class ptrs_re_offset : uint8_t { offset00 = 0, offset01 = 1, offset10 = 2, offset11 = 3 };

/// Converts PT-RS RE offset to a string.
inline const char* to_string(ptrs_re_offset re_offset)
{
  switch (re_offset) {
    case ptrs_re_offset::offset00:
      return "offset00";
    case ptrs_re_offset::offset01:
      return "offset01";
    case ptrs_re_offset::offset10:
      return "offset10";
    case ptrs_re_offset::offset11:
      return "offset11";
  }
  return "invalid";
}

/// Converts PT-RS RE offset to an integer.
inline unsigned to_underlying(ptrs_re_offset re_offset)
{
  return static_cast<unsigned>(re_offset);
}

} // namespace ocudu
