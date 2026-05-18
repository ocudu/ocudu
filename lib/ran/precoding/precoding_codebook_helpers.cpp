// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/ran/precoding/precoding_codebook_helpers.h"
#include "ocudu/adt/to_array.h"
#include "ocudu/support/math/math_utils.h"

using namespace ocudu;

/// Contains the parameters describing the antenna single-panel configurations according to TS38.214 Table 5.2.2.2.1-2.
static constexpr auto single_panel_antenna_configurations = to_array<pmi_codebook_single_panel_info>({
    {2, 1, 4, 1},  // Row 0
    {2, 2, 4, 4},  // Row 1
    {4, 1, 4, 1},  // Row 2
    {3, 2, 4, 4},  // Row 3
    {6, 1, 4, 1},  // Row 4
    {4, 2, 4, 4},  // Row 5
    {8, 1, 4, 1},  // Row 6
    {4, 3, 4, 4},  // Row 7
    {6, 2, 4, 4},  // Row 8
    {12, 1, 4, 1}, // Row 9
    {4, 4, 4, 4},  // Row 10
    {8, 2, 4, 4},  // Row 11
    {16, 1, 4, 1}, // Row 12
});

const pmi_codebook_single_panel_info& ocudu::get_single_panel_info(pmi_codebook_single_panel_config n1_n2)
{
  ocudu_assert(n1_n2 <= pmi_codebook_single_panel_config::sixteen_one, "Row index exceeds the table size.");
  return single_panel_antenna_configurations[static_cast<unsigned>(n1_n2)];
}

pmi_typeI_single_panel_param_sizes
ocudu::get_pmi_sizes_typeI_single_panel(const pmi_codebook_single_panel_info& panel_info, uint8_t ri)
{
  unsigned nof_csi_rs_antenna_ports = 2 * panel_info.n1 * panel_info.n2;
  unsigned N1                       = panel_info.n1;
  unsigned N2                       = panel_info.n2;
  unsigned O1                       = panel_info.o1;
  unsigned O2                       = panel_info.o2;

  if ((ri == 1) && (nof_csi_rs_antenna_ports > 2)) {
    return {log2_ceil(N1 * O1), log2_ceil(N2 * O2), 0, 2};
  }

  if ((ri == 2) && (nof_csi_rs_antenna_ports == 4) && (N2 == 1)) {
    return {log2_ceil(N1 * O1), log2_ceil(N2 * O2), 1, 1};
  }

  if ((ri == 2) && (nof_csi_rs_antenna_ports > 4)) {
    return {log2_ceil(N1 * O1), log2_ceil(N2 * O2), 2, 1};
  }

  if (((ri == 3) || (ri == 4)) && (nof_csi_rs_antenna_ports == 4)) {
    return {log2_ceil(N1 * O1), log2_ceil(N2 * O2), 0, 1};
  }

  if (((ri == 3) || (ri == 4)) && (nof_csi_rs_antenna_ports == 8)) {
    return {log2_ceil(N1 * O1), log2_ceil(N2 * O2), 2, 1};
  }

  if ((ri == 5) || (ri == 6)) {
    return {log2_ceil(N1 * O1), log2_ceil(N2 * O2), 0, 1};
  }

  if (((ri == 7) || (ri == 8)) && ((panel_info.n1 == 4) && (panel_info.n2 == 1))) {
    return {log2_ceil((N1 * O1) / 2), log2_ceil(N2 * O2), 0, 1};
  }

  if (((ri == 7) || (ri == 8)) && ((panel_info.n1 == 2) && (panel_info.n2 == 2))) {
    return {log2_ceil(N1 * O1), log2_ceil(N2 * O2), 0, 1};
  }

  report_error("Unhandled case with ri={} nof_csi_rs_antenna_ports={} N2={}.", ri, nof_csi_rs_antenna_ports, N2);
}
