// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include <cstdint>

namespace ocudu {

/// Indicates the number of SSBs per RACH occasion (L1 parameter 'SSB-per-rach-occasion'). See TS 38.331, \c
/// ssb-perRACH-OccasionAndCB-PreamblesPerSSB. Values {1/8, 1/4, 1/2, 1, 2, 4, 8, 16}.
/// Value 1/8 corresponds to one SSB associated with 8 RACH occasions and so on so forth.
enum class ssb_per_rach_occasions : uint8_t { one_eighth = 0, one_forth, one_half, one, two, four, eight, sixteen };

inline float ssb_per_rach_occ_to_float(ssb_per_rach_occasions value)
{
  return static_cast<float>(1U << (static_cast<unsigned>(value))) / 8.0f;
}

/// Number of SS/PBCH block indexes associated with one PRACH occasion, rounded up to unity.
inline unsigned get_nof_ssb_per_ro(ssb_per_rach_occasions value)
{
  const auto idx = static_cast<unsigned>(value);
  const auto one = static_cast<unsigned>(ssb_per_rach_occasions::one);
  return idx >= one ? (1U << (idx - one)) : 1U;
}

/// Number of consecutive PRACH occasions associated with one SS/PBCH block index, rounded up to unity.
inline unsigned get_nof_ro_per_ssb(ssb_per_rach_occasions value)
{
  const auto idx = static_cast<unsigned>(value);
  const auto one = static_cast<unsigned>(ssb_per_rach_occasions::one);
  return idx < one ? (1U << (one - idx)) : 1U;
}

} // namespace ocudu
