// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ldpc_encoder_avx2.h"
#include "avx2_support.h"

using namespace ocudu;

template <unsigned NOF_REGISTERS>
void ldpc_encoder_avx2::compute_check_node_avx2(span<uint8_t> out, unsigned i_check, unsigned nof_var_nodes) const
{
  __m256i sum[NOF_REGISTERS];
  std::fill_n(sum, NOF_REGISTERS, _mm256_setzero_si256());

  for_each_connected_node(i_check, nof_var_nodes, [&sum](const uint8_t* doubled_node, unsigned shift) {
    const uint8_t* data      = doubled_node + shift / 8;
    unsigned       bit_shift = shift % 8;

    // Each output byte combines the trailing bits of an input byte with the leading bits of the next one.
    __m256i head_mask = _mm256_set1_epi8(static_cast<int8_t>(0xff << bit_shift));
    for (unsigned i_reg = 0; i_reg != NOF_REGISTERS; ++i_reg) {
      __m256i head = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(data + i_reg * AVX2_SIZE_BYTE));
      __m256i tail = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(data + i_reg * AVX2_SIZE_BYTE + 1));
      head         = _mm256_and_si256(_mm256_slli_epi16(head, bit_shift), head_mask);
      tail         = _mm256_andnot_si256(head_mask, _mm256_srli_epi16(tail, 8 - bit_shift));
      sum[i_reg]   = _mm256_xor_si256(sum[i_reg], _mm256_or_si256(head, tail));
    }
  });

  // The second register holds at most the last 16 bytes of the largest lifting size.
  _mm256_storeu_si256(reinterpret_cast<__m256i*>(out.data()), sum[0]);
  if constexpr (NOF_REGISTERS == 2) {
    _mm_storeu_si128(reinterpret_cast<__m128i*>(out.data() + AVX2_SIZE_BYTE), _mm256_castsi256_si128(sum[1]));
  }
}

void ldpc_encoder_avx2::compute_check_node(span<uint8_t> out, unsigned i_check, unsigned nof_var_nodes) const
{
  static_assert(max_node_nof_bytes <= AVX2_SIZE_BYTE + 16, "The lifted node does not fit in the stored registers.");
  ocudu_assert(out.size() >= max_node_nof_bytes, "The output size (i.e., {}) is too small.", out.size());
  if (lifting_size <= 64) {
    ldpc_encoder_generic::compute_check_node(out, i_check, nof_var_nodes);
  } else if (get_node_nof_bytes() <= AVX2_SIZE_BYTE) {
    compute_check_node_avx2<1>(out, i_check, nof_var_nodes);
  } else {
    compute_check_node_avx2<2>(out, i_check, nof_var_nodes);
  }
}
