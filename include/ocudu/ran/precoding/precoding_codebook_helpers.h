// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/precoding/precoding_codebook_configuration.h"

/// \file
/// \brief Precoding Matrix Indicator (PMI) extended information structures and helper functions.
///
/// This file contains structures that extend the information from the PMI codebooks.

namespace ocudu {

/// Single-panel codebook configuration of \f$(N_1, N_2)\f$ and \f$(O_1, O_2)\f$
struct pmi_codebook_single_panel_info {
  /// Parameter \f$N_1\f$.
  unsigned n1;
  /// Parameter \f$N_2\f$.
  unsigned n2;
  /// Parameter \f$O_1\f$.
  unsigned o1;
  /// Parameter \f$O_2\f$.
  unsigned o2;
};

/// Returns the single-panel codebook configuration of \f$(N_1, N_2)\f$ and \f$(O_1, O_2)\f$.
const pmi_codebook_single_panel_info& get_single_panel_info(pmi_codebook_single_panel_config n1_n2);

/// \brief Precoding Matrix Indicator (PMI) parameter bit-widths for Type I single-panel codebooks.
///
/// Unused or fix values for the given configuration are set to zero.
struct pmi_typeI_single_panel_param_sizes {
  /// Parameter \f$i_{1,1}\f$ bit-width.
  unsigned i_1_1;
  /// Parameter \f$i_{1,2}\f$ bit-width.
  unsigned i_1_2;
  /// Parameter \f$i_{1,3}\f$ bit-width.
  unsigned i_1_3;
  /// Parameter \f$i_2\f$ bit-width.
  unsigned i_2;
};

/// Gets PMI parameter sizes for \e TypeI-SinglePanel Mode 1 codebook configuration as per TS38.212 Table 6.3.1.1.2-1.
pmi_typeI_single_panel_param_sizes get_pmi_sizes_typeI_single_panel(const pmi_codebook_single_panel_info& panel_info,
                                                                    uint8_t                               ri);

} // namespace ocudu
