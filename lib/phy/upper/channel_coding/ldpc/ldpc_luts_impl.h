// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

/// \file
/// \brief Lookup tables for ocudu::ldpc_impl -- access function declarations.
#pragma once

#include "ldpc_graph_impl.h"
#include "ocudu/phy/upper/channel_coding/ldpc/ldpc.h"

namespace ocudu {
namespace ldpc {
/// \brief Get the LDPC Tanner graph corresponding to the given base graph and lifting size.
///
/// \param[in] bg Desired base graph.
/// \param[in] ls Desired lifting size.
/// \returns A compact parity check matrix: the value taken by entry \f$(m, n)\f$ represents the shift
///          applied when lifting the edge between check node \f$m\f$ and variable node \f$n\f$ of the
///          base graph.
BG_matrix_t get_graph(ldpc_base_graph_type bg, lifting_size_t ls);

/// Get the lifting size index for a given lifting size.
uint8_t get_lifting_index(lifting_size_t ls);

/// Get the position of the lifting size in the list of all lifting sizes.
uint8_t get_lifting_size_position(lifting_size_t ls);

/// Get the adjacency matrix of base graph \c bg (sparse representation).
const BG_adjacency_matrix_t* get_adjacency_matrix(ldpc_base_graph_type bg);
} // namespace ldpc
} // namespace ocudu
