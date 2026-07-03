// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "ocudu/ocuduvec/bit.h"
#include "ocudu/ocuduvec/copy.h"
#include "ocudu/ocuduvec/zero.h"
#include "ocudu/support/math/math_utils.h"
#include "ocudu/support/ocudu_assert.h"

#ifdef __x86_64__
#include <immintrin.h>
#endif // __x86_64__

using namespace ocudu;
using namespace ocuduvec;

template <typename InType = uint8_t>
static void unpack_8bit(span<uint8_t> unpacked, InType value)
{
  ocudu_assert(unpacked.size() == 8, "The amount of data to pack (i.e., {}) must be eight.", unpacked.size());

  // Unpack a byte by copying each bit into a byte MSB within a 64-bit register.
  uint64_t unpacked_ = ((static_cast<uint64_t>(value) * 0x8040201008040201) & 0x8080808080808080) >> 7UL;

  // Store the unpacked data.
  std::memcpy(unpacked.data(), &unpacked_, sizeof(uint64_t));
}

template <typename RetType = uint8_t>
static RetType pack_8bit(span<const uint8_t> unpacked)
{
  ocudu_assert(unpacked.size() == 8, "The amount of data to pack (i.e., {}) must be eight.", unpacked.size());

  // Load unpacked data.
  uint64_t unpacked_ = 0;
  std::memcpy(&unpacked_, unpacked.data(), sizeof(uint64_t));

  // Mask the unpacked bits.
  unpacked_ &= 0x101010101010101UL;

  // Pack data and select the first eight MSB.
  uint64_t packed = (unpacked_ * 0x8040201008040201UL) >> 56UL;

  return static_cast<RetType>(packed);
}

span<uint8_t> ocudu::ocuduvec::bit_unpack(span<uint8_t> bits, unsigned value, unsigned nof_bits)
{
  ocudu_assert(bits.size() >= nof_bits, "Input span size is too small");

  for (unsigned i = 0; i != nof_bits; ++i) {
    bits[i] = (value >> (nof_bits - i - 1)) & 0x1;
  }

  return bits.last(bits.size() - nof_bits);
}

