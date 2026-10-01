// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

/// \file
/// \brief LDPC rate matcher declaration.

#pragma once

#include "ldpc_graph_impl.h"
#include "ocudu/phy/upper/channel_coding/ldpc/ldpc_encoder_buffer.h"
#include "ocudu/phy/upper/channel_coding/ldpc/ldpc_rate_matcher.h"

namespace ocudu {

/// LDPC rate matching implementation, as per TS38.212 Section 5.4.2.
class ldpc_rate_matcher_impl : public ldpc_rate_matcher
{
public:
  // See interface for the documentation.
  void rate_match(bit_buffer& output, const ldpc_encoder_buffer& input, const codeblock_metadata& cfg) override;

private:
  /// Initializes the rate matcher internal state.
  void init(const codeblock_metadata& cfg, unsigned block_length, unsigned rm_length);

  /// \brief Carries out bit selection, as per TS38.212 Section 5.4.2.1.
  ///
  /// \param[out] out Sequence of selected bits.
  /// \param[in]  in  Input encoded codeblock.
  void select_bits(span<uint8_t> out, const ldpc_encoder_buffer& in) const;

  /// \brief Carries out bit interleaving, as per TS38.212 Section 5.4.2.2.
  ///
  /// \param[out] out Sequence of interleaved bits.
  /// \param[in]  in  Sequence of selected bits (see ldpc_rate_matcher_impl::select_bits).
  void interleave_bits(bit_buffer& out, span<const uint8_t> in) const;

  // Data members

  /// Auxiliary buffer.
  std::array<uint8_t, ldpc::MAX_CODEBLOCK_RM_SIZE> auxiliary_buffer;
  /// Redundancy version, values in {0, 1, 2, 3}.
  unsigned rv = 0;
  /// Modulation scheme.
  unsigned modulation_order = 1;
  /// Base graph.
  ldpc_base_graph_type base_graph = ldpc_base_graph_type::BG1;
  /// Number of systematic bits (including filler bits) in the codeblock.
  unsigned nof_systematic_bits = 0;
  /// Number of filler bits.
  unsigned nof_filler_bits = 0;
  /// Buffer length.
  unsigned buffer_length = 0;
  /// Shift \f$ k_0 \f$ as defined in TS38.212 Table 5.4.2.1-2
  unsigned shift_k0 = 0;
};

} // namespace ocudu
