// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/resource_block.h"
#include "ocudu/support/units.h"

namespace ocudu {

/// \brief Maximum segment length.
///
/// This is given by the maximum lifting size (i.e., 384) times the maximum number of information bits in base graph
/// BG1 (i.e., 22), as per TS38.212 Section 5.2.2.
constexpr units::bits MAX_SEG_LENGTH{22 * 384};

/// \brief Maximum number of segments per transport block.
///
/// It assumes 156 resource elements for a maximum of 275 PRB, four layers and eight bits per RE.
constexpr unsigned MAX_NOF_SEGMENTS = (156 * MAX_NOF_PRBS * 4 * 8) / MAX_SEG_LENGTH.value();

/// \brief Default \f$TBS_{LBRM}\f$.
///
/// Transport block size calculated as per TS38.212 Section 5.4.2.1, parameter \f$TBS_{LBRM}\f$ for 256-QAM, four
/// layers, 273 resource blocks, and maximum rate of \f$948 / 1024\f$.
constexpr units::bytes tbs_lbrm_default = units::bytes(159749);

} // namespace ocudu
