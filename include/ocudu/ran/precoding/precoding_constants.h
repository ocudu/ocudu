// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/resource_block.h"
#include "ocudu/support/math/math_utils.h"

namespace ocudu {

namespace precoding_constants {

/// Minimum precoding PRG size.
constexpr unsigned MIN_PRG_SIZE = 4;

/// Maximum number of PRG.
constexpr unsigned MAX_NOF_PRG = divide_ceil(MAX_NOF_PRBS, MIN_PRG_SIZE);

/// Maximum number of layers supported by the precoder.
constexpr unsigned MAX_NOF_LAYERS = 8;

/// Maximum number of ports supported by the precoder.
constexpr unsigned MAX_NOF_PORTS = 8;

} // namespace precoding_constants

} // namespace ocudu