void ocudu::ocuduvec::bit_unpack(span<uint8_t> unpacked, const bit_buffer& packed)
{
  ocudu_assert(packed.size() == unpacked.size(),
               "The packed number of bits (i.e.{}) must be equal to the number of unpacked bits (i.e., {}).",
               packed.size(),
               unpacked.size());
  // Read/write byte index.
  unsigned i_byte       = 0;
  uint8_t* unpacked_ptr = unpacked.data();

#if defined(__AVX512F__) && defined(__AVX512BW__)
  const __mmask64* avx512_packed_ptr = reinterpret_cast<const __mmask64*>(packed.get_buffer().data());
  for (unsigned i_byte_end = (packed.size() / 64) * 8; i_byte != i_byte_end; i_byte += 8) {
    // Load 64 bits in a go as a mask.
    __mmask64 blend_mask = *(avx512_packed_ptr++);

    // Convert mask to 0x01 (for 1s) and 0x00 (for 0s).
    __m512i reg = _mm512_movm_epi8(blend_mask);
    reg         = _mm512_and_si512(reg, _mm512_set1_epi8(0x01));

    // Reverses bits within bytes.
    __m512i shuffle_idx = _mm512_set_epi8(
        // clang-format off
        0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f,
        0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
        0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
        0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
        0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
        0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07
        // clang-format on
    );
    reg = _mm512_shuffle_epi8(reg, shuffle_idx);

    // Store register.
    _mm512_storeu_si512(reinterpret_cast<__m512i*>(unpacked_ptr + 8 * i_byte), reg);
  }
#endif // defined(__AVX512F__) && defined(__AVX512BW__)

#if defined(__AVX__) && defined(__AVX2__)
  const uint8_t* avx2_packed_ptr   = packed.get_buffer().data();
  uint8_t*       avx2_unpacked_ptr = unpacked.data();

  for (unsigned i_byte_end = (packed.size() / 32) * 4; i_byte != i_byte_end; i_byte += 4) {
    // Load input in different registers.
    __m256i in = _mm256_setzero_si256();
    in         = _mm256_insert_epi8(in, avx2_packed_ptr[i_byte + 0], 0);
    in         = _mm256_insert_epi8(in, avx2_packed_ptr[i_byte + 1], 8);
    in         = _mm256_insert_epi8(in, avx2_packed_ptr[i_byte + 2], 16);
    in         = _mm256_insert_epi8(in, avx2_packed_ptr[i_byte + 3], 24);

    // Repeats each byte 8 times.
    __m256i shuffle_mask = _mm256_setr_epi8(
        // clang-format off
        0, 0, 0, 0, 0, 0, 0, 0,
        8, 8, 8, 8, 8, 8, 8, 8,
        0, 0, 0, 0, 0, 0, 0, 0,
        8, 8, 8, 8, 8, 8, 8, 8
        // clang-format on
    );
    in = _mm256_shuffle_epi8(in, shuffle_mask);

    // Selects each of the bits.
    __m256i mask = _mm256_set_epi8(
        // clang-format off
        1, 2, 4, 8, 16, 32, 64, -128,
        1, 2, 4, 8, 16, 32, 64, -128,
        1, 2, 4, 8, 16, 32, 64, -128,
        1, 2, 4, 8, 16, 32, 64, -128
        // clang-format on
    );
    mask = _mm256_and_si256(mask, in);

    //  Moves the selected bit to the LSB of each output byte.
    mask = ~_mm256_cmpeq_epi8(mask, _mm256_setzero_si256());
    mask = _mm256_and_si256(mask, _mm256_set1_epi8(1));

    // Store register.
    _mm256_storeu_si256(reinterpret_cast<__m256i*>(avx2_unpacked_ptr + i_byte * 8), mask);
  }
#endif // defined(__AVX__) && defined(__AVX2__)

  for (unsigned i_byte_end = packed.size() / 8; i_byte != i_byte_end; ++i_byte) {
    // Extract byte.
    uint8_t byte = packed.get_byte(i_byte);
    // Unpack byte.
    unpack_8bit({unpacked_ptr + 8 * i_byte, 8}, byte);
  }

  // Unpack remainder bits.
  unpacked = unpacked.last(unpacked.size() % 8);
  if (!unpacked.empty()) {
    std::array<uint8_t, 8> temp_unpacked;
    span<uint8_t>          unpacked2 = temp_unpacked;
    unpack_8bit(unpacked2, packed.extract(8 * i_byte, unpacked.size()));
    ocuduvec::copy(unpacked, unpacked2.last(unpacked.size()));
  }
}

void ocudu::ocuduvec::bit_unpack(span<uint8_t> unpacked, const bit_buffer& packed, unsigned offset)
{
  ocudu_assert(unpacked.size() <= packed.size() - offset,
               "The unpacked number of bits (i.e.{}) must be equal or lower to the number of packed bits - offset "
               "(i.e., {}-{}).",
               unpacked.size(),
               packed.size(),
               offset);

  // Calculate the number of bits to align the packed data to byte boundary.
  unsigned nof_head_bits = std::min((8 - (offset % 8)) % 8, static_cast<unsigned>(unpacked.size()));
  if (nof_head_bits != 0) {
    // Extract the alignment bits.
    uint8_t head_bits = packed.extract(offset, nof_head_bits);

    // Unpack the head.
    unpacked = bit_unpack(unpacked, head_bits, nof_head_bits);
  }

  unsigned aligned_offset = divide_ceil(offset, 8) * 8;
  if (!unpacked.empty() && (packed.size() != aligned_offset)) {
    const bit_buffer aligned_packed = packed.last(packed.size() - aligned_offset).first(unpacked.size());
    bit_unpack(unpacked, aligned_packed);
  }
}

unsigned ocudu::ocuduvec::bit_pack(span<const uint8_t>& bits, unsigned nof_bits)
{
  ocudu_assert(nof_bits <= 32U, "Number of bits ({}) exceeds maximum (32).", nof_bits);

  unsigned value = 0;

  for (unsigned i = 0; i != nof_bits; ++i) {
    value |= (unsigned)bits[i] << (nof_bits - i - 1U);
  }

  // Advance pointer
  bits = bits.last(bits.size() - nof_bits);

  return value;
}

