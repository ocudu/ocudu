// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

/// \file
/// \brief LDPC encoder - Declarations for AVX2-optimized version.

#pragma once

#include "ldpc_encoder_generic.h"

namespace ocudu {

/// LDPC encoder implementation based on AVX2 intrinsics.
class ldpc_encoder_avx2 : public ldpc_encoder_generic
{
private:
  // See ldpc_encoder_generic for documentation.
  void compute_check_node(span<uint8_t> out, unsigned i_check, unsigned nof_var_nodes) const override;

  /// Computes a check node whose packed lifted bits fit in \c NOF_REGISTERS AVX2 registers.
  template <unsigned NOF_REGISTERS>
  void compute_check_node_avx2(span<uint8_t> out, unsigned i_check, unsigned nof_var_nodes) const;
};

} // namespace ocudu
