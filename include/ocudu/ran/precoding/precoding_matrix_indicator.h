// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "precoding_codebook_type2_helpers.h"
#include "ocudu/adt/static_vector.h"
#include "ocudu/ran/precoding/precoding_codebook_configuration.h"
#include <optional>
#include <variant>

/// \file
/// \brief Precoding Matrix Indicator (PMI) definitions.
///
/// This file contains the different PMI types. The different types of PMI are described in TS38.214 Section 5.2.2.2.
/// The purpose of these structures are unifying the PMI in CSI reports and the generation of precoding matrices from
/// the CSI reports.

namespace ocudu {

/// Precoding Matrix Indicator (PMI) for two antenna ports.
struct pmi_two_antenna_port {
  /// PMI codebook index from TS38.214 Table 5.2.2.2.1-1
  uint8_t pmi;
};

/// \brief Precoding Matrix Indicator (PMI) field for Type I Single-Panel codebook.
///
/// This PMI codebook mode is described in TS38.214 Section 5.2.2.2.1.
struct pmi_typeI_single_panel {
  /// Single-panel topology configuration.
  pmi_codebook_typeI_single_panel panel_config;
  /// PMI parameter \f$i_{1,1}\f$. Values: {0,...,N_1*O_1 - 1}, at most {0,...,63}.
  uint8_t i_1_1;
  /// PMI parameter \f$i_{1,2}\f$. Only available for \f$N_2 > 1\f$ or \f$\upsilon > 2\f$.
  std::optional<uint8_t> i_1_2;
  /// PMI parameter \f$i_{1,3}\f$. Only available for \f$\upsilon \in \{2,3,4\}\f$.
  std::optional<uint8_t> i_1_3;
  /// PMI parameter \f$i_2\f$.
  uint8_t i_2;
};

/// \brief Precoding Matrix Indicator (PMI) field for the Type II codebook.
///
/// This PMI codebook is described in TS38.214 Section 5.2.2.2.3. The wideband parameters (beam selection \f$i_{1,1}\f$,
/// group selection \f$i_{1,2}\f$, strongest beam \f$i_{1,3,l}\f$ and wideband amplitudes \f$i_{1,4,l}\f$) apply
/// to the whole band, whereas the phase \f$i_{2,1,l}\f$ and optional subband amplitude \f$i_{2,2,l}\f$ coefficients
/// apply to a single subband.
struct pmi_typeII {
  /// \brief Per-layer Type II combining coefficients.
  ///
  /// Each coefficient vector holds one entry per beam and polarization, i.e. 2*L entries.
  struct layer_coefficients {
    /// PMI parameter \f$i_{1,3,l}\f$. Strongest-coefficient index, {0, ..., 2L-1}.
    uint8_t i_1_3;
    /// PMI parameter \f$i_{1,4,l}\f$. Wideband amplitude indices, one per beam and polarization.
    static_vector<uint8_t, max_nof_typeII_coefficients> i_1_4;
    /// PMI parameter \f$i_{2,1,l}\f$. Phase coefficient indices, one per beam and polarization.
    static_vector<uint8_t, max_nof_typeII_coefficients> i_2_1;
    /// PMI parameter \f$i_{2,2,l}\f$. Subband amplitude indices, one per beam and polarization. Empty when subband
    /// amplitude reporting is disabled.
    static_vector<uint8_t, max_nof_typeII_coefficients> i_2_2;
  };

  /// Type II codebook configuration.
  pmi_codebook_typeII config;
  /// PMI parameter \f$i_{1,1}\f$, encoding the beam selection parameters \f$(q_1, q_2)\f$ inside a group. Values:
  /// {0,...,O_1*O_2 - 1}, at most {0,...,15}.
  uint8_t i_1_1;
  /// PMI parameter \f$i_{1,2}\f$, the combinatorial \f$L\f$-beam group selection index, as per TS38.214
  /// Table 5.2.2.2.3-1. Values: {0,...,C(N_1*N_2, L) - 1}, at most {0,...,1819}.
  uint16_t i_1_2;
  /// Per-layer combining coefficients, one entry per layer. The Type II codebook supports rank 1 or 2.
  static_vector<layer_coefficients, max_nof_typeII_layers> layers;
};

/// Unified Precoding Matrix Indicator (PMI) type.
using precoding_matrix_indicator =
    std::variant<std::monostate, pmi_two_antenna_port, pmi_typeI_single_panel, pmi_typeII>;

} // namespace ocudu
