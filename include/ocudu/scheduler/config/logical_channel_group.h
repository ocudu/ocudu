// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include <cstdint>
#include <type_traits>

namespace ocudu {

/// Logical Channel Group as per TS38.331.
enum lcg_id_t : uint8_t { MAX_LCG_ID = 7, MAX_NOF_LCGS = 8, LCG_ID_INVALID = 8 };

constexpr lcg_id_t uint_to_lcg_id(std::underlying_type_t<lcg_id_t> val)
{
  return static_cast<lcg_id_t>(val);
}

/// Logical channel group of the SRBs, as per the default SRB configuration of TS 38.331, Section 9.2.1.
constexpr lcg_id_t SRB_LCG_ID = uint_to_lcg_id(0);
/// [Implementation-defined] Logical channel groups of the DRBs, by bearer class and UL HARQ mode.
constexpr lcg_id_t GBR_DRB_LCG_ID            = uint_to_lcg_id(1);
constexpr lcg_id_t NON_GBR_DRB_LCG_ID        = uint_to_lcg_id(2);
constexpr lcg_id_t GBR_DRB_MODE_B_LCG_ID     = uint_to_lcg_id(3);
constexpr lcg_id_t NON_GBR_DRB_MODE_B_LCG_ID = uint_to_lcg_id(4);

} // namespace ocudu