unsigned ocudu::ocuduvec::bit_pack(span<const uint8_t> bits)
{
  ocudu_assert(bits.size() <= 32U, "Number of bits ({}) exceeds maximum (32).", bits.size());

  unsigned value = 0;

  for (unsigned i = 0, nof_bits = bits.size(); i != nof_bits; ++i) {
    value |= (unsigned)bits[i] << (nof_bits - i - 1U);
  }

  return value;
}

void ocudu::ocuduvec::bit_pack(bit_buffer& packed, span<const uint8_t> unpacked)
{
  ocudu_assert(packed.size() == unpacked.size(),
               "The packed number of bits (i.e.{}) must be equal to the number of unpacked bits (i.e., {}).",
               packed.size(),
               unpacked.size());
  unsigned i_byte = 0;

#if defined(__AVX512F__) && defined(__AVX512BW__)
  __mmask64* avx_packed_ptr = reinterpret_cast<__mmask64*>(packed.get_buffer().data());
  for (unsigned i_byte_end = (packed.size() / 64) * 8; i_byte != i_byte_end; i_byte += 8) {
    // Load the 64 input values into an AVX-512 register.
    __m512i reg = _mm512_loadu_si512(reinterpret_cast<const __m512i*>(unpacked.data()));

    // Reverses bits within bytes.
    __m512i shuffle_idx = _mm512_set_epi8(
        // clang-format off
        0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f,
        0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
        0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
        0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
        0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
        0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07
        // clang-format on
    );
    reg = _mm512_shuffle_epi8(reg, shuffle_idx);

    // Extract the LSBs using a bitwise AND operation.
    *(avx_packed_ptr++) = _mm512_test_epi8_mask(reg, _mm512_set1_epi8(1));

    unpacked = unpacked.last(unpacked.size() - 64);
  }
#endif // defined(__AVX512F__) && defined(__AVX512BW__)

  // Use raw pointers to avoid an overuse of span<>::operator[].
  uint8_t*       packed_ptr   = packed.get_buffer().data();
  const uint8_t* unpacked_ptr = unpacked.data();
  for (unsigned i_byte_end = packed.size() / 8; i_byte != i_byte_end; ++i_byte) {
    unsigned byte      = pack_8bit<unsigned>({unpacked_ptr, 8});
    packed_ptr[i_byte] = byte;
    unpacked_ptr += 8;
  }
  unpacked = unpacked.last(unpacked.size() % 8);

  if (!unpacked.empty()) {
    unsigned               bit_offset = 8 * (packed.size() / 8);
    std::array<uint8_t, 8> temp_unpacked;
    span<uint8_t>          unpacked2 = temp_unpacked;
    ocuduvec::copy(unpacked2.first(unpacked.size()), unpacked);
    ocuduvec::zero(unpacked2.last(8 - unpacked.size()));

    unsigned byte = pack_8bit<unsigned>(unpacked2) >> (8 - unpacked.size());
    packed.insert(byte, bit_offset, unpacked.size());
  }
}

void ocudu::ocuduvec::bit_pack(ocudu::bit_buffer& packed, unsigned offset, span<const uint8_t> unpacked)
{
  ocudu_assert(
      packed.size() >= unpacked.size() + offset,
      "The packed number of bits (i.e.{}) must be equal or higher to the number of packed bits + offset (i.e., {}+{}).",
      packed.size(),
      unpacked.size(),
      offset);
  // Calculate the number of bits to align the packed data to byte boundary.
  unsigned nof_head_bits = std::min((8 - (offset % 8)) % 8, static_cast<unsigned>(unpacked.size()));
  if (nof_head_bits != 0) {
    // Pack the bits of the head.
    unsigned head_bits = bit_pack(unpacked, nof_head_bits);

    // Insert the packed head bits.
    packed.insert(head_bits, offset, nof_head_bits);
  }

  unsigned aligned_offset = divide_ceil(offset, 8) * 8;
  if (!unpacked.empty() && (packed.size() != aligned_offset)) {
    bit_buffer aligned_packed = packed.last(packed.size() - aligned_offset).first(unpacked.size());
    bit_pack(aligned_packed, unpacked);
  }
}

