// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "precoding_matrix_indicator.h"
#include "ocudu/ran/precoding/precoding_constants.h"
#include "ocudu/ran/precoding/precoding_weight_matrix.h"
#include "ocudu/ran/precoding_beamforming_composite.h"

namespace ocudu {

/// \brief Maximum number of beams contained in a composite precoding matrix weights.
///
/// It is derived from the structure of the 3GPP precoding matrices defined in TS38.214 Section 5.2.2.2:
/// - the Type I Single-Panel codebook uses at most two spatial beams, \f$v_{l, m}\f$ and \f$v_{l', m'}\f$ with
///   \f$l \neq l'\f$ and \f$m \neq m'\f$, which occurs for four layers; and
/// - the Type II codebook combines up to \f$L = 4\f$ spatial beams, as per Section 5.2.2.2.3.
///
/// In both cases each spatial beam is allocated once per polarization, so the Type II codebook sets the maximum at
/// \f$2L\f$ beams, one per combining coefficient.
static constexpr unsigned max_nof_beams_per_pmi = 2 * max_nof_typeII_beams;
static_assert(max_nof_beams_per_pmi <= precoding_constants::MAX_NOF_PORTS,
              "The composite precoding matrix cannot hold one port per beam.");

/// Constructs a precoder configuration for a single transmitter port.
precoding_weight_matrix make_single_port();

/// \brief Constructs a precoding weight matrix for one layer mapped into one transmit port.
///
/// The matrix dimensions correspond to one layer and a given number of ports. All weights are zero except for the
/// indicated port.
///
/// \param[in] nof_ports Number of ports available for transmitting.
/// \param[in] i_port    Port identifier {0, ..., \c nof_ports - 1}.
/// \return A precoding weight matrix for one layer and \c nof_ports ports.
/// \remark An assertion is triggered if \c i_port is equal to or greater than \c nof_ports.
precoding_weight_matrix make_one_layer_one_port(unsigned nof_ports, unsigned i_port);

/// \brief Constructs a precoding weight matrix for the one layer&ndash;one port mapping.
///
/// The precoding maps each layer to the port with the same identifier.
///
/// \param[in] nof_streams Number of layers and ports.
/// \return A scaled identity precoding weight matrix for \c nof_streams layers and ports. The constant value of the
/// entries in the main diagonal is computed to satisfy power-allocation requirements. All other entries are equal to
/// zero.
/// \remark An assertion is triggered if \c nof_streams is out of the range {1, ..., \ref
/// precoding_constants::MAX_NOF_LAYERS}.
precoding_weight_matrix make_identity(unsigned nof_streams);

/// \brief Constructs a precoding weight matrix for one layer mapped into all transmit ports.
///
/// All weights are set so that an identical signal is generated for each antenna port.
///
/// \param[in] nof_ports Number of ports available for transmitting.
/// \return A precoding weight matrix (actually, a column vector) for one layer and \c nof_ports.
/// \remark This precoding matrix is intended for testing purposes only.
precoding_weight_matrix make_one_layer_all_ports(unsigned nof_ports);

/// \brief Constructs a precoding weight matrix from the specified PMI for the given number of layers.
///
/// \param[in] pmi        The Precoding Matrix Indicator (PMI) codebook parameters.
/// \param[in] nof_layers The number of layers used for the transmission.
/// \return A precoding weight matrix for the given number of layers and the codebook described by the PMI.
precoding_weight_matrix make_precoding(const precoding_matrix_indicator& pmi, unsigned nof_layers);

/// \brief Derives the MIMO precoding matrix and its beam list from the specified PMI for the given number of layers.
///
/// Returns a \ref mimo_matrix, i.e., the precoding weight matrix together with the list of distinct beams it maps onto.
///
/// The weight matrix has one column per distinct beam, two for a single spatial beam, four for two. The beam list
/// contains the distinct beams in the same order as the matrix columns (one entry per beam and polarization), and
/// defines the port order for beam mapping.
///
/// All weights are normalized by the number of layers.
///
/// \param[in] pmi        Precoding Matrix Indicator (PMI) full form. Describes the compact precoding as given in CSI
///                       reports.
/// \param[in] nof_layers Number of transmission layers.
/// \return The MIMO precoding matrix and beam list derived from the PMI and the number of layers.
precoding_beamforming_composite get_mimo_matrix_from_pmi(const precoding_matrix_indicator& pmi, unsigned nof_layers);

} // namespace ocudu
