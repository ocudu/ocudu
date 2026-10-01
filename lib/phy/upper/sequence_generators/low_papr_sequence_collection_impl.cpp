// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "low_papr_sequence_collection_impl.h"
#include "ocudu/adt/format.h"
#include "ocudu/ran/resource_block.h"
#include "ocudu/support/math/math_utils.h"

using namespace ocudu;

low_papr_sequence_collection_impl::low_papr_sequence_collection_impl(low_papr_sequence_generator& generator,
                                                                     unsigned                     m,
                                                                     unsigned                     delta,
                                                                     span<const float>            alphas)

{
  // Calculate sequence length.
  unsigned M_zc = (NOF_SUBCARRIERS_PER_RB * m) / pow2(delta);

  // Calculate number of bases.
  unsigned nof_bases = MAX_NOF_BASES;
  if ((NOF_SUBCARRIERS_PER_RB / 2) <= M_zc && M_zc <= (5 * NOF_SUBCARRIERS_PER_RB)) {
    nof_bases = 1;
  }

  unsigned nof_alphas = alphas.size();

  pregen_signals.reserve({M_zc, NOF_GROUPS, nof_bases, nof_alphas});

  // Create sequence collection for all possible of u, v and alpha index values.
  for (unsigned u = 0; u != NOF_GROUPS; ++u) {
    for (unsigned v = 0; v != nof_bases; ++v) {
      for (unsigned alpha_idx = 0; alpha_idx != nof_alphas; ++alpha_idx) {
        // Generate a sequence for each alpha.
        generator.generate(pregen_signals.get_view<1>({u, v, alpha_idx}), u, v, alpha_idx, nof_alphas);
      }
    }
  }
}

span<const cf_t> low_papr_sequence_collection_impl::get(unsigned u, unsigned v, unsigned alpha_idx) const
{
  std::array<unsigned, 4> dimensions = pregen_signals.get_dimensions_size();
  std::array<unsigned, 3> indexes    = {u, v, alpha_idx};

  // Make sure the indexes are valid.
  ocudu_assert(u < dimensions[1] && v < dimensions[2] && alpha_idx < dimensions[3],
               "The sequence collection was initialized with dimension sizes {} and it is trying to access {}",
               span<const unsigned>(dimensions),
               span<const unsigned>(indexes));

  // Return the vector view of the sequence.
  return pregen_signals.get_view<1>({u, v, alpha_idx});
}
