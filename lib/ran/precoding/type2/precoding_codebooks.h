// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once
#include "ocudu/ran/precoding/precoding_matrix_indicator.h"
#include "ocudu/ran/precoding_beamforming_composite.h"

namespace ocudu {

/// \brief Calculates the MIMO precoding matrix and its beam list for a Type II PMI.
///
/// \param[in] pmi        Type II Precoding Matrix Indicator (PMI).
/// \param[in] nof_layers Number of transmission layers, one or two.
/// \return The MIMO precoding matrix and beam list described by the PMI.
precoding_beamforming_composite calculate_mimo_matrix(const pmi_typeII& pmi, unsigned nof_layers);

/// \brief Constructs a precoding weight matrix for a given number of layers for a Type II precoding codebook.
///
/// All weights are derived from TS38.214 Section 5.2.2.2.3, which describe CSI reporting using Type II codebook for one
/// or two layers. The generated precoding weights for the first half of ports corresponds to the first polarization,
/// while the second half of ports corresponds to the second polarization.
///
/// \param[in] pmi        The Precoding Matrix Indicator (PMI) codebook parameters.
/// \param[in] nof_layers The number of layers used for the transmission.
/// \return A precoding weight matrix for the given number of layers and the given antenna panel distribution.
precoding_weight_matrix make_type2(const pmi_typeII& pmi, unsigned nof_layers);

} // namespace ocudu
