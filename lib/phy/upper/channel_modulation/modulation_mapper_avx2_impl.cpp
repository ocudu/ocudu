// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "modulation_mapper_avx2_impl.h"
#include "ocudu/support/math/math_utils.h"
#include <immintrin.h>

using namespace ocudu;

/// \brief Implements a generic QAM modulator using AVX2 instruction sets.
///
/// It modulates 32 symbols in every call. Each byte in \c input corresponds to a symbol.
template <unsigned QM>
void generic_modulator(ci8_t* output, __m256i input)
{
  __m256i offset = _mm256_set1_epi8(-1);
  __m256i real   = _mm256_setzero_si256();
  __m256i imag   = _mm256_setzero_si256();

  for (unsigned j = 0, j_end = QM / 2; j != j_end; ++j) {
    real   = _mm256_add_epi8(real, offset);
    imag   = _mm256_add_epi8(imag, offset);
    offset = _mm256_add_epi8(offset, offset);

    __m256i real_mask =
        _mm256_cmpeq_epi8(_mm256_and_si256(_mm256_set1_epi8((1U << (2U * j + 1U))), input), _mm256_setzero_si256());
    __m256i imag_mask =
        _mm256_cmpeq_epi8(_mm256_and_si256(_mm256_set1_epi8((1U << (2U * j + 0U))), input), _mm256_setzero_si256());

    __m256i real_n = _mm256_sub_epi8(_mm256_setzero_si256(), real);
    __m256i imag_n = _mm256_sub_epi8(_mm256_setzero_si256(), imag);

    real = _mm256_blendv_epi8(real, real_n, real_mask);
    imag = _mm256_blendv_epi8(imag, imag_n, imag_mask);
  }

  // Interleave I and Q components into ci8_t stream.
  __m256i interleaved_lo = _mm256_unpacklo_epi8(real, imag);
  __m256i interleaved_hi = _mm256_unpackhi_epi8(real, imag);

  __m256i out0 = _mm256_permute2x128_si256(interleaved_lo, interleaved_hi, 0x20);
  __m256i out1 = _mm256_permute2x128_si256(interleaved_lo, interleaved_hi, 0x31);

  _mm256_storeu_si256(reinterpret_cast<__m256i*>(output), out0);
  _mm256_storeu_si256(reinterpret_cast<__m256i*>(output + 16), out1);
}

/// \brief Implements a QAM modulator for 16-bit symbol indices using AVX2 instruction sets.
///
/// It modulates 16 symbols in every call. Each 16-bit lane in \c input corresponds to a symbol. The real part is
/// computed in the low byte and the imaginary part in the high byte of each lane, so the result is directly laid out
/// as 16 consecutive \c ci8_t symbols.
///
/// Only modulation orders above 256-QAM are supported (QM > 8). Lower orders are handled by \c generic_modulator and
/// other paths in this mapper.
///
/// \tparam QM Modulation order (must be greater than 8, even, and at most 14).
/// \param[in] input Data to modulate. Only the QM least significant bits of each 16-bit lane contain information.
/// \return The 16 modulated symbols.
template <unsigned QM>
__m256i generic_modulator_epi16(__m256i input)
{
  static_assert((QM > 8) && (QM % 2 == 0) && (QM <= 14),
                "generic_modulator_epi16 is only for orders above 256-QAM; amplitude must fit in a signed 8-bit "
                "integer.");

  // A component is negated when its selecting bit is zero, so work with the inverted input.
  __m256i ninput        = _mm256_xor_si256(input, _mm256_set1_epi8(-1));
  __m256i low_byte_mask = _mm256_set1_epi16(0x00ff);
  __m256i one           = _mm256_set1_epi8(1);

  // Keep the real selector bits (odd positions) of the low byte in place and move the imaginary selector bits (even
  // positions) of the low byte to the high byte, nine positions up. For the four inner amplitude levels, a left shift
  // by 6 - 2j places the real selector bit in the sign of the low byte and the imaginary selector bit in the sign of
  // the high byte.
  __m256i selector = _mm256_or_si256(_mm256_and_si256(ninput, low_byte_mask), _mm256_slli_epi16(ninput, 9));

  // Modulate the lowest 8 bits.
  __m256i symbols = _mm256_setzero_si256();
  for (unsigned j = 0; j != 4; ++j) {
    __m256i sign = _mm256_slli_epi16(selector, 6 - 2 * j);
    // Only the sign bit is relevant. Setting the LSB prevents the sign operation from zeroing the component.
    sign = _mm256_or_si256(sign, one);

    symbols = _mm256_add_epi8(symbols, _mm256_set1_epi8(static_cast<int8_t>(-(1 << j))));
    symbols = _mm256_sign_epi8(symbols, sign);
  }

  // Modulate the highest QM-8 bits.
  for (unsigned j = 4, j_end = QM / 2; j != j_end; ++j) {
    __m256i sign =
        _mm256_blendv_epi8(_mm256_slli_epi16(ninput, 15 - 2 * j), _mm256_srli_epi16(ninput, 2 * j - 6), low_byte_mask);
    sign = _mm256_or_si256(sign, one);

    symbols = _mm256_add_epi8(symbols, _mm256_set1_epi8(static_cast<int8_t>(-(1 << j))));
    symbols = _mm256_sign_epi8(symbols, sign);
  }

  return symbols;
}

