// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ldpc_encoder_neon.h"
#include "neon_support.h"
#include <algorithm>

using namespace ocudu;

template <unsigned NOF_REGISTERS>
void ldpc_encoder_neon::compute_check_node_neon(span<uint8_t> out, unsigned i_check, unsigned nof_var_nodes) const
{
  static_assert((NOF_REGISTERS >= 1) && (NOF_REGISTERS <= 3), "Only one to three registers are supported.");

  uint8x16_t sum[NOF_REGISTERS];
  std::fill_n(sum, NOF_REGISTERS, vdupq_n_u8(0));

  for_each_connected_node(i_check, nof_var_nodes, [&sum](const uint8_t* doubled_node, unsigned shift) {
    const uint8_t* data      = doubled_node + shift / 8;
    int8_t         bit_shift = static_cast<int8_t>(shift % 8);

    // Each output byte combines the trailing bits of an input byte with the leading bits of the next one. A negative
    // shift amount shifts to the right.
    int8x16_t head_shift = vdupq_n_s8(bit_shift);
    int8x16_t tail_shift = vdupq_n_s8(static_cast<int8_t>(bit_shift - 8));
    for (unsigned i_reg = 0; i_reg != NOF_REGISTERS; ++i_reg) {
      uint8x16_t head = vshlq_u8(vld1q_u8(data + i_reg * NEON_SIZE_BYTE), head_shift);
      uint8x16_t tail = vshlq_u8(vld1q_u8(data + i_reg * NEON_SIZE_BYTE + 1), tail_shift);
      sum[i_reg]      = veorq_u8(sum[i_reg], vorrq_u8(head, tail));
    }
  });

  for (unsigned i_reg = 0; i_reg != NOF_REGISTERS; ++i_reg) {
    vst1q_u8(out.data() + i_reg * NEON_SIZE_BYTE, sum[i_reg]);
  }
}

void ldpc_encoder_neon::compute_check_node(span<uint8_t> out, unsigned i_check, unsigned nof_var_nodes) const
{
  static_assert(max_node_nof_bytes <= 3 * NEON_SIZE_BYTE, "The lifted node does not fit in the stored registers.");
  ocudu_assert(out.size() >= max_node_nof_bytes, "The output size (i.e., {}) is too small.", out.size());
  if (lifting_size <= 64) {
    ldpc_encoder_generic::compute_check_node(out, i_check, nof_var_nodes);
  } else if (get_node_nof_bytes() <= NEON_SIZE_BYTE) {
    compute_check_node_neon<1>(out, i_check, nof_var_nodes);
  } else if (get_node_nof_bytes() <= 2 * NEON_SIZE_BYTE) {
    compute_check_node_neon<2>(out, i_check, nof_var_nodes);
  } else {
    compute_check_node_neon<3>(out, i_check, nof_var_nodes);
  }
}
