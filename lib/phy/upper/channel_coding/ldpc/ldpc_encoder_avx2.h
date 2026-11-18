// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

/// \file
/// \brief LDPC encoder - Declarations for AVX2-optimized version.

#pragma once

#include "ldpc_encoder_generic.h"

namespace ocudu {

/// \brief LDPC encoder implementation based on AVX2 intrinsics.
///
/// Inherits the packed-bit storage and encoding steps of \c ldpc_encoder_generic and accelerates only the check-node
/// computation. Lifting sizes up to 64 use the generic path. Larger lifting sizes accumulate the shifted variable
/// nodes in one AVX2 register, or two when the packed node exceeds 32 bytes.
class ldpc_encoder_avx2 : public ldpc_encoder_generic
{
private:
  // See ldpc_encoder_generic for documentation.
  void compute_check_node(span<uint8_t> out, unsigned i_check, unsigned nof_var_nodes) const override;

  /// \brief Computes the sum of the variable nodes connected to a check node, modulo 2, with AVX2 registers.
  ///
  /// Each connected variable node is read from its doubled storage at the byte offset of its circular shift. The
  /// remaining bit shift combines every byte with the next one, and the result is XOR-accumulated in
  /// \c NOF_REGISTERS registers.
  ///
  /// \tparam     NOF_REGISTERS Number of AVX2 registers holding the packed lifted node, either 1 or 2.
  /// \param[out] out           Packed sum, with room for \c max_node_nof_bytes bytes. The bits beyond the lifting size
  ///                           are left undefined.
  /// \param[in]  i_check       Check node index.
  /// \param[in]  nof_var_nodes Number of variable nodes considered, starting from the first one.
  /// \remark The loads read up to <tt>32 * NOF_REGISTERS + 1</tt> bytes past the shifted start of each doubled node.
  /// The padding of the doubled node storage covers them.
  template <unsigned NOF_REGISTERS>
  void compute_check_node_avx2(span<uint8_t> out, unsigned i_check, unsigned nof_var_nodes) const;
};

} // namespace ocudu
