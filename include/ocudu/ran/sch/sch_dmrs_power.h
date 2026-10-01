// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/support/ocudu_assert.h"
#include <array>

namespace ocudu {

/// \brief Calculates the ratio between the Physical Shared Channel EPRE and the DM-RS EPRE.
///
/// The calculation is common for PDSCH and PUSCH, as defined in TS38.214 Table 4.1-1 and Table 6.2.2-1.
///
/// \param[in] nof_cdm_groups_without_data Number of CDM groups without data.
/// \return Parameter \f$\beta_{\textup{DMRS}}\f$ in decibels.
inline float get_sch_to_dmrs_ratio_dB(unsigned nof_cdm_groups_without_data)
{
  ocudu_assert(nof_cdm_groups_without_data > 0 && nof_cdm_groups_without_data < 4,
               "Invalid number of DMRS CDM groups without data.");
  static constexpr std::array<float, 4> beta_dmrs_values = {NAN, 0, -3, -4.77};
  return beta_dmrs_values[nof_cdm_groups_without_data];
}

} // namespace ocudu