void ocudu::ocuduvec::copy_offset(ocudu::bit_buffer& output, span<const uint8_t> input, unsigned startpos)
{
  static constexpr unsigned bits_per_word    = 8;
  unsigned                  input_start_word = startpos / bits_per_word;
  unsigned                  input_start_mod  = startpos % bits_per_word;

  using extended_word_t = uint32_t;

  unsigned nof_full_words     = output.size() / bits_per_word;
  unsigned nof_remainder_bits = output.size() % bits_per_word;

  span<uint8_t> buffer = output.get_buffer();

  if (input_start_mod == 0) {
    std::copy_n(input.begin() + input_start_word, nof_full_words, buffer.begin());
  } else {
    unsigned i_word = 0;
#ifdef __AVX2__
    for (unsigned i_word_end = (nof_full_words / 32) * 32; i_word != i_word_end; i_word += 32) {
      __m256i word0 = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(input.data() + input_start_word + i_word));
      __m256i word1 =
          _mm256_loadu_si256(reinterpret_cast<const __m256i*>(input.data() + input_start_word + i_word + 1));
      word0        = _mm256_and_si256(_mm256_slli_epi32(word0, input_start_mod),
                               _mm256_set1_epi8(mask_msb_ones<uint8_t>(bits_per_word - input_start_mod)));
      word1        = _mm256_and_si256(_mm256_srli_epi32(word1, bits_per_word - input_start_mod),
                               _mm256_set1_epi8(mask_lsb_ones<uint8_t>(input_start_mod)));
      __m256i word = _mm256_or_si256(word0, word1);
      _mm256_storeu_si256(reinterpret_cast<__m256i*>(buffer.data() + i_word), word);
    }
#endif // __AVX2__
    for (; i_word != nof_full_words; ++i_word) {
      extended_word_t input_word = static_cast<extended_word_t>(input[input_start_word + i_word]) << bits_per_word;
      input_word |= static_cast<extended_word_t>(input[input_start_word + i_word + 1]);
      input_word = input_word >> (bits_per_word - input_start_mod);
      input_word &= mask_lsb_ones<extended_word_t>(bits_per_word);
      buffer[i_word] = static_cast<uint8_t>(input_word);
    }
  }

  if (nof_remainder_bits != 0) {
    extended_word_t remainder_bits = static_cast<extended_word_t>(input[input_start_word + nof_full_words])
                                     << bits_per_word;
    if (input_start_mod + nof_remainder_bits > bits_per_word) {
      remainder_bits |= static_cast<extended_word_t>(input[input_start_word + nof_full_words + 1]);
    }
    remainder_bits = remainder_bits >> (bits_per_word - input_start_mod);
    remainder_bits &= remainder_bits & mask_lsb_ones<extended_word_t>(bits_per_word);
    buffer[nof_full_words] = static_cast<uint8_t>(remainder_bits) & mask_msb_ones<uint8_t>(nof_remainder_bits);
  }
}

