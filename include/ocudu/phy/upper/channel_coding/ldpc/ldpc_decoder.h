// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

/// \file
/// \brief LDPC decoder interface.

#pragma once

#include "ocudu/adt/span.h"
#include "ocudu/phy/upper/channel_coding/crc_calculator.h"
#include "ocudu/phy/upper/codeblock_metadata.h"
#include "ocudu/phy/upper/log_likelihood_ratio.h"
#include <optional>

namespace ocudu {

/// LDPC decoder interface.
class ldpc_decoder
{
public:
  /// Default destructor.
  virtual ~ldpc_decoder() = default;

  /// Decoder configuration.
  struct configuration {
    /// Code base graph.
    ldpc_base_graph_type base_graph = ldpc_base_graph_type::BG1;
    /// Code lifting size.
    ldpc::lifting_size_t lifting_size = ldpc::LS2;
    /// Number of filler bits in the full codeblock.
    unsigned nof_filler_bits = 0;
    /// CRC bits
    unsigned nof_crc_bits = 16;
    /// Maximum number of iterations.
    unsigned max_iterations = 6;
  };

  /// \brief Decodes a codeblock.
  ///
  /// By passing a CRC calculator, the CRC is verified after each iteration allowing, when successful, an early stop of
  /// the decoding process.
  ///
  /// The decoding is successful if:
  /// - An early stop mechanism condition is met (CRC or syndrome check); or
  /// - No early stop mechanism is configured and syndrome check is successful after completing all iterations.
  ///
  /// \param[out] output  Reconstructed message of information bits.
  /// \param[in]  input   Log-likelihood ratios of the codeblock to be decoded.
  /// \param[in]  crc     Pointer to a CRC calculator for early stopping. Set to \c nullptr for disabling early
  ///                     stopping.
  /// \param[in]  cfg     Decoder configuration.
  /// \return If the decoding is successful, returns the number of LDPC iterations needed by the decoder. Otherwise, no
  ///         value is returned.
  /// \note A codeblock of all zero-valued log-likelihood ratios will automatically return an empty value (i.e., failed
  /// CRC) and set all the output bits to one.
  virtual std::optional<unsigned>
  decode(bit_buffer& output, span<const log_likelihood_ratio> input, crc_calculator* crc, const configuration& cfg) = 0;
};

} // namespace ocudu
