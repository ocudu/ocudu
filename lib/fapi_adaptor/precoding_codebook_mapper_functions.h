// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/ran/precoding/precoding_codebook_type1_helpers.h"

namespace ocudu {
namespace fapi_adaptor {

/// Returns the precoding matrix index for the SSB codebook.
inline unsigned get_ssb_precoding_matrix_index()
{
  return 0U;
}

/// Returns the precoding matrix index for the CSI-RS codebook.
inline unsigned get_csi_rs_precoding_matrix_index()
{
  return 0U;
}

/// Returns the precoding matrix index for the PDCCH codebook.
inline unsigned get_pdcch_precoding_matrix_index()
{
  return 0U;
}

/// Returns the precoding matrix index for the DL-PRS codebook.
inline unsigned get_prs_precoding_matrix_index()
{
  return 0U;
}

/// Returns the precoding matrix index for the one-port PDSCH codebook.
inline unsigned get_pdsch_one_port_precoding_matrix_index()
{
  return 0U;
}

/// Returns the precoding matrix index for the PDSCH omnidirectional codebook.
inline unsigned get_pdsch_omnidirectional_precoding_matrix_index()
{
  return 0U;
}

/// Returns the precoding matrix index for the two-port PDSCH codebook using the given PMI.
inline unsigned get_pdsch_two_port_precoding_matrix_index(unsigned pmi)
{
  return pmi;
}

/// Returns the precoding matrix index for a single-panel type 1 PDSCH codebook for the given PMI parameters.
inline unsigned
get_pdsch_single_panel_type1_precoding_matrix_index(const pmi_typeI_single_panel_param_ranges& param_ranges,
                                                    const pmi_typeI_single_panel&              pmi)
{
  // Make sure that the PMI fields are within the valid range by applying modulo operator to each of them.
  unsigned index = pmi.i_1_1 % param_ranges.i_1_1;

  if (param_ranges.i_1_2 > 1) {
    // The PMI report must contain a valid i_1_2 value. If it doesn't, use 0 as a default value to compute a PMI index.
    unsigned i_1_2 = pmi.i_1_2.value_or(0) % param_ranges.i_1_2;
    index          = (index * param_ranges.i_1_2) + i_1_2;
  }

  if (param_ranges.i_1_3 > 1) {
    // The PMI report must contain a valid i_1_3 value. If it doesn't, use 0 as a default value to compute a PMI index.
    unsigned i_1_3 = pmi.i_1_3.value_or(0) % param_ranges.i_1_3;
    index          = (index * param_ranges.i_1_3) + i_1_3;
  }

  index = (index * param_ranges.i_2) + (pmi.i_2 % param_ranges.i_2);

  return index;
}

} // namespace fapi_adaptor
} // namespace ocudu
