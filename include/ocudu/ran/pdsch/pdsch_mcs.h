// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/sch/sch_mcs.h"

namespace ocudu {

/// \brief Modulation and Code Scheme table identifiers for PDSCH.
///
/// The identifiers are inspired by TS38.331 Section as 6.3.2 (see field \e mcs-Table in various Information Elements).
/// For the actual tables and the selection procedure, see TS38.214 Section 5.1.3.1.
enum class pdsch_mcs_table : uint8_t {
  /// Identifies MCS index table 1, TS38.214 Table 5.1.3.1-1.
  qam64 = 0,
  /// Identifies MCS index table 2, TS38.214 Table 5.1.3.1-2.
  qam256 = 1,
  /// Identifies MCS index table 3, TS38.214 Table 5.1.3.1-3.
  qam64LowSe = 2,
  /// Identifies MCS index table 4, TS38.214 Table 5.1.3.1-4.
  qam1024 = 3,
};

/// \brief Gets the Modulation and Coding Scheme configuration for PDSCH.
///
/// Reserved target code rates and spectral efficiencies are indicated with zero.
///
/// \param[in] table MCS table.
/// \param[in] index MCS index.
/// \return Modulation and Coding Scheme parameters.
sch_mcs_description pdsch_mcs_get_config(pdsch_mcs_table table, sch_mcs_index index);

} // namespace ocudu
