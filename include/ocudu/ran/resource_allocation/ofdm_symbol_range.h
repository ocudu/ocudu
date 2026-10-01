// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/interval.h"
#include "ocudu/ran/frame_types.h"
#include "ocudu/ran/resource_allocation/sliv.h"

namespace ocudu {

struct ofdm_symbol_range_tag {};
/// Range [start,stop) of OFDM symbols.
using ofdm_symbol_range = interval<uint8_t, false, ofdm_symbol_range_tag>;

/// \brief Converts SLIV to OFDM symbol start S and length L.
/// \param[in] sliv An index giving a combination (jointly encoded) of start symbols and length indicator (SLIV).
/// \param[out] symbols Symbol interval as [S, S+L).
inline ofdm_symbol_range sliv_to_ofdm_symbols(uint32_t sliv)
{
  uint8_t symbol_S, symbol_L;
  sliv_to_s_and_l(NOF_OFDM_SYM_PER_SLOT_NORMAL_CP, sliv, symbol_S, symbol_L);
  return {symbol_S, symbol_S + symbol_L};
}

/// \brief Converts OFDM symbol range to SLIV.
/// \param[out] symbols Symbol interval as [S, S+L).
/// \param[in] sliv An index giving a combination (jointly encoded) of start symbols and length indicator (SLIV).
inline uint32_t ofdm_symbol_range_to_sliv(ofdm_symbol_range symb_range)
{
  return sliv_from_s_and_l(NOF_OFDM_SYM_PER_SLOT_NORMAL_CP, symb_range.start(), symb_range.length());
}

} // namespace ocudu
