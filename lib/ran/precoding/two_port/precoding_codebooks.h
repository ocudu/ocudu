// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once
#include "ocudu/ran/precoding/precoding_weight_matrix.h"

namespace ocudu {

/// \brief Constructs a precoding weight matrix for one layer mapped into two transmit ports.
///
/// All weights are derived from TS38.214 Table 5.2.2.2.1-1 for 1-layer CSI reporting.
///
/// \param[in] i_codebook Codebook identifier.
/// \return A precoding weight matrix for one layer and two ports.
precoding_weight_matrix make_one_layer_two_ports(unsigned i_codebook);

/// \brief Constructs a precoding weight matrix for two layers mapped into two transmit ports.
///
/// All weights are derived from TS38.214 Table 5.2.2.2.1-1 for 2-layer CSI reporting.
///
/// \param[in] i_codebook Codebook identifier.
/// \return A precoding weight matrix for two layers and two ports.
precoding_weight_matrix make_two_layer_two_ports(unsigned i_codebook);

} // namespace ocudu
