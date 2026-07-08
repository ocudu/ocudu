// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/span.h"

namespace ocudu::pucch_td_helper {

/// Maximum K1 candidates in a dl-DataToUL-ACK list.
static constexpr unsigned MAX_K1_CANDIDATES = 8;

/// \brief Returns the common (fallback) k1 candidates for PDSCH-to-HARQ timing, as per TS38.213, 9.1.2.1.
inline span<const uint8_t> get_common_k1_candidates()
{
  static constexpr std::array<uint8_t, MAX_K1_CANDIDATES> pool = {1, 2, 3, 4, 5, 6, 7, 8};
  return pool;
}

/// \brief Returns the common (fallback) k1 candidates for PDSCH-to-HARQ timing, given a minimum supported k1, as per
/// TS38.213, 9.1.2.1.
inline span<const uint8_t> get_common_k1_candidates(uint8_t min_k1)
{
  auto all = get_common_k1_candidates();
  return span<const uint8_t>(all.data() + (min_k1 - 1), all.size() - (min_k1 - 1));
}

} // namespace ocudu::pucch_td_helper
