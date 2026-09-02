// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

/// \file
/// \brief Mathematical utility functions.

#pragma once

#include "ocudu/support/math/pow2_utils.h"
#include "ocudu/support/ocudu_assert.h"
#include <cmath>
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

} // namespace ocudu
