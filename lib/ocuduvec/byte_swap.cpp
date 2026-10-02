// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "ocudu/ocuduvec/byte_swap.h"
#include "ocudu/ocuduvec/simd.h"
#include "ocudu/support/ocudu_assert.h"

using namespace ocudu;
using namespace ocuduvec;

/// Number of bytes in one complex 16-bit sample.
static constexpr unsigned BYTES_PER_CI16 = 4;

/// Swaps the two bytes of every 16-bit word from \c in into \c out. \c nof_bytes must be even.
static void swap_bytes_16bit_simd(uint8_t* out, const uint8_t* in, unsigned nof_bytes)
{
  unsigned i = 0;

#if defined(__SSE4_1__)
  // Byte shuffle that exchanges the two bytes of each 16-bit word within a 128-bit lane.
  const __m128i swap_mask_128 = _mm_setr_epi8(1, 0, 3, 2, 5, 4, 7, 6, 9, 8, 11, 10, 13, 12, 15, 14);
#endif // defined(__SSE4_1__)

#if defined(__AVX512F__) && defined(__AVX512BW__)
  // The 512-bit shuffle works per 128-bit lane, so the lane mask is repeated in all four lanes.
  const __m512i swap_mask_512 = _mm512_broadcast_i32x4(swap_mask_128);

  // Process one 512-bit register, 16 complex samples, at a time.
  static constexpr unsigned AVX512_BLOCK_BYTES = 64;
  for (unsigned i_end = (nof_bytes / AVX512_BLOCK_BYTES) * AVX512_BLOCK_BYTES; i != i_end; i += AVX512_BLOCK_BYTES) {
    const __m512i words = _mm512_loadu_si512(reinterpret_cast<const void*>(in + i));
    _mm512_storeu_si512(reinterpret_cast<void*>(out + i), _mm512_shuffle_epi8(words, swap_mask_512));
  }
#endif // defined(__AVX512F__) && defined(__AVX512BW__)

#if defined(__AVX2__)
  // The 256-bit shuffle works per 128-bit lane, so the lane mask is repeated in both lanes.
  const __m256i swap_mask_256 = _mm256_broadcastsi128_si256(swap_mask_128);

  // Process one 256-bit register, 8 complex samples, at a time.
  static constexpr unsigned AVX2_BLOCK_BYTES = 32;
  for (unsigned i_end = (nof_bytes / AVX2_BLOCK_BYTES) * AVX2_BLOCK_BYTES; i != i_end; i += AVX2_BLOCK_BYTES) {
    const __m256i words = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(in + i));
    _mm256_storeu_si256(reinterpret_cast<__m256i*>(out + i), _mm256_shuffle_epi8(words, swap_mask_256));
  }
#endif // defined(__AVX2__)

#if defined(__SSE4_1__)
  // Process one 128-bit register, 4 complex samples, at a time.
  static constexpr unsigned SSE_BLOCK_BYTES = 16;
  for (unsigned i_end = (nof_bytes / SSE_BLOCK_BYTES) * SSE_BLOCK_BYTES; i != i_end; i += SSE_BLOCK_BYTES) {
    const __m128i words = _mm_loadu_si128(reinterpret_cast<const __m128i*>(in + i));
    _mm_storeu_si128(reinterpret_cast<__m128i*>(out + i), _mm_shuffle_epi8(words, swap_mask_128));
  }
#endif // defined(__SSE4_1__)

#if defined(__ARM_NEON)
  // Process one 128-bit register, 4 complex samples, at a time.
  static constexpr unsigned NEON_BLOCK_BYTES = 16;
  for (unsigned i_end = (nof_bytes / NEON_BLOCK_BYTES) * NEON_BLOCK_BYTES; i != i_end; i += NEON_BLOCK_BYTES) {
    vst1q_u8(out + i, vrev16q_u8(vld1q_u8(in + i)));
  }
#endif // defined(__ARM_NEON)

  // Remaining 16-bit words.
  static constexpr unsigned WORD_BYTES = 2;
  for (; i != nof_bytes; i += WORD_BYTES) {
    const uint8_t low = in[i];
    out[i]            = in[i + 1];
    out[i + 1]        = low;
  }
}

void ocudu::ocuduvec::swap_bytes(span<uint8_t> out, span<const ci16_t> in)
{
  ocudu_assert(out.size() == BYTES_PER_CI16 * in.size(),
               "Output size (i.e., {}) must be {} bytes per input sample (i.e., {}).",
               out.size(),
               BYTES_PER_CI16,
               in.size());

  swap_bytes_16bit_simd(out.data(), reinterpret_cast<const uint8_t*>(in.data()), out.size());
}

void ocudu::ocuduvec::swap_bytes(span<ci16_t> out, span<const uint8_t> in)
{
  ocudu_assert(in.size() == BYTES_PER_CI16 * out.size(),
               "Input size (i.e., {}) must be {} bytes per output sample (i.e., {}).",
               in.size(),
               BYTES_PER_CI16,
               out.size());

  swap_bytes_16bit_simd(reinterpret_cast<uint8_t*>(out.data()), in.data(), in.size());
}