inline __m256i load_qam64_symbols(const uint8_t* input_ptr)
{
  static constexpr int8_t bswap_idx1_data[16] = {1, 0, 4, 3, 7, 6, 10, 9, 13, 12, -1, -1, -1, -1, -1, -1};
  static constexpr int8_t bswap_idx2_data[16] = {2, 1, 5, 4, 8, 7, 11, 10, 14, 13, -1, -1, -1, -1, -1, -1};
  static constexpr int8_t sel_shift2_data[16] = {0, -1, -1, -1, 3, -1, -1, -1, 6, -1, -1, -1, 9, -1, -1, -1};
  static constexpr int8_t sel_shift4_data[16] = {-1, 0, -1, -1, -1, 2, -1, -1, -1, 4, -1, -1, -1, 6, -1, -1};
  static constexpr int8_t sel_shift6_data[16] = {-1, -1, 0, -1, -1, -1, 2, -1, -1, -1, 4, -1, -1, -1, 6, -1};
  static constexpr int8_t sel_input_data[16]  = {-1, -1, -1, 2, -1, -1, -1, 5, -1, -1, -1, 8, -1, -1, -1, 11};

  __m256i bswap_idx1 = _mm256_broadcastsi128_si256(_mm_loadu_si128(reinterpret_cast<const __m128i*>(bswap_idx1_data)));
  __m256i bswap_idx2 = _mm256_broadcastsi128_si256(_mm_loadu_si128(reinterpret_cast<const __m128i*>(bswap_idx2_data)));
  __m256i sel_shift2 = _mm256_broadcastsi128_si256(_mm_loadu_si128(reinterpret_cast<const __m128i*>(sel_shift2_data)));
  __m256i sel_shift4 = _mm256_broadcastsi128_si256(_mm_loadu_si128(reinterpret_cast<const __m128i*>(sel_shift4_data)));
  __m256i sel_shift6 = _mm256_broadcastsi128_si256(_mm_loadu_si128(reinterpret_cast<const __m128i*>(sel_shift6_data)));
  __m256i sel_input  = _mm256_broadcastsi128_si256(_mm_loadu_si128(reinterpret_cast<const __m128i*>(sel_input_data)));

  // 24 input bytes encode 32 QAM64 symbols. Build two 12-byte lanes padded with zeros.
  static constexpr int8_t keep_low_12_data[16] = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 0, 0, 0, 0};
  __m128i                 keep_low_12          = _mm_loadu_si128(reinterpret_cast<const __m128i*>(keep_low_12_data));

  __m128i in0 = _mm_and_si128(_mm_loadu_si128(reinterpret_cast<const __m128i*>(input_ptr)), keep_low_12);
  __m128i in1 = _mm_srli_si128(_mm_loadu_si128(reinterpret_cast<const __m128i*>(input_ptr + 8)), 4);

  __m256i in = _mm256_castsi128_si256(in0);
  in         = _mm256_inserti128_si256(in, in1, 1);

  __m256i shift2 = _mm256_srli_epi64(in, 2);
  __m256i shift4 = _mm256_srli_epi16(_mm256_shuffle_epi8(in, bswap_idx1), 4);
  __m256i shift6 = _mm256_srli_epi16(_mm256_shuffle_epi8(in, bswap_idx2), 6);

  __m256i out = _mm256_or_si256(_mm256_shuffle_epi8(shift2, sel_shift2), _mm256_shuffle_epi8(shift4, sel_shift4));
  out         = _mm256_or_si256(out, _mm256_shuffle_epi8(shift6, sel_shift6));
  out         = _mm256_or_si256(out, _mm256_shuffle_epi8(in, sel_input));

  return _mm256_and_si256(out, _mm256_set1_epi8(0x3f));
}

