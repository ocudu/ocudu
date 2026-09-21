// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/ran/csi_report/csi_report_configuration.h"
#include "ocudu/ran/precoding/precoding_codebook_configuration.h"

namespace ocudu {
namespace fapi_adaptor {

constexpr unsigned MAX_NOF_RI_RESTRICTIONS = (1U << 8);
constexpr unsigned MAX_NOF_CSI_RESOURCES   = 1;

/// The table is indexed by PMI codebook identifier, so it covers all of them.
constexpr unsigned MAX_NOF_CODEBOOKS = pmi_codebook_id::max() + 1;

constexpr unsigned MAX_NOF_QUANTITIES = static_cast<unsigned>(csi_report_quantities::other);

/// Returns the UCI Part2 correspondence index using the given parameters.
inline unsigned get_uci_part2_correspondence_index(unsigned nof_csi_resources,
                                                   unsigned pmi_codebook,
                                                   unsigned ri_restriction,
                                                   unsigned quantities)
{
  return (MAX_NOF_RI_RESTRICTIONS * MAX_NOF_CSI_RESOURCES * MAX_NOF_CODEBOOKS * quantities) +
         (MAX_NOF_CSI_RESOURCES * MAX_NOF_CODEBOOKS * ri_restriction) + (MAX_NOF_CSI_RESOURCES * pmi_codebook) +
         nof_csi_resources;
}

} // namespace fapi_adaptor
} // namespace ocudu
