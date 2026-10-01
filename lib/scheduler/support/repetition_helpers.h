// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/support/ocudu_assert.h"
#include <algorithm>
#include <array>
#include <cstdint>

namespace ocudu {

/// \brief RV of a PDSCH or PUSCH repetition occasion, as per TS 38.214 Table 5.1.2.1-2 (PDSCH) and the equivalent
/// UL table (PUSCH).
///
/// The UE assumes the fixed cycle {0, 2, 3, 1}, starting at the RV signalled in the DCI and indexed by the nominal
/// occasion position (dropped occasions consume RVs).
inline uint8_t get_repetition_rv(uint8_t dci_rv, unsigned occasion_idx)
{
  static constexpr std::array<uint8_t, 4> rv_cycle = {0, 2, 3, 1};
  const auto*                             it       = std::find(rv_cycle.begin(), rv_cycle.end(), dci_rv);
  ocudu_assert(it != rv_cycle.end(), "Invalid RV value={}", dci_rv);
  return rv_cycle[(std::distance(rv_cycle.begin(), it) + occasion_idx) % rv_cycle.size()];
}

/// \brief Retrieve the RV to be used for a Configured Grant transmission for a specific repetition index and sequence.
///
/// \param[in] reps repetition RV sequence and length specified for the CG.
/// \param[in] rep_idx Repetition index.
/// \return The RV corresponding to the repetition index and sequence.
inline uint8_t get_cg_repetition_rv(const cg_configuration::repetitions_t& reps, unsigned rep_idx)
{
  ocudu_assert(rep_idx < static_cast<unsigned>(reps.rep_k), "Invalid RV index={}", rep_idx);
  // As per \c ConfiguredGrantConfig, TS 38.331, the Repetition RV sequence maximum length is 4; if the \ref rep_idx is
  // larger than 4, the mod operation is performed w.r.t. 4.
  constexpr unsigned max_cg_rv_rep_length = 4U;
  // RV index for the first repetition is always 0.
  if (reps.rep_k == cg_configuration::rep_k_t::n1 or reps.rv_seq == cg_configuration::rep_k_rv::s3_0000) {
    return 0U;
  }
  if (reps.rv_seq == cg_configuration::rep_k_rv::s1_0231) {
    static constexpr std::array<uint8_t, 4> rv_0231 = {0, 2, 3, 1};
    return rv_0231[rep_idx % max_cg_rv_rep_length];
  }
  static constexpr std::array<uint8_t, 4> rv_0303 = {0, 3, 0, 3};
  return rv_0303[rep_idx % max_cg_rv_rep_length];
}

} // namespace ocudu
