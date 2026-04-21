// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "srs_constants.h"
#include "ocudu/ran/channel_matrix/channel_matrix.h"

namespace ocudu {

/// \brief SRS-based estimated channel matrix.
///
/// Channel matrix estimated from the Sounding Reference Signals (SRS).
using srs_channel_matrix = channel_matrix<srs_constants::max_nof_tx_ports, srs_constants::max_nof_rx_ports>;

} // namespace ocudu
