// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

/// \file
/// \brief LDPC encoder interface.

#pragma once

#include "ocudu/adt/span.h"
#include "ocudu/phy/upper/codeblock_metadata.h"

namespace ocudu {

class ldpc_encoder_buffer;

/// LDPC encoder interface.
class ldpc_encoder
{
public:
  /// Default destructor.
  virtual ~ldpc_encoder() = default;

  /// Collects the necessary parameters for encoding an LDPC codeblock.
  struct configuration {
    /// Code base graph.
    ldpc_base_graph_type base_graph = ldpc_base_graph_type::BG1;
    /// Code lifting size.
    ldpc::lifting_size_t lifting_size = ldpc::LS2;
    /// \brief Limited buffer rate matching length, as per TS38.212 Section 5.4.2.
    /// \note Set to zero for unlimited buffer length.
    unsigned Nref = 0;
  };

  /// \brief Encodes a message.
  ///
  /// \param[in]  input   Message: original information bits, with the filler bits (if any) set to zero.
  /// \param[in]  cfg     Encoder configuration for the current codeblock.
  /// \return A reference to the LDPC encoder buffer.
  /// \note The length of the output codeblock is deduced from the size of parameter \c output.
  virtual const ldpc_encoder_buffer& encode(const bit_buffer& input, const configuration& cfg) = 0;
};

} // namespace ocudu