void ocudu::ocuduvec::copy_offset(ocudu::bit_buffer&       output,
                                  unsigned                 out_offset,
                                  const ocudu::bit_buffer& input,
                                  unsigned                 in_offset,
                                  unsigned                 nof_bits)
{
  ocudu_assert(input.size() - in_offset >= nof_bits,
               "Input buffer size (i.e., {}) and reading offset (i.e., {}) cannot accomodate copy size of {} bits.",
               input.size(),
               in_offset,
               nof_bits);

  ocudu_assert(input.size() - in_offset >= nof_bits,
               "Output buffer size (i.e., {}) and writing offset (i.e., {}) cannot accomodate copy size of {} bits.",
               output.size(),
               out_offset,
               nof_bits);

  // 8 bit per word.
  static constexpr unsigned bits_per_word = 8U;

  unsigned in_start_word = in_offset / bits_per_word;
  unsigned in_start_mod  = in_offset % bits_per_word;

  unsigned out_start_word = out_offset / bits_per_word;
  unsigned out_start_mod  = out_offset % bits_per_word;

  unsigned remaining_bits = nof_bits;

  unsigned i_bit = 0;
  if ((in_start_mod == 0) && (out_start_mod == 0)) {
    // Determine the number of bit words to copy.
    unsigned nof_words = nof_bits / bits_per_word;

    // Get the buffer views.
    span<uint8_t>       out_buffer = output.get_buffer().subspan(out_start_word, nof_words);
    span<const uint8_t> in_buffer  = input.get_buffer().subspan(in_start_word, nof_words);

    // Copy data.
    ocuduvec::copy(out_buffer, in_buffer);

    // Compute the remaining bits to copy.
    remaining_bits -= nof_words * bits_per_word;

    // Increment input offset.
    i_bit += nof_words * bits_per_word;
  }

  for (; remaining_bits != 0;) {
    // Extract at most one byte from the input.
    unsigned bits_to_extract = std::min(remaining_bits, 8U);
    uint8_t  byte            = input.extract(i_bit + in_offset, bits_to_extract);

    // Insert the extracted bits into the output buffer.
    output.insert(byte, i_bit + out_offset, bits_to_extract);

    remaining_bits -= bits_to_extract;
    i_bit += bits_to_extract;
  }
}

void ocudu::ocuduvec::reverse_bit_order_in_bytes(span<uint8_t> output, span<const uint8_t> input)
{
  std::size_t nof_bytes = input.size();
  ocudu_assert(output.size() == nof_bytes,
               "The input number of bytes (i.e.{}) must be equal to the output number of bytes (i.e., {}).",
               nof_bytes,
               output.size());
  std::size_t i_byte = 0U;

// AVX2 implementation. It reverses the bytes in groups of 32.
#if defined(__AVX__) && defined(__AVX2__)
  static constexpr std::size_t avx2_nof_bytes = 32;

  // Look-up table for reverting the bit order inside a nibble. It maps nibbles into other nibbles that contain the same
  // bits with the opposite order.
  __m256i nibble_reversal_lut = _mm256_setr_epi8(
      // clang-format off
      0x0,0x8,0x4,0xc,0x2,0xa,0x6,0xe,
      0x1,0x9,0x5,0xd,0x3,0xb,0x7,0xf,
      0x0,0x8,0x4,0xc,0x2,0xa,0x6,0xe,
      0x1,0x9,0x5,0xd,0x3,0xb,0x7,0xf
      // clang-format on
  );

  // Mask that selects the low nibble within each byte.
  __m256i nibble_mask = _mm256_set1_epi8(0x0f);

  for (std::size_t i_byte_end = nof_bytes / avx2_nof_bytes * avx2_nof_bytes; i_byte != i_byte_end;
       i_byte += avx2_nof_bytes) {
    // Load 32 bytes from the input.
    __m256i in = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(&input[i_byte]));

    // Mask the low and high nibbles in each byte.
    __m256i lo_nibbles = _mm256_and_si256(in, nibble_mask);
    __m256i hi_nibbles = _mm256_and_si256(_mm256_srli_epi16(in, 4), nibble_mask);

    // For each of the input nibbles, select its corresponding reversed entry in the LUT.
    lo_nibbles = _mm256_shuffle_epi8(nibble_reversal_lut, lo_nibbles);
    hi_nibbles = _mm256_shuffle_epi8(nibble_reversal_lut, hi_nibbles);

    // Swap low and high nibble positions.
    __m256i out = _mm256_or_si256(_mm256_slli_epi16(lo_nibbles, 4), hi_nibbles);

    // Store the results.
    _mm256_storeu_si256(reinterpret_cast<__m256i*>(&output[i_byte]), out);
  }
#endif // defined(__AVX__) && defined(__AVX2__)

  // Process the remaining bytes using a LUT.
  for (; i_byte != nof_bytes; ++i_byte) {
    output[i_byte] = reverse_byte<uint8_t>(input[i_byte]);
  }
}
