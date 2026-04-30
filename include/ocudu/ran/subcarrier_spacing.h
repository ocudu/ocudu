// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/frequency_range.h"
#include "ocudu/support/ocudu_assert.h"
#include <string_view>

namespace ocudu {

/// Number of possible numerology values.
constexpr size_t NOF_NUMEROLOGIES = 5;

/// Representation of subcarrier spacing.
enum class subcarrier_spacing : uint8_t { kHz15 = 0, kHz30, kHz60, kHz120, kHz240, invalid };

/// Check if SCS value is valid.
constexpr bool is_scs_valid(subcarrier_spacing scs)
{
  return scs <= subcarrier_spacing::kHz240;
}

/// Checks whether the provided SCS&ndash;FR pair is valid.
constexpr bool is_scs_valid(subcarrier_spacing scs, frequency_range fr)
{
  return is_scs_valid(scs) and (((fr == frequency_range::FR2) and scs >= subcarrier_spacing::kHz60) or
                                ((fr == frequency_range::FR1) and scs <= subcarrier_spacing::kHz60));
}

/// Convert SCS to numerology index (\f$\mu\f$).
constexpr unsigned to_numerology_value(subcarrier_spacing scs)
{
  return static_cast<unsigned>(scs);
}

/// Converts SCS into integer in kHz.
constexpr unsigned scs_to_khz(subcarrier_spacing scs)
{
  ocudu_assert(is_scs_valid(scs), "Invalid SCS.");
  return 15U << to_numerology_value(scs);
}

/// Convert kHz value to SCS. Returns subcarrier_spacing::invalid if khz does not match a valid SCS.
constexpr subcarrier_spacing khz_to_scs(unsigned khz)
{
  switch (khz) {
    case 15:
      return subcarrier_spacing::kHz15;
    case 30:
      return subcarrier_spacing::kHz30;
    case 60:
      return subcarrier_spacing::kHz60;
    case 120:
      return subcarrier_spacing::kHz120;
    case 240:
      return subcarrier_spacing::kHz240;
    default:
      return subcarrier_spacing::invalid;
  }
}

/// Convert numerology index (\f$\mu\f$) to SCS.
constexpr subcarrier_spacing to_subcarrier_spacing(unsigned numerology)
{
  return static_cast<subcarrier_spacing>(numerology);
}

/// Convert a string to SCS.
inline subcarrier_spacing to_subcarrier_spacing(std::string_view str)
{
  auto in_scs = static_cast<unsigned>(std::strtof(str.data(), nullptr));
  for (unsigned numerology = 0, numerology_end = to_numerology_value(subcarrier_spacing::invalid);
       numerology != numerology_end;
       ++numerology) {
    subcarrier_spacing scs     = to_subcarrier_spacing(numerology);
    unsigned           scs_kHz = scs_to_khz(scs);

    if ((in_scs == scs_kHz) || (in_scs == scs_kHz * 1000)) {
      return scs;
    }
  }
  return subcarrier_spacing::invalid;
}

/// Convert SCS to string.
constexpr const char* to_string(subcarrier_spacing scs)
{
  switch (scs) {
    case subcarrier_spacing::kHz15:
      return "15kHz";
    case subcarrier_spacing::kHz30:
      return "30kHz";
    case subcarrier_spacing::kHz60:
      return "60kHz";
    case subcarrier_spacing::kHz120:
      return "120kHz";
    case subcarrier_spacing::kHz240:
      return "240kHz";
    case subcarrier_spacing::invalid:
    default:
      return "invalid";
  }
}

/// Calculates number of slots per subframe.
constexpr unsigned get_nof_slots_per_subframe(subcarrier_spacing scs)
{
  return 1U << to_numerology_value(scs);
}

} // namespace ocudu