/// \brief Distributes the bits of 16 1024-QAM symbols onto 16-bit lanes.
///
/// It reads 26 bytes from \c input_ptr, of which only the first 20 contain the symbols.
inline __m256i load_qam1024_symbols(const uint8_t* input_ptr)
{
  // Every four symbols span five bytes. Each 16-bit lane selects the two bytes containing the bits of its symbol, in
  // little-endian order.
  static constexpr int8_t gather_idx_data[16] = {1, 0, 2, 1, 3, 2, 4, 3, 6, 5, 7, 6, 8, 7, 9, 8};
  __m256i gather_idx = _mm256_broadcastsi128_si256(_mm_loadu_si128(reinterpret_cast<const __m128i*>(gather_idx_data)));

  // Each 128-bit lane takes the ten bytes of eight symbols.
  __m256i in = _mm256_castsi128_si256(_mm_loadu_si128(reinterpret_cast<const __m128i*>(input_ptr)));
  in         = _mm256_inserti128_si256(in, _mm_loadu_si128(reinterpret_cast<const __m128i*>(input_ptr + 10)), 1);
  in         = _mm256_shuffle_epi8(in, gather_idx);

  // Shift the symbol bits to the MSB of every lane, discarding the bits of the previous symbol, and then align them to
  // the LSB. The multiplication by a power of two implements a per-lane left shift by 0, 2, 4 and 6.
  in = _mm256_mullo_epi16(in, _mm256_setr_epi16(1, 4, 16, 64, 1, 4, 16, 64, 1, 4, 16, 64, 1, 4, 16, 64));
  return _mm256_srli_epi16(in, 6);
}

float modulation_mapper_avx2_impl::modulate_qam64(span<ci8_t> symbols, const bit_buffer& input)
{
  const uint8_t* input_ptr = input.get_buffer().data();
  unsigned       i_symbol  = 0;

  for (unsigned i_symbol_end = (symbols.size() / 32) * 32; i_symbol != i_symbol_end; i_symbol += 32) {
    __m256i avx_in = load_qam64_symbols(input_ptr);
    generic_modulator<6>(symbols.data() + i_symbol, avx_in);
    input_ptr += (32 * 6) / 8;
  }

  unsigned remainder = symbols.size() - i_symbol;
  return lut_modulator.modulate(symbols.last(remainder), input.last(6 * remainder), modulation_scheme::QAM64);
}

float modulation_mapper_avx2_impl::modulate_qam256(span<ci8_t> symbols, const bit_buffer& input)
{
  for (unsigned i_symbol = 0, i_symbol_end = (symbols.size() / 32) * 32; i_symbol != i_symbol_end; i_symbol += 32) {
    __m256i avx_in = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(input.get_buffer().data() + i_symbol));
    generic_modulator<8>(symbols.data() + i_symbol, avx_in);
  }

  unsigned remainder = symbols.size() % 32;
  return lut_modulator.modulate(symbols.last(remainder), input.last(8 * remainder), modulation_scheme::QAM256);
}

float modulation_mapper_avx2_impl::modulate_qam1024(span<ci8_t> symbols, const bit_buffer& input)
{
  // Number of symbols modulated per block.
  static constexpr unsigned block_size = 16;
  // Number of input bytes consumed per block.
  static constexpr unsigned block_nof_bytes = (block_size * 10) / 8;
  // Number of bytes read per block.
  static constexpr unsigned block_nof_read_bytes = 10 + 16;
  // Minimum number of remaining symbols for which reading a block stays within the input bounds.
  static constexpr unsigned min_nof_symbols_safe_read = divide_ceil(block_nof_read_bytes * 8, 10);

  const uint8_t* input_ptr   = input.get_buffer().data();
  ci8_t*         symbols_ptr = symbols.data();
  unsigned       i_symbol    = 0;
  for (unsigned nof_symbols = symbols.size(); nof_symbols - i_symbol >= min_nof_symbols_safe_read;
       i_symbol += block_size, input_ptr += block_nof_bytes) {
    __m256i out = generic_modulator_epi16<10>(load_qam1024_symbols(input_ptr));
    _mm256_storeu_si256(reinterpret_cast<__m256i*>(symbols_ptr + i_symbol), out);
  }

  unsigned remainder = symbols.size() - i_symbol;
  return lut_modulator.modulate(symbols.last(remainder), input.last(10 * remainder), modulation_scheme::QAM1024);
}

void modulation_mapper_avx2_impl::modulate(span<cf_t> symbols, const bit_buffer& input, modulation_scheme scheme)
{
  return lut_modulator.modulate(symbols, input, scheme);
}

float modulation_mapper_avx2_impl::modulate(span<ci8_t> symbols, const bit_buffer& input, modulation_scheme scheme)
{
  if (scheme == modulation_scheme::QAM1024) {
    return modulate_qam1024(symbols, input);
  }

  if (scheme == modulation_scheme::QAM256) {
    return modulate_qam256(symbols, input);
  }

  if (scheme == modulation_scheme::QAM64) {
    return modulate_qam64(symbols, input);
  }

  return lut_modulator.modulate(symbols, input, scheme);
}
