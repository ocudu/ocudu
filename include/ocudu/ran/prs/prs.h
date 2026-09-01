// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

/// \file
/// \brief Positioning Reference Signals (PRS) parameters.

#pragma once

#include "ocudu/adt/to_array.h"
#include <cstdint>

namespace ocudu {

/// PRS transmission comb size.
enum class prs_comb_size : uint8_t { two = 2, four = 4, six = 6, twelve = 12 };

/// Valid comb size values, as per TS 38.455, Section 9.2.44, "Comb Size".
inline constexpr auto PRS_VALID_COMB_SIZES = to_array<unsigned>({2, 4, 6, 12});

/// PRS resource repetition factor.
enum class prs_repetition_factor : uint8_t {
  one       = 1,
  two       = 2,
  four      = 4,
  six       = 6,
  eight     = 8,
  sixteen   = 16,
  thirtytwo = 32
};

/// Valid resource repetition factor values, as per TS 38.455, Section 9.2.44, "Resource Repetition Factor".
inline constexpr auto PRS_VALID_REPETITION_FACTORS = to_array<unsigned>({1, 2, 4, 6, 8, 16, 32});

/// PRS transmission time domain duration.
enum class prs_num_symbols : uint8_t { two = 2, four = 4, six = 6, twelve = 12 };

/// Valid time domain duration values, in OFDM symbols, as per TS 38.455, Section 9.2.44, "Resource Number of Symbols".
inline constexpr auto PRS_VALID_NUM_SYMBOLS = to_array<unsigned>({2, 4, 6, 12});

/// Valid resource set periodicity values, in slots, as per TS 38.455, Section 9.2.44, "Resource Set Periodicity".
inline constexpr auto PRS_VALID_PERIODICITIES = to_array<unsigned>(
    {4, 5, 8, 10, 16, 20, 32, 40, 64, 80, 160, 320, 640, 1280, 2560, 5120, 10240, 20480, 40960, 81920, 128, 256, 512});

/// Valid resource time gap values, in slots, as per TS 38.455, Section 9.2.44, "Resource Time Gap".
inline constexpr auto PRS_VALID_TIME_GAPS = to_array<unsigned>({1, 2, 4, 8, 16, 32});

/// \brief Determines whether the combination of time domain duration and comb size is valid.
///
/// The valid combinations are given in TS38.211 Section 7.4.1.7.3.
inline bool prs_valid_num_symbols_and_comb_size(prs_num_symbols nsymb, prs_comb_size comb_sz)
{
  uint8_t nsymb_u8   = static_cast<uint8_t>(nsymb);
  uint8_t comb_sz_u8 = static_cast<uint8_t>(comb_sz);
  return (nsymb_u8 >= comb_sz_u8) && (nsymb_u8 % comb_sz_u8 == 0);
}

} // namespace ocudu
