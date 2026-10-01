// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/bit_buffer.h"
#include "ocudu/adt/span.h"

namespace ocudu {

/// \brief Cyclic Redundancy Check (CRC) calculator polynomials enumeration.
///
/// The CRC polynomials for 5G NR are given in TS38.212 Section 5.1.
/// The hexadecimal value associated with each label expands to the generator polynomial, with the MSB being the
/// highest-degree coefficient, e.g. `0x61` \f$\longrightarrow D^6 + D^5 + 1\f$.
enum class crc_generator_poly : uint32_t {
  CRC24A = 0x1864cfb,
  CRC24B = 0x1800063,
  CRC24C = 0x1b2b117,
  CRC16  = 0x11021,
  CRC11  = 0xe21,
  CRC6   = 0x61
};

/// Creates a CRC polynomial that is not defined by the 5G NR specification.
constexpr crc_generator_poly crc_polynomial_from_uint(uint32_t polynomial)
{
  return static_cast<crc_generator_poly>(polynomial);
}

/// Converts the CRC generator polynomial type enumeration to a 32-bit unsigned integer.
constexpr uint32_t to_uint(crc_generator_poly polynomial)
{
  return static_cast<uint32_t>(polynomial);
}

/// \brief Returns the CRC size in bits from a polynomial.
///
/// The CRC size is derived from the polynomial order.
inline unsigned get_crc_size(crc_generator_poly poly)
{
  uint32_t polynomial = to_uint(poly);
#if defined(__GNUC__) || defined(__clang__)
  return 31 - __builtin_clz(polynomial);
#else  // defined(__GNUC__) || defined(__clang__)
  unsigned int pos = 0;
  while (polynomial >>= 1) {
    ++pos;
  }
  return pos;
#endif // defined(__GNUC__) || defined(__clang__)
}

/// Checksum type.
using crc_calculator_checksum_t = unsigned;

/// \brief Calculates the CRC, as per TS38.212 Section 5.1. Interface.
class crc_calculator
{
public:
  virtual ~crc_calculator() = default;

  ///\brief Calculates the checksum from a byte buffer (8 packed bits in every input byte).
  ///\param[in] data Provides the byte buffer.
  ///\return The resulting CRC checksum.
  virtual crc_calculator_checksum_t calculate_byte(span<const uint8_t> data) const = 0;

  ///\brief Calculates the checksum from a bit buffer (1 bit in for every input byte).
  ///\param[in] data Provides the byte buffer.
  ///\return The resulting CRC checksum.
  virtual crc_calculator_checksum_t calculate_bit(span<const uint8_t> data) const = 0;

  ///\brief Calculates the checksum from a bit buffer.
  ///\param[in] data Input data.
  ///\return The resulting CRC checksum.
  virtual crc_calculator_checksum_t calculate(const bit_buffer& data) const = 0;

  ///\brief Returns the identifier of the generator polynomial.
  virtual crc_generator_poly get_generator_poly() const = 0;
};

} // namespace ocudu
