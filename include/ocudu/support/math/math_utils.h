// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

/// \file
/// \brief Mathematical utility functions.

#pragma once

#include "ocudu/support/math/pow2_utils.h"
#include "ocudu/support/ocudu_assert.h"
#include <array>
#include <cmath>
#include <cstdint>
#include <numeric>

namespace ocudu {

/// Defines two times Pi.
constexpr float TWOPI = 2.0F * static_cast<float>(M_PI);

/// Floating point near zero value.
constexpr float near_zero = 1e-9;

/// \brief Performs an integer division rounding up.
///
/// \tparam     NumType Division numerator integer type.
/// \tparam     DenType Division denominator integer type.
/// \param[in]  num     Numerator.
/// \param[out] den     Denominator.
/// \return The result of the operation.
template <typename NumType, typename DenType>
constexpr auto divide_ceil(NumType num, DenType den)
{
  static_assert(std::is_integral_v<NumType>, "The numerator must be an integer.");
  static_assert(std::is_integral_v<DenType>, "The denominator must be an integer.");
  ocudu_sanity_check(den != 0, "Denominator cannot be zero.");
  return (num + (den - 1)) / den;
}

/// \brief Performs an integer division rounding to the nearest integer.
///
/// \param[in]  num Numerator.
/// \param[out] den Denominator.
/// \return The result of the operation.
constexpr unsigned divide_round(unsigned num, unsigned den)
{
  ocudu_sanity_check(den != 0, "Denominator cannot be zero.");
  return static_cast<unsigned>(std::round(static_cast<float>(num) / static_cast<float>(den)));
}

/// Determines whether a floating point value is near zero.
inline bool is_near_zero(float value)
{
  return std::abs(value) < near_zero;
}

/// \brief Converts a value in decibels to linear amplitude ratio
/// \param [in] value is in decibels
/// \return the resultant amplitude ratio
inline float convert_dB_to_amplitude(float value)
{
  return std::pow(10.0F, value / 20.0F);
}

/// \brief Converts a value in decibels to linear power ratio
/// \param [in] value is in decibels
/// \return the resultant power ratio
inline float convert_dB_to_power(float value)
{
  return std::pow(10.0F, value / 10.0F);
}

/// \brief Converts a linear amplitude ratio to decibels
/// \param [in] value is the linear amplitude
/// \return the resultant decibels
inline float convert_amplitude_to_dB(float value)
{
  return 20.0F * std::log10(value);
}

/// \brief Converts a linear power ratio to decibels
/// \param [in] value is the linear power
/// \return the resultant decibels
inline float convert_power_to_dB(float value)
{
  return 10.0F * std::log10(value);
}

/// \brief Finds the smallest prime number greater than \c n.
/// \remark Only works for prime numbers not larger than 3299.
unsigned prime_greater_than(unsigned n);

/// \brief Finds the biggest prime number less than \c n.
/// \remark Only works for prime numbers not larger than 3299.
unsigned prime_lower_than(unsigned n);

/// Calculates the least common multiplier (LCM) for a range of integers.
template <typename Integer, typename It>
Integer lcm(It begin, It end)
{
  return std::accumulate(begin, end, Integer(1), [](Integer a, Integer b) { return std::lcm<Integer>(a, b); });
}

/// \brief Applies the extended Euclidean algorithm to \c a and \c b.
///
/// \param[in] a First operand.
/// \param[in] b Second operand.
/// \return A tuple {g, x, y}, where g = gcd(a, b) and x, y are Bezout coefficients such that a*x + b*y = g.
inline std::array<int64_t, 3> extended_gcd(int64_t a, int64_t b)
{
  int64_t old_r = a;
  int64_t r     = b;
  int64_t old_x = 1;
  int64_t cur_x = 0;
  int64_t old_y = 0;
  int64_t cur_y = 1;

  while (r != 0) {
    const int64_t quotient = old_r / r;

    const int64_t next_r = old_r - quotient * r;
    old_r                = r;
    r                    = next_r;

    const int64_t next_x = old_x - quotient * cur_x;
    old_x                = cur_x;
    cur_x                = next_x;

    const int64_t next_y = old_y - quotient * cur_y;
    old_y                = cur_y;
    cur_y                = next_y;
  }

  return {old_r, old_x, old_y};
}

/// \brief Checks whether an s exists that satisfies the two congruences, s = a (mod n1) and s = b (mod n2).
///
/// \param[in] a First congruence residue.
/// \param[in] n1 First congruence modulus.
/// \param[in] b Second congruence residue.
/// \param[in] n2 Second congruence modulus.
/// \return True if a solution exists.
inline bool crt_solvable(unsigned a, unsigned n1, unsigned b, unsigned n2)
{
  // Generalized Chinese Remainder Theorem states there is a solution iff: a mod gcd(n1, n2) = b mod gcd(n1, n2).
  const unsigned g = std::gcd(n1, n2);
  return (a % g) == (b % g);
}

/// \brief Finds the smallest s that satisfies the two congruences, s = a (mod n1) and s = b (mod n2).
///
/// \param[in] a First congruence residue.
/// \param[in] n1 First congruence modulus.
/// \param[in] b Second congruence residue.
/// \param[in] n2 Second congruence modulus.
/// \return The smallest non-negative s satisfying both congruences. The result is expressed modulo lcm(n1, n2).
inline unsigned crt(unsigned a, unsigned n1, unsigned b, unsigned n2)
{
  ocudu_sanity_check(crt_solvable(a, n1, b, n2), "No solution exists for the given congruences");

  const auto [g, x, y]       = extended_gcd(n1, n2);
  const int64_t combined_mod = static_cast<int64_t>(n1) / g * n2;

  int64_t s = a + static_cast<int64_t>(n1) * x * ((static_cast<int64_t>(b) - a) / g);
  s         = ((s % combined_mod) + combined_mod) % combined_mod;
  return static_cast<unsigned>(s);
}

} // namespace ocudu
