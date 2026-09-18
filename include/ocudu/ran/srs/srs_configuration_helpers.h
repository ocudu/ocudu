// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/cyclic_prefix.h"
#include "ocudu/ran/resource_allocation/ofdm_symbol_range.h"
#include "ocudu/ran/srs/srs_configuration.h"
#include "ocudu/ran/srs/srs_resource_configuration.h"

namespace ocudu {

/// Gets the SRS symbol range for a SRS resource within a slot.
ofdm_symbol_range get_srs_symbol_range(const srs_config::srs_resource& res, cyclic_prefix cp);

/// \brief Converts a \c SRS-Resource, as per TS 38.331, into the physical layer parameters needed to derive its RE
/// mapping, as per TS 38.211, Section 6.4.1.4.
srs_resource_configuration to_srs_resource_configuration(const srs_config::srs_resource& res, cyclic_prefix cp);

} // namespace ocudu
