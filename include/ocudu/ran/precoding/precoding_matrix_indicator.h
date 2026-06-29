// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

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
  unsigned pmi;
};

/// \brief Precoding Matrix Indicator (PMI) field for Type I Single-Panel codebook.
///
/// This PMI codebook mode is described in TS38.214 Section 5.2.2.2.1.
struct pmi_typeI_single_panel {
  /// Single-panel topology configuration.
  pmi_codebook_typeI_single_panel panel_config;
  /// PMI parameter \f$i_{1,1}\f$.
  unsigned i_1_1;
  /// PMI parameter \f$i_{1,2}\f$. Only available for \f$N_2 > 1\f$ or \f$\upsilon > 2\f$.
  std::optional<unsigned> i_1_2;
  /// PMI parameter \f$i_{1,3}\f$. Only available for \f$\upsilon \in \{2,3,4\}\f$.
  std::optional<unsigned> i_1_3;
  /// PMI parameter \f$i_2\f$.
  unsigned i_2;
};

/// Unified Precoding Matrix Indicator (PMI) type.
using precoding_matrix_indicator = std::variant<std::monostate, pmi_two_antenna_port, pmi_typeI_single_panel>;

} // namespace ocudu
