// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/antenna_topology.h"
#include <cstdint>

namespace ocudu {

/// \brief Single-panel PMI Codebook antenna panel configurations.
///
/// The enumeration contains the possible combinations of \f$N_1, N_2\f$ given in TS38.214 Section 5.2.2.2.1.
///
/// The value of the enumeration is mapped to each of the rows in TS38.214 Table 5.2.2.2.1-2, where the product of
/// \f$N_1\times N_2\f$ is equal to half the number of CSI-RS antenna ports \f$P_{CSI-RS}\f$.
enum class pmi_codebook_single_panel_config : uint8_t {
  two_one     = 0,
  two_two     = 1,
  four_one    = 2,
  three_two   = 3,
  six_one     = 4,
  four_two    = 5,
  eight_one   = 6,
  four_three  = 7,
  six_two     = 8,
  twelve_one  = 9,
  four_four   = 10,
  eight_two   = 11,
  sixteen_one = 12
};

/// \brief PMI Codebook Type I mode enumeration.
///
/// This parameter is given by the higher layer parameter \e codebookMode in the Information Element \e CodebookConfig.
enum class pmi_codebook_typeI_mode : uint8_t { one = 1, two = 2 };

/// \brief PMI Codebook Type II phase-alphabet size.
///
/// This parameter is given by the higher layer parameter \e phaseAlphabetSize in the Information Element
/// \e CodebookConfig. It selects the \f$N_{PSK}\f$ alphabet used for the Type II phase combining coefficients defined
/// in TS38.214 Section 5.2.2.2.3, namely QPSK (\f$N_{PSK}=4\f$) or 8-PSK (\f$N_{PSK}=8\f$).
enum class pmi_codebook_typeII_phase_size : uint8_t { qpsk = 4, psk8 = 8 };

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

/// \brief Returns the antenna topology that realizes a single-panel codebook configuration.
///
/// \param[in] n1_n2 Single-panel configuration \f$(N_1, N_2)\f$.
/// \return The antenna topology with the same antenna element distribution.
/// \remark An error is reported if no supported antenna topology realizes the configuration.
antenna_topology get_single_panel_topology(pmi_codebook_single_panel_config n1_n2);

} // namespace ocudu
