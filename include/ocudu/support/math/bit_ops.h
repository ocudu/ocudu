// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <type_traits>

namespace ocudu {

/// \brief Returns an unsigned integer with the N most significant bits (MSB) set to zero, and the remaining bits set
/// to 1. e.g.
/// - mask_msb_zeros<uint8_t>(0) == 0xffU
/// - mask_msb_zeros<uint8_t>(1) == 0x7fU
/// - mask_msb_zeros<uint8_t>(2) == 0x3fU
/// - mask_msb_zeros<uint8_t>(8) == 0x00U
/// \tparam Integer Type of unsigned integer returned by the function.
/// \param[in] N Number of MSB bits set to zero.
/// \return Resulting integer bitmap.
template <typename Integer>
constexpr Integer mask_msb_zeros(size_t N)
{
  static_assert(std::is_unsigned_v<Integer>, "T must be unsigned integer");
  return (N >= static_cast<size_t>(std::numeric_limits<Integer>::digits))
             ? 0
             : static_cast<Integer>(std::numeric_limits<Integer>::max() >> N);
}

/// \brief Returns an unsigned integer with the N least significant bits (LSB) set to one, and the remaining bits set
/// to 1. e.g.
/// - mask_lsb_ones<uint8_t>(0) == 0x00U
/// - mask_lsb_ones<uint8_t>(1) == 0x01U
/// - mask_lsb_ones<uint8_t>(7) == 0x7fU
/// - mask_lsb_ones<uint8_t>(8) == 0xffU
/// \tparam Integer Type of unsigned integer returned by the function.
/// \param[in] N Number of LSB bits set to zero.
/// \return Resulting integer bitmap.
template <typename Integer>
constexpr Integer mask_lsb_ones(size_t N)
{
  return mask_msb_zeros<Integer>(sizeof(Integer) * 8U - N);
}

/// \brief Returns an unsigned integer with the N most significant bits (MSB) set to one, and the remaining bits set
/// to zero.
/// \tparam Integer Type of unsigned integer returned by the function.
/// \param[in] N Number of MSB bits set to one.
/// \return Resulting integer bitmap.
template <typename Integer>
constexpr Integer mask_msb_ones(size_t N)
{
  return ~mask_msb_zeros<Integer>(N);
}

/// \brief Returns an unsigned integer with the N least significant bits (LSB) set to zero, and the remaining bits set
/// to one.
/// \tparam Integer Type of unsigned integer returned by the function.
/// \param[in] N Number of LSB bits set to one.
/// \return Resulting integer bitmap.
template <typename Integer>
constexpr Integer mask_lsb_zeros(size_t N)
{
  return ~mask_lsb_ones<Integer>(N);
}

/// \brief Counts the number of bits set to one in an integer.
template <typename Integer>
int count_ones(Integer value)
{
  static_assert(std::is_unsigned_v<Integer>, "Integer must be an unsigned integer");

#ifdef __GNUC__ // clang and gcc
  if constexpr (sizeof(Integer) <= sizeof(unsigned)) {
    return __builtin_popcount(value);
  } else if constexpr (sizeof(Integer) <= sizeof(unsigned long)) {
    return __builtin_popcountl(static_cast<unsigned long>(value));
  } else {
    return __builtin_popcountll(static_cast<unsigned long long>(value));
  }
#else
  // Note: use an "int" for count triggers popcount optimization if SSE instructions are enabled.
  int c = 0;
  for (Integer w = value; w != 0; c++) {
    w &= w - 1;
  }
  return c;
#endif
}

/// \brief Counts the number of contiguous bits set to zero, starting from the MSB.
/// If value is zero, the result is the number of bits of Integer.
template <typename Integer>
Integer zero_msb_count(Integer value)
{
  static_assert(std::is_unsigned_v<Integer>, "Integer must be an unsigned integer");

  if (value == 0) {
    return std::numeric_limits<Integer>::digits;
  }

#ifdef __GNUC__ // clang and gcc
  if constexpr (sizeof(Integer) <= sizeof(unsigned)) {
    // The builtin counts over the promoted argument, so discard the leading zeros the promotion adds.
    constexpr int nof_promoted_bits = std::numeric_limits<unsigned>::digits - std::numeric_limits<Integer>::digits;
    return static_cast<Integer>(__builtin_clz(value) - nof_promoted_bits);
  } else if constexpr (sizeof(Integer) <= sizeof(unsigned long)) {
    return static_cast<Integer>(__builtin_clzl(static_cast<unsigned long>(value)));
  } else {
    return static_cast<Integer>(__builtin_clzll(static_cast<unsigned long long>(value)));
  }
#else
  Integer ret = 0;
  for (Integer shift = std::numeric_limits<Integer>::digits >> 1; shift != 0; shift >>= 1) {
    Integer tmp = value >> shift;
    if (tmp != 0) {
      value = tmp;
    } else {
      ret |= shift;
    }
  }
  return ret;
#endif
}

/// \brief Counts the number of contiguous bits set to zero, starting from the LSB.
/// If value is zero, the result is the number of bits of Integer.
template <typename Integer>
Integer zero_lsb_count(Integer value)
{
  static_assert(std::is_unsigned_v<Integer>, "Integer must be an unsigned integer");

  if (value == 0) {
    return std::numeric_limits<Integer>::digits;
  }

#ifdef __GNUC__ // clang and gcc
  if constexpr (sizeof(Integer) <= sizeof(unsigned)) {
    return static_cast<Integer>(__builtin_ctz(value));
  } else if constexpr (sizeof(Integer) <= sizeof(unsigned long)) {
    return static_cast<Integer>(__builtin_ctzl(static_cast<unsigned long>(value)));
  } else {
    return static_cast<Integer>(__builtin_ctzll(static_cast<unsigned long long>(value)));
  }
#else
  // Every bit below the lowest set bit of value is set in (value - 1) and clear in value.
  return static_cast<Integer>(count_ones(static_cast<Integer>((value - 1) & ~value)));
#endif
}

/// \brief Finds the position of the first bit set to one, starting from the MSB.
/// \tparam Integer Integer type of received bitmap.
/// \param[in] value Integer bitmap
/// \return MSB position with the bit set to one. The MSB has position sizeof(Integer) * 8 - 1.
template <typename Integer>
Integer find_first_msb_one(Integer value)
{
  return value ? static_cast<Integer>(std::numeric_limits<Integer>::digits - 1 - zero_msb_count(value))
               : std::numeric_limits<Integer>::digits;
}

/// \brief Finds the position of the first bit set to one, starting from the LSB.
/// \tparam Integer Integer type of received bitmap.
/// \param[in] value Integer bitmap
/// \return LSB position with the bit set to one. The LSB has position zero.
template <typename Integer>
Integer find_first_lsb_one(Integer value)
{
  return zero_lsb_count(value);
}

namespace detail {

/// \brief Knuth's swap of upper and lower sections of a bitset.
/// \param m Mask of bits to swap.
/// \param k shift amount.
template <typename T, T m, int k>
T swapbits(T p)
{
  T q = ((p >> k) ^ p) & m;
  return p ^ q ^ (q << k);
}

} // namespace detail

/// \brief Knuth's 64-bit reverse. E.g. 0x0000000000000001 -> 0x8000000000000000.
/// For more information see: https://matthewarcus.wordpress.com/2012/11/18/reversing-a-64-bit-word/
/// \param n Number to reverse.
/// \return Reversed number.
inline uint64_t bit_reverse(uint64_t n)
{
  static constexpr uint64_t m0 = 0x5555555555555555LLU;
  static constexpr uint64_t m1 = 0x0300c0303030c303LLU;
  static constexpr uint64_t m2 = 0x00c0300c03f0003fLLU;
  static constexpr uint64_t m3 = 0x00000ffc00003fffLLU;
  n                            = ((n >> 1U) & m0) | (n & m0) << 1U;
  n                            = detail::swapbits<uint64_t, m1, 4>(n);
  n                            = detail::swapbits<uint64_t, m2, 8>(n);
  n                            = detail::swapbits<uint64_t, m3, 20>(n);
  n                            = (n >> 34U) | (n << 30U);
  return n;
}

/// Reverses the bits of a given byte.
template <typename Integer>
Integer reverse_byte(Integer byte)
{
  static_assert(std::is_convertible_v<Integer, uint8_t>,
                "The input type must be convertible to an unsigned integer of eight bits");
  static constexpr std::array<Integer, 256> reverse_lut = {
      0b00000000, 0b10000000, 0b01000000, 0b11000000, 0b00100000, 0b10100000, 0b01100000, 0b11100000, 0b00010000,
      0b10010000, 0b01010000, 0b11010000, 0b00110000, 0b10110000, 0b01110000, 0b11110000, 0b00001000, 0b10001000,
      0b01001000, 0b11001000, 0b00101000, 0b10101000, 0b01101000, 0b11101000, 0b00011000, 0b10011000, 0b01011000,
      0b11011000, 0b00111000, 0b10111000, 0b01111000, 0b11111000, 0b00000100, 0b10000100, 0b01000100, 0b11000100,
      0b00100100, 0b10100100, 0b01100100, 0b11100100, 0b00010100, 0b10010100, 0b01010100, 0b11010100, 0b00110100,
      0b10110100, 0b01110100, 0b11110100, 0b00001100, 0b10001100, 0b01001100, 0b11001100, 0b00101100, 0b10101100,
      0b01101100, 0b11101100, 0b00011100, 0b10011100, 0b01011100, 0b11011100, 0b00111100, 0b10111100, 0b01111100,
      0b11111100, 0b00000010, 0b10000010, 0b01000010, 0b11000010, 0b00100010, 0b10100010, 0b01100010, 0b11100010,
      0b00010010, 0b10010010, 0b01010010, 0b11010010, 0b00110010, 0b10110010, 0b01110010, 0b11110010, 0b00001010,
      0b10001010, 0b01001010, 0b11001010, 0b00101010, 0b10101010, 0b01101010, 0b11101010, 0b00011010, 0b10011010,
      0b01011010, 0b11011010, 0b00111010, 0b10111010, 0b01111010, 0b11111010, 0b00000110, 0b10000110, 0b01000110,
      0b11000110, 0b00100110, 0b10100110, 0b01100110, 0b11100110, 0b00010110, 0b10010110, 0b01010110, 0b11010110,
      0b00110110, 0b10110110, 0b01110110, 0b11110110, 0b00001110, 0b10001110, 0b01001110, 0b11001110, 0b00101110,
      0b10101110, 0b01101110, 0b11101110, 0b00011110, 0b10011110, 0b01011110, 0b11011110, 0b00111110, 0b10111110,
      0b01111110, 0b11111110, 0b00000001, 0b10000001, 0b01000001, 0b11000001, 0b00100001, 0b10100001, 0b01100001,
      0b11100001, 0b00010001, 0b10010001, 0b01010001, 0b11010001, 0b00110001, 0b10110001, 0b01110001, 0b11110001,
      0b00001001, 0b10001001, 0b01001001, 0b11001001, 0b00101001, 0b10101001, 0b01101001, 0b11101001, 0b00011001,
      0b10011001, 0b01011001, 0b11011001, 0b00111001, 0b10111001, 0b01111001, 0b11111001, 0b00000101, 0b10000101,
      0b01000101, 0b11000101, 0b00100101, 0b10100101, 0b01100101, 0b11100101, 0b00010101, 0b10010101, 0b01010101,
      0b11010101, 0b00110101, 0b10110101, 0b01110101, 0b11110101, 0b00001101, 0b10001101, 0b01001101, 0b11001101,
      0b00101101, 0b10101101, 0b01101101, 0b11101101, 0b00011101, 0b10011101, 0b01011101, 0b11011101, 0b00111101,
      0b10111101, 0b01111101, 0b11111101, 0b00000011, 0b10000011, 0b01000011, 0b11000011, 0b00100011, 0b10100011,
      0b01100011, 0b11100011, 0b00010011, 0b10010011, 0b01010011, 0b11010011, 0b00110011, 0b10110011, 0b01110011,
      0b11110011, 0b00001011, 0b10001011, 0b01001011, 0b11001011, 0b00101011, 0b10101011, 0b01101011, 0b11101011,
      0b00011011, 0b10011011, 0b01011011, 0b11011011, 0b00111011, 0b10111011, 0b01111011, 0b11111011, 0b00000111,
      0b10000111, 0b01000111, 0b11000111, 0b00100111, 0b10100111, 0b01100111, 0b11100111, 0b00010111, 0b10010111,
      0b01010111, 0b11010111, 0b00110111, 0b10110111, 0b01110111, 0b11110111, 0b00001111, 0b10001111, 0b01001111,
      0b11001111, 0b00101111, 0b10101111, 0b01101111, 0b11101111, 0b00011111, 0b10011111, 0b01011111, 0b11011111,
      0b00111111, 0b10111111, 0b01111111, 0b11111111,
  };
  return reverse_lut[byte];
}

} // namespace ocudu
