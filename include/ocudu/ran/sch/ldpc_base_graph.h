// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/support/ocudu_assert.h"
#include "ocudu/support/units.h"

namespace ocudu {

/// LDPC Base graph types.
enum class ldpc_base_graph_type : uint8_t { BG1 = 1, BG2 = 2 };

/// \brief Returns the LDPC base graph based on the given code rate and transport block size in bits, as per TS 38.212
/// section 7.2.2.
///
/// \param R Target code rate, parameter \f$R\f$.
/// \param A Size in bits of the payload \f$A\f$.
constexpr ldpc_base_graph_type get_ldpc_base_graph(float R, units::bits A)
{
  ocudu_assert(R > 0.F && R < 1.F, "Invalid target code rate {}, expected a value between 0 and 1", R);

  using namespace units::literals;
  if (A <= 292_bits || R <= 0.25F || (A <= 3824_bits && R <= 0.67F)) {
    return ldpc_base_graph_type::BG2;
  }

  return ldpc_base_graph_type::BG1;
}

} // namespace ocudu
