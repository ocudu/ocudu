// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include <cstdint>
#include <type_traits>

namespace ocudu {

/// Logical Channel Identity used to associate one logical channel to the corresponding RLC bearer. Values (0..32)
enum lcid_t : uint16_t {
  LCID_SRB0        = 0,
  LCID_SRB1        = 1,
  LCID_SRB2        = 2,
  LCID_SRB3        = 3,
  LCID_MIN_DRB     = 4,
  LCID_MAX_DRB     = 32,
  MAX_NOF_RB_LCIDS = 33,
  INVALID_LCID     = 64
};

constexpr lcid_t uint_to_lcid(std::underlying_type_t<lcid_t> val)
{
  return static_cast<lcid_t>(val);
}

constexpr std::underlying_type_t<lcid_t> format_as(lcid_t lcid)
{
  return static_cast<std::underlying_type_t<lcid_t>>(lcid);
}

/// Maximum value of Logical Channel ID.
/// \remark See TS 38.331, maxLC-ID.
constexpr lcid_t MAX_LCID = LCID_MAX_DRB;

constexpr bool is_srb(lcid_t lcid)
{
  return lcid <= LCID_SRB3;
}

inline bool is_lcid_valid(lcid_t lcid)
{
  return lcid <= MAX_LCID;
}

} // namespace ocudu
