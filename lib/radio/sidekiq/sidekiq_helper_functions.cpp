// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "sidekiq_helper_functions.h"

#if defined(__SSE3__) && defined(__AVX__) && defined(__AVX2__)
#define HAVE_AVX
#include <immintrin.h>
#endif // defined(__SSE3__) && defined(__AVX__) && defined(__AVX2__)

using namespace ocudu;

void ocudu::convert_i12_to_ci16(span<ci16_t> out, span<const uint32_t> in)
{
  ocudu_assert(in.size() % 3 == 0, "The number of packed words (i.e., {}) must be multiple of 3.", in.size());
  ocudu_assert(out.size() == (in.size() / 3) * 4,
               "The out size (i.e., {}) and in size (i.e., {}) are not consistent.",
               out.size(),
               in.size());

  for (unsigned i_word = 0, i_sample = 0, end = in.size(); i_word != end; i_word += 3, i_sample += 4) {
#ifdef HAVE_AVX
    // Load three 32-bit words elements into a 128-bit vector register.
    __m128i input_vec = _mm_setzero_si128();
    input_vec         = _mm_insert_epi32(input_vec, in[i_word + 0], 0);
    input_vec         = _mm_insert_epi32(input_vec, in[i_word + 1], 1);
    input_vec         = _mm_insert_epi32(input_vec, in[i_word + 2], 2);

    // Extract in-phase values, they are aligned to the MSB.
    __m128i i16_high =
        _mm_shuffle_epi8(input_vec, _mm_setr_epi8(2, 3, -1, -1, 7, 0, -1, -1, 4, 5, -1, -1, 9, 10, -1, -1));

    // Extract quadrature values, they are aligned to the LSB.
    __m128i i16_low =
        _mm_shuffle_epi8(input_vec, _mm_setr_epi8(-1, -1, 1, 2, -1, -1, 6, 7, -1, -1, 11, 4, -1, -1, 8, 9));

    // Shift the quadrature part for aligning the first bit to the MSB.
    i16_low = _mm_slli_epi16(i16_low, 4);

    // Combine in-phase and quadrature.
    input_vec = _mm_or_si128(i16_high, i16_low);
    input_vec = _mm_and_si128(input_vec, _mm_set1_epi32(static_cast<int32_t>(0xfff0fff0U)));

    // Store the result back to memory.
    _mm_storeu_si128(reinterpret_cast<__m128i_u*>(&out[i_sample]), input_vec);
#else  // HAVE_AVX
    // Extract words.
    uint32_t word0  = in[i_word + 0];
    uint32_t word1  = in[i_word + 1];
    uint32_t word2  = in[i_word + 2];
    uint64_t word01 = (static_cast<uint64_t>(word0) << 32U) | static_cast<uint64_t>(word1);
    uint64_t word12 = (static_cast<uint64_t>(word1) << 32U) | static_cast<uint64_t>(word2);

    // Extract int16 values.
    std::array<int16_t, 8> iq_values;
    iq_values[0] = static_cast<int16_t>((word0 >> 16U) & 0xfff0);
    iq_values[1] = static_cast<int16_t>((word0 >> 4U) & 0xfff0);
    iq_values[2] = static_cast<int16_t>((word01 >> 24U) & 0xfff0);
    iq_values[3] = static_cast<int16_t>((word1 >> 12U) & 0xfff0);
    iq_values[4] = static_cast<int16_t>((word1 >> 0U) & 0xfff0);
    iq_values[5] = static_cast<int16_t>((word12 >> 20U) & 0xfff0);
    iq_values[6] = static_cast<int16_t>((word2 >> 8U) & 0xfff0);
    iq_values[7] = static_cast<int16_t>((word2 << 4U) & 0xfff0);

    // Store as ci16_t complex int16 pairs (real, imag).
    for (unsigned i = 0; i < 4; ++i) {
      out[i_sample + i] = ci16_t{iq_values[2 * i], iq_values[2 * i + 1]};
    }
#endif // HAVE_AVX
  }
}

void ocudu::convert_ci16_to_i12(span<uint32_t> out, span<const ci16_t> in)
{
  ocudu_assert(out.size() % 3 == 0, "The number of packed words (i.e., {}) must be multiple of 3.", out.size());
  ocudu_assert(in.size() == (out.size() / 3) * 4,
               "The out size (i.e., {}) and in size (i.e., {}) are not consistent.",
               out.size(),
               in.size());

  for (unsigned i_word = 0, i_sample = 0, end = out.size(); i_word != end; i_word += 3, i_sample += 4) {
#ifdef HAVE_AVX
    // Load input.
    __m128i i16_vec = _mm_loadu_si128(reinterpret_cast<const __m128i_u*>(in.data() + i_sample));

    // Drop the 4 least significant bits.
    i16_vec = _mm_and_si128(i16_vec, _mm_set1_epi16(0xfff0));

    // 4-bit shifted registers.
    __m128i shifted = _mm_srli_epi16(i16_vec, 4);

    // Pack eight 16-bit words in three 32-bit words.
    __m128i i16_low =
        _mm_shuffle_epi8(i16_vec, _mm_setr_epi8(5, -1, 0, 1, 8, 9, -1, 4, -1, 12, 13, -1, -1, -1, -1, -1));
    __m128i i16_high =
        _mm_shuffle_epi8(shifted, _mm_setr_epi8(-1, 2, 3, -1, 11, -1, 6, 7, 14, 15, -1, 10, -1, -1, -1, -1));
    __m128i combined = _mm_or_si128(i16_low, i16_high);

    // Extract 32-bit words.
    out[i_word + 0] = _mm_extract_epi32(combined, 0);
    out[i_word + 1] = _mm_extract_epi32(combined, 1);
    out[i_word + 2] = _mm_extract_epi32(combined, 2);
#else  // HAVE_AVX
    // Convert cf to integer.
    const int16_t*         src = reinterpret_cast<const int16_t*>(&in.front());
    std::array<int32_t, 8> iq_values;
    for (unsigned i = 0; i != 8; ++i) {
      iq_values[i] = (static_cast<int32_t>(src[2 * i_sample + i]) >> 4) & 0xfffU;
    }

    // Pack.
    out[i_word + 0] = (iq_values[0] << 20U) | (iq_values[1] << 8U) | (iq_values[2] >> 4U);
    out[i_word + 1] = (iq_values[2] << 28U) | (iq_values[3] << 16U) | (iq_values[4] << 4U) | (iq_values[5] >> 8U);
    out[i_word + 2] = (iq_values[5] << 24U) | (iq_values[6] << 12U) | (iq_values[7]);
#endif // HAVE_AVX
  }
}
