// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "crc_calculator_generic_impl.h"
#include "ocudu/ocuduvec/bit.h"
#include "ocudu/support/math/math_utils.h"

using namespace ocudu;

crc_calculator_generic_impl::crc_calculator_generic_impl(crc_generator_poly poly_) : poly(poly_)
{
  order   = get_crc_size(poly);
  polynom = to_uint(poly);
}

crc_calculator_checksum_t ocudu::crc_calculator_generic_impl::calculate_byte(span<const uint8_t> input) const
{
  uint64_t highbit   = 1U << order;
  uint64_t remainder = 0;

  for (uint8_t v : input) {
    for (unsigned bit_idx = 0; bit_idx != 8; ++bit_idx) {
      uint64_t bit = (static_cast<uint64_t>(v) >> (7UL - bit_idx)) & 1UL;

      remainder = (remainder << 1U) | bit;

      if ((remainder & highbit) != 0) {
        remainder ^= polynom;
      }
    }
  }

  for (unsigned bit_idx = 0; bit_idx != order; ++bit_idx) {
    remainder = (remainder << 1U);

    if ((remainder & highbit) != 0) {
      remainder ^= polynom;
    }
  }

  return static_cast<crc_calculator_checksum_t>(remainder & (highbit - 1));
}

crc_calculator_checksum_t crc_calculator_generic_impl::calculate_bit(ocudu::span<const uint8_t> input) const
{
  uint64_t highbit   = 1U << order;
  uint64_t remainder = 0;

  for (uint8_t bit : input) {
    remainder = (remainder << 1U) | bit;

    if ((remainder & highbit) != 0) {
      remainder ^= polynom;
    }
  }

  for (unsigned bit_idx = 0; bit_idx != order; ++bit_idx) {
    remainder = (remainder << 1U);

    if ((remainder & highbit) != 0) {
      remainder ^= polynom;
    }
  }

  return static_cast<crc_calculator_checksum_t>(remainder & (highbit - 1));
}

crc_calculator_checksum_t crc_calculator_generic_impl::calculate(const bit_buffer& input) const
{
  uint64_t highbit   = 1U << order;
  uint64_t remainder = 0;

  for (unsigned i_bit = 0, i_bit_end = input.size(); i_bit != i_bit_end; ++i_bit) {
    remainder = (remainder << 1U) | input.extract<uint64_t>(i_bit, 1);

    if ((remainder & highbit) != 0) {
      remainder ^= polynom;
    }
  }

  for (unsigned bit_idx = 0; bit_idx != order; ++bit_idx) {
    remainder = (remainder << 1U);

    if ((remainder & highbit) != 0) {
      remainder ^= polynom;
    }
  }

  return static_cast<crc_calculator_checksum_t>(remainder & (highbit - 1));
}
