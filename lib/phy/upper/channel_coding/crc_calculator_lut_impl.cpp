// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "crc_calculator_lut_impl.h"
#include "ocudu/ocuduvec/bit.h"
#include "ocudu/support/math/bit_ops.h"

#if __SSE4_1__
#include <immintrin.h>
#endif // __SSE4_1__

using namespace ocudu;

crc_calculator_lut_impl::crc_table_s::crc_table_s(unsigned polynom_, unsigned order_) :
  order(order_), crcmask((((1UL << (order - 1UL)) - 1UL) << 1UL) | 1UL), polynom(polynom_)
{
  unsigned pad        = (order < 8) ? (8 - order) : 0;
  unsigned ord        = order + pad - 8;
  uint64_t crchighbit = (uint64_t)1 << (order - 1);

  for (unsigned i = 0; i != 256; ++i) {
    uint64_t remainder = ((uint64_t)i) << ord;
    for (unsigned j = 0; j != 8; ++j) {
      bool bit = (remainder & crchighbit) != 0;
      remainder <<= 1U;
      if (bit) {
        remainder ^= polynom;
      }
    }
    (*this)[i] = (remainder >> pad) & crcmask;
  }
}

const crc_calculator_lut_impl::crc_table_s& crc_calculator_lut_impl::get_crc_table(crc_generator_poly polynomial)
{
  static std::map<crc_generator_poly, crc_table_s> crc_tables;

  // Create a new table if the given polynomial has not been created earlier.
  if (crc_tables.count(polynomial) == 0) {
    crc_tables.emplace(polynomial, crc_table_s(to_uint(polynomial), get_crc_size(polynomial)));
  }

  return crc_tables.at(polynomial);
}

crc_calculator_lut_impl::crc_calculator_lut_impl(crc_generator_poly poly_) :
  table(get_crc_table(poly_)), order(table.order), crcmask(table.crcmask), poly(poly_)
{
  // Do nothing
}

crc_calculator_checksum_t ocudu::crc_calculator_lut_impl::calculate_byte(span<const uint8_t> input) const
{
  uint32_t crc = 0;

  // For each byte
  for (uint8_t byte : input) {
    crc = put_byte(crc, byte);
  }

  return get_checksum(crc);
}

crc_calculator_checksum_t crc_calculator_lut_impl::calculate_bit(ocudu::span<const uint8_t> input) const
{
  uint32_t crc = 0;

  int a = 0;

  // Pack bits into bytes.
  unsigned nbytes = input.size() / 8;
  unsigned res8   = input.size() % 8;
  if (res8 > 0) {
    a = 1;
  }

  // Calculate CRC.
  for (unsigned i = 0; i < nbytes; i++) {
    // Get pack a byte from 8 bits.
    span<const uint8_t> pter = input.subspan(8U * i, 8);
#ifdef __SSE4_1__
    // Get 8 Bit
    __m64 mask = _mm_cmpgt_pi8(*(reinterpret_cast<const __m64*>(pter.data())), _mm_set1_pi8(0));

    // Get mask and reverse.
    uint8_t byte = reverse_byte(static_cast<uint8_t>(_mm_movemask_pi8(mask)));
#else  // __SSE4_1__
    uint8_t byte = (uint8_t)(ocuduvec::bit_pack(pter, 8) & 0xff);
#endif // __SSE4_1__
    crc = put_byte(crc, byte);
  }

  for (unsigned i = nbytes; i < nbytes + a; i++) {
    uint8_t byte = 0x00;
    for (unsigned k = 0; k < res8; k++) {
      byte |= (uint8_t)((input[i * 8 + k]) << (7U - k));
    }
    crc = put_byte(crc, byte);
  }

  crc = (uint32_t)get_checksum(crc);

  // Reverse CRC res8 positions
  if (a == 1) {
    crc = reversecrcbit(crc, 8 - res8);
  }

  return get_checksum(crc);
}
crc_calculator_checksum_t crc_calculator_lut_impl::calculate(const bit_buffer& input) const
{
  // reset();
  uint32_t crc = 0;

  // Pack bits into bytes.
  unsigned nbytes = input.size() / 8;
  unsigned res8   = input.size() % 8;

  // Extract and calculate CRC for each byte.
  for (unsigned i_byte = 0; i_byte != nbytes; ++i_byte) {
    uint8_t byte = input.get_byte(i_byte);
    crc          = put_byte(crc, byte);
  }

  // Process remainder bits.
  if (res8 > 0) {
    uint8_t remainder = input.extract(nbytes * 8, res8);
    crc               = put_byte(crc, remainder << (8 - res8));
  }

  crc = (uint32_t)get_checksum(crc);

  if (res8 > 0) {
    // Reverse CRC res8 positions
    crc = reversecrcbit(crc, 8 - res8);
  }

  return get_checksum(crc);
}
