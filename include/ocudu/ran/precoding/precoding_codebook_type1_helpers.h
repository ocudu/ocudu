// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/precoding/precoding_codebook_configuration.h"
#include "ocudu/ran/precoding/precoding_codebook_properties.h"
#include <cstdint>

/// \file
/// \brief Precoding Matrix Indicator (PMI) extended information structures and helper functions for the
/// Type I single-panel codebook.
///
/// This file contains structures that extend the information from the Type I single-panel PMI codebook.

namespace ocudu {

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

/// \brief Precoding Matrix Indicator (PMI) parameter ranges for Type I single-panel codebooks.
///
/// Each of the values give the number of possible values for each of the parameters. The ranges are exclusive, meaning
/// that the fields start at zero.
struct pmi_typeI_single_panel_param_ranges {
  /// Parameter \f$i_{1,1}\f$.
  unsigned i_1_1;
  /// Parameter \f$i_{1,2}\f$.
  unsigned i_1_2;
  /// Parameter \f$i_{1,3}\f$.
  unsigned i_1_3;
  /// Parameter \f$i_2\f$.
  unsigned i_2;
};

/// Gets PMI parameter sizes for \e TypeI-SinglePanel Mode 1 codebook configuration as per TS38.212 Table 6.3.1.1.2-1.
pmi_typeI_single_panel_param_sizes get_pmi_sizes_typeI_single_panel(const pmi_codebook_single_panel_info& panel_info,
                                                                    uint8_t                               ri);

/// \brief Gets PMI parameter ranges for \e TypeI-SinglePanel Mode 1 codebook configuration as per TS38.214
/// Section 5.2.2.2.1.
///
/// The range for each PMI parameter returned by this function is defined as in an exclusive range. Hence, each PMI
/// range value indicates the number of possible values for the corresponding PMI parameter for the given panel
/// topology.
pmi_typeI_single_panel_param_ranges get_pmi_ranges_typeI_single_panel(const pmi_codebook_typeI_single_panel& panel,
                                                                      uint8_t                                ri);

} // namespace ocudu
