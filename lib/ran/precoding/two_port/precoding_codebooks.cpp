// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/ran/precoding/precoding_codebooks.h"
#include "ocudu/adt/interval.h"

using namespace ocudu;

precoding_weight_matrix ocudu::make_one_layer_two_ports(unsigned i_codebook)
{
  static constexpr interval<unsigned, true>           i_codebook_range(0, 3);
  static constexpr cf_t                               sqrt_1_2         = {M_SQRT1_2, 0};
  static constexpr cf_t                               j_sqrt_1_2       = {0, M_SQRT1_2};
  static constexpr cf_t                               minus_sqrt_1_2   = {-M_SQRT1_2, 0};
  static constexpr cf_t                               minus_j_sqrt_1_2 = {0, -M_SQRT1_2};
  static constexpr std::array<std::array<cf_t, 2>, 4> codebooks        = {
      {{sqrt_1_2, sqrt_1_2}, {sqrt_1_2, j_sqrt_1_2}, {sqrt_1_2, minus_sqrt_1_2}, {sqrt_1_2, minus_j_sqrt_1_2}}};

  ocudu_assert(i_codebook_range.contains(i_codebook),
               "The given codebook identifier (i.e., {}) is out of the range {}",
               i_codebook,
               i_codebook_range);

  precoding_weight_matrix result(1, 2);

  // Select codebook.
  span<const cf_t> codebook = codebooks[i_codebook];

  // Set weights per port.
  for (unsigned i_port = 0; i_port != 2; ++i_port) {
    result.set_coefficient(codebook[i_port], 0, i_port);
  }

  return result;
}

precoding_weight_matrix ocudu::make_two_layer_two_ports(unsigned i_codebook)
{
  static constexpr interval<unsigned, true>           i_codebook_range(0, 1);
  static constexpr cf_t                               dot_five         = {0.5F, 0.0F};
  static constexpr cf_t                               minus_dot_five   = {-0.5F, 0.0F};
  static constexpr cf_t                               j_dot_five       = {0.0F, 0.5F};
  static constexpr cf_t                               minus_j_dot_five = {0.0F, -0.5F};
  static constexpr std::array<std::array<cf_t, 2>, 2> codebook0 = {{{dot_five, dot_five}, {dot_five, minus_dot_five}}};
  static constexpr std::array<std::array<cf_t, 2>, 2> codebook1 = {
      {{dot_five, j_dot_five}, {dot_five, minus_j_dot_five}}};

  ocudu_assert(i_codebook_range.contains(i_codebook),
               "The given codebook identifier (i.e., {}) is out of the range {}",
               i_codebook,
               i_codebook_range);

  precoding_weight_matrix result(2, 2);

  // Select codebook.
  const std::array<std::array<cf_t, 2>, 2>& codebook = (i_codebook == 0) ? codebook0 : codebook1;

  // Set weights per port.
  for (unsigned i_layer = 0; i_layer != 2; ++i_layer) {
    span<const cf_t> codebook_layer = codebook[i_layer];
    for (unsigned i_port = 0; i_port != 2; ++i_port) {
      result.set_coefficient(codebook_layer[i_port], i_layer, i_port);
    }
  }

  return result;
}
