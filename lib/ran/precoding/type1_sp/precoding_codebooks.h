// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once
#include "ocudu/ran/precoding/precoding_matrix_indicator.h"
#include "ocudu/ran/precoding_beamforming_composite.h"

namespace ocudu {

/// \brief Calculates the MIMO precoding matrix and its beam list for a Type I Single-Panel PMI.
///
/// \param[in] pmi        Type I Single-Panel Precoding Matrix Indicator (PMI).
/// \param[in] nof_layers Number of transmission layers, one to four.
/// \return The MIMO precoding matrix and beam list described by the PMI.
precoding_beamforming_composite calculate_mimo_matrix(const pmi_typeI_single_panel& pmi, unsigned nof_layers);

/// \brief Constructs a precoding weight matrix for a given number of layers for a Type I Single-Panel antenna
/// configuration.
///
/// All weights are derived from TS38.214 Table 5.2.2.2.1-5 to 5.2.2.2.1-8, which describe CSI reporting using Type I
/// Single-Panel codebook for one to four layers. The generated precoding weights for the first half of ports
/// corresponds to the first polarization, while the second half of ports corresponds to the second polarization.
///
/// \param[in] pmi The Precoding Matrix Indicator (PMI) codebook parameters.
/// \param[in] nof_layers The number of layers used for the transmission.
/// \return A precoding weight matrix for the given number of layers and the given antenna panel distribution.
precoding_weight_matrix make_type1_sp_mode1(const pmi_typeI_single_panel& pmi, unsigned nof_layers);

} // namespace ocudu
