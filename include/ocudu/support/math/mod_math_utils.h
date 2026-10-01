// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include <type_traits>

namespace ocudu {

/// \brief Computes the signed modular difference between two unsigned integers.
///
/// The result represents the distance between lhs and rhs around a circle, choosing the shortest direction (clockwise
/// or anticlockwise), each direction represented by a different sign.
/// This operation can be represented mathematically as ((lhs - rhs + N/2) mod N) - N/2, where N is the modulus.
/// The result is in the range [-N/2, N/2).
/// \tparam N The modulus. Must be a positive integer.
/// \param lhs The left-hand side operand. It must be an unsigned integer, but does not need to be limited to the range
/// [0, N).
/// \param rhs The right-hand side operand. It must be an unsigned integer, but does not need to be limited to the range
/// [0, N).
/// \return The signed modular difference between lhs and rhs.
template <auto N, typename Integer>
constexpr auto signed_modular_difference(Integer lhs, Integer rhs)
{
  static_assert(std::is_unsigned_v<Integer>, "signed_modular_difference only should be used for unsigned integers");
  return static_cast<std::make_signed_t<Integer>>(((lhs - rhs + N + (N / 2)) % N) - (N / 2));
}
template <typename Integer>
constexpr auto signed_modular_difference(Integer lhs, Integer rhs, Integer N)
{
  static_assert(std::is_unsigned_v<Integer>, "signed_modular_difference only should be used for unsigned integers");
  return static_cast<std::make_signed_t<Integer>>((((lhs - rhs) + N + (N / 2)) % N) - (N / 2));
}

/// \brief Computes the modular minimum between two unsigned integers.
template <auto N, typename Integer>
constexpr auto modular_min(Integer lhs, Integer rhs)
{
  return signed_modular_difference<N>(lhs, rhs) < 0 ? lhs : rhs;
}
template <typename Integer>
constexpr auto modular_min(Integer lhs, Integer rhs, Integer N)
{
  return signed_modular_difference(lhs, rhs, N) < 0 ? lhs : rhs;
}

/// \brief Computes the modular maximum between two unsigned integers.
template <auto N, typename Integer>
constexpr auto modular_max(Integer lhs, Integer rhs)
{
  return signed_modular_difference<N>(lhs, rhs) > 0 ? lhs : rhs;
}
template <typename Integer>
constexpr auto modular_max(Integer lhs, Integer rhs, Integer N)
{
  return signed_modular_difference(lhs, rhs, N) > 0 ? lhs : rhs;
}

/// \brief Checks whether a point is inside a periodic window.
/// \remark Signed integers are used to avoid issues with unsigned integer wrap-around if point < window_offset.
constexpr bool is_inside_periodic_window(int64_t period, int64_t window_offset, int64_t window_len, int64_t point)
{
  // Compute (point - window_offset) mod period.
  int64_t diff = (point - window_offset) % period;
  // Note: % is not a true mod, so we need to handle negative values manually to place them inside [0, T).
  if (diff < 0) {
    diff += period;
  }
  return diff < window_len;
}

} // namespace ocudu
