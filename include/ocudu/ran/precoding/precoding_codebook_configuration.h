// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/bounded_integer.h"
#include "ocudu/ran/precoding/precoding_codebook_properties.h"
#include <string>
#include <variant>

/// \file
///
/// \brief Defines the codebook configuration for generating the precoding codebooks and reporting the
/// Precoding Matrix Indicator (PMI).

namespace ocudu {

/// Precoding Matrix Indicator (PMI) codebook configuration for one CSI-RS antenna port.
struct pmi_codebook_one_port {};

/// Precoding Matrix Indicator (PMI) codebook configuration for two CSI-RS antenna ports.
struct pmi_codebook_two_port {};

/// \brief Precoding Matrix Indicator (PMI) codebook configuration for Type I single-panel.
///
/// The configuration is given by higher layer parameters <em>n1-n2</em> and \e codebookMode in the Information Element
/// \e CodebookConfig in TS38.331 Section 6.3.2.
struct pmi_codebook_typeI_single_panel {
  /// Single-panel configuration.
  pmi_codebook_single_panel_config n1_n2;
  /// Codebook mode.
  pmi_codebook_typeI_mode mode;
};

/// \brief Precoding Matrix Indicator (PMI) codebook configuration for Type II.
///
/// The configuration is given by higher layer parameters <em>n1-n2</em>, \e numberOfBeams, \e phaseAlphabetSize and
/// \e subbandAmplitude in the Information Element \e CodebookConfig in TS38.331 Section 6.3.2. The Type II codebook is
/// described in TS38.214 Section 5.2.2.2.3.
struct pmi_codebook_typeII {
  /// Spatial-beam panel configuration \f$(N_1, N_2)\f$, same as the single-panel table.
  pmi_codebook_single_panel_config n1_n2;
  /// Number of combined beams \f$L\f$. Valid values are {2, 3, 4}.
  bounded_integer<unsigned, 2, 4> nof_beams;
  /// Phase-alphabet size \f$N_{PSK}\f$.
  pmi_codebook_typeII_phase_size phase_alphabet_size;
  /// Subband-amplitude reporting flag.
  bool subband_amplitude;
};

/// Defines the PMI codebook configuration types.
using pmi_codebook_config = std::variant<std::monostate,
                                         pmi_codebook_one_port,
                                         pmi_codebook_two_port,
                                         pmi_codebook_typeI_single_panel,
                                         pmi_codebook_typeII>;

/// \brief PMI codebook configuration identifier.
///
/// Each combination of PMI codebook parameters is mapped onto a unique identifier.
///
/// The codebooks are mapped as follows:
/// - 0: one port codebook (and monostate)
/// - 1: two port codebook
/// - [2, 14]: Type I single-panel, mode 1 codebook
/// - [15, 22]: Type II codebook
///
/// \remark Any other combination triggers an assertion.
using pmi_codebook_id = bounded_integer<unsigned, 0, 22>;

/// Converts the codebook configuration to an identifier.
pmi_codebook_id to_pmi_codebook_identifier(const pmi_codebook_config& codebook);

/// Converts a PMI codebook identifier to a PMI codebook configuration.
const pmi_codebook_config& to_pmi_codebook_config(pmi_codebook_id identifier);

/// Converts the PMI codebook configuration to a string.
std::string to_string(const pmi_codebook_config& codebook);

/// Gets the number of CSI-RS antenna ports from the PMI codebook configuration.
unsigned get_precoding_codebook_antenna_ports(const pmi_codebook_config& pmi_codebook);

/// \brief Gets the maximum rank that can be reported with the PMI codebook configuration.
///
/// It is limited by the number of CSI-RS antenna ports and by the maximum number of layers of the codebook type.
unsigned get_precoding_codebook_max_rank(const pmi_codebook_config& pmi_codebook);

} // namespace ocudu
