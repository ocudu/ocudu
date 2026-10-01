// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

/// \file
/// \brief Utility functions for powers of 2.

#pragma once

#include <type_traits>

namespace ocudu {

/// \brief Calculates the integer power of 2.
///
/// \param[in] power Indicates the power of 2 to calculate.
/// \return The result of the operation.
constexpr unsigned pow2(unsigned power)
{
  return 1U << power;
}

/// \brief Calculates \f$\left \lceil log_2(n) \right \rceil\f$.
///
/// \tparam Integer Any unsigned integer type.
/// \param[in] value Parameter \f$n\f$.
/// \return The result of the calculation if \c value is not zero. Otherwise 0.
template <typename Integer>
constexpr Integer log2_ceil(Integer value)
{
  static_assert(std::is_unsigned_v<Integer>, "log2_ceil only works for unsigned integers");

  // Avoid unbounded results.
  if (value <= 0) {
    return 0;
  }

  Integer result = 0;
  Integer v      = value - 1;
  while (v > 0) {
    v >>= 1U;
    ++result;
  }
  return result;
}

/// \brief Determines whether \c value is a power of 2. Zero is not considered a power of 2.
/// \tparam Integer Any unsigned integer type.
template <typename Integer>
constexpr bool is_pow2(Integer value)
{
  static_assert(std::is_unsigned_v<Integer>, "is_pow2 only works for unsigned integers");

  return value > 0 and (value & (value - 1)) == 0;
}

/// \brief Rounds \c value up to the closest power of 2. Zero is left unchanged.
/// \tparam Integer Any unsigned integer type.
template <typename Integer>
constexpr Integer to_next_pow2(Integer value)
{
  static_assert(std::is_unsigned_v<Integer>, "to_next_pow2 only works for unsigned integers");

  Integer result = (value > 0) ? 1 : 0;
  while (result < value) {
    result <<= 1U;
  }
  return result;
}

} // namespace ocudu
