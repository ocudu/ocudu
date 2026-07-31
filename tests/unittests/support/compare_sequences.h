// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/adt/complex.h"
#include "ocudu/adt/expected.h"
#include "ocudu/adt/span.h"
#include "ocudu/support/ocudu_assert.h"
#include <fmt/format.h>
#include <string>
#include <type_traits>
#include <utility>

namespace ocudu {
namespace detail {

/// Formats one table row describing a mismatched pair of values.
template <typename T, typename U, typename V>
std::string format_mismatch_row(unsigned index, const T& actual, const U& expected, V distance, V tolerance)
{
  return fmt::format("   {:>10}{:>20}{:>20}{:>14}{:>14}\n", index, actual, expected, distance, tolerance);
}

/// Specialization for complex float pairs, so the real and imaginary parts are printed in a single row.
template <typename V>
std::string format_mismatch_row(unsigned index, const cf_t& actual, const cf_t& expected, V distance, V tolerance)
{
  return fmt::format("   {:>10}{:>20}{:>20}{:>14}{:>14}\n",
                     index,
                     fmt::format("{:+.4f}{:+.4f}j", actual.real(), actual.imag()),
                     fmt::format("{:+.4f}{:+.4f}j", expected.real(), expected.imag()),
                     distance,
                     tolerance);
}

/// Specialization for a complex float actual and a complex bf16 expected.
template <typename V>
std::string format_mismatch_row(unsigned index, const cf_t& actual, const cbf16_t& expected, V distance, V tolerance)
{
  const cf_t expected_cf = to_cf(expected);
  return format_mismatch_row(index, actual, expected_cf, distance, tolerance);
}

/// Specialization for a complex bf16 actual and a complex float expected.
template <typename V>
std::string format_mismatch_row(unsigned index, const cbf16_t& actual, const cf_t& expected, V distance, V tolerance)
{
  const cf_t actual_cf = to_cf(actual);
  return format_mismatch_row(index, actual_cf, expected, distance, tolerance);
}

/// Specialization for a 16-bit integer complex actual and a complex float expected.
template <typename V>
std::string format_mismatch_row(unsigned index, const ci16_t& actual, const cf_t& expected, V distance, V tolerance)
{
  const cf_t actual_cf = to_cf(actual);
  return format_mismatch_row(index, actual_cf, expected, distance, tolerance);
}

/// Specialization for a complex float actual and a 16-bit integer complex expected.
template <typename V>
std::string format_mismatch_row(unsigned index, const cf_t& actual, const ci16_t& expected, V distance, V tolerance)
{
  const cf_t expected_cf = to_cf(expected);
  return format_mismatch_row(index, actual, expected_cf, distance, tolerance);
}

/// Specialization for two 16-bit integer complex values.
template <typename V>
std::string format_mismatch_row(unsigned index, const ci16_t& actual, const ci16_t& expected, V distance, V tolerance)
{
  return format_mismatch_row(index, to_cf(actual), to_cf(expected), distance, tolerance);
}

/// Builds the header line of the mismatch table.
template <typename T>
std::string format_mismatch_header()
{
  return fmt::format("   {:>10}{:>20}{:>20}{:>14}{:>14}\n", "index", "actual", "expected", "error", "tolerance");
}

} // namespace detail

/// \brief Compares two sequences element-wise against a tolerance.
///
/// Checks whether two sequences are equal up to an element-wise tolerance. The provided callable \c fn
/// returns a non-negative distance value for each pair of elements; an element pair is considered a
/// match when <tt>distance &lt;= tolerance</tt>.
///
/// When the sequences differ, the returned error message contains a table with up to the first
/// \c max_output mismatches and reports the maximum observed distance, to make failures easy to
/// diagnose.
///
/// \tparam T            Actual sequence element type.
/// \tparam U            Expected sequence element type.
/// \tparam F            Callable type: \c V(const T&, const U&).
/// \tparam V            Distance (and tolerance) value type.
/// \param[in] actual    First sequence to compare (actual values).
/// \param[in] expected  Second sequence to compare (expected/golden values).
/// \param[in] fn        Distance function returning a non-negative \c V for a pair of elements.
/// \param[in] tolerance Maximum allowed distance between corresponding elements.
/// \return              Success if all element pairs are within tolerance, otherwise an error message.
template <typename T, typename U, typename F, typename V>
error_type<std::string> compare_sequences(span<const T> actual, span<const U> expected, F&& fn, V tolerance)
{
  static_assert(std::is_invocable_v<F, const T&, const U&>,
                "The distance callable must be invocable as V(const T&, const U&).");
  static_assert(std::is_convertible_v<std::invoke_result_t<F, const T&, const U&>, V>,
                "The distance callable result must be convertible to the tolerance type V.");

  ocudu_assert(actual.size() == expected.size(), "Compared sequences must have the same size.");

  static constexpr size_t max_output = 10;
  std::string             msg;
  V                       max_distance = 0;
  size_t                  max_index    = 0;
  size_t                  counter      = 0;

  bool are_equal = true;
  for (size_t index = 0, size = actual.size(); index != size; ++index) {
    V distance = static_cast<V>(std::forward<F>(fn)(actual[index], expected[index]));
    if (distance <= tolerance) {
      continue;
    }

    are_equal = false;
    if (distance > max_distance) {
      max_distance = distance;
      max_index    = index;
    }
    if (counter < max_output) {
      msg += detail::format_mismatch_row(
          static_cast<unsigned>(index), actual[index], expected[index], distance, tolerance);
      ++counter;
    }
  }

  if (are_equal) {
    return default_success_t{};
  }

  std::string output_msg = "   The compared sequences are not equal.\n    Failure table";
  if (counter >= max_output) {
    output_msg += fmt::format(" (first {} mismatches shown)", max_output);
  }
  output_msg += ":\n";
  output_msg += detail::format_mismatch_header<T>();
  output_msg += msg;
  output_msg += fmt::format("\n    Max error is {} at index {}.\n", max_distance, max_index);
  return make_unexpected(output_msg);
}

/// \brief Compares two sequences for exact equality.
///
/// Uses a distance of \c 0 when the elements compare equal and \c 1 otherwise (tolerance \c 0).
/// Useful for sequences of discrete types (e.g. bool, integers, log_likelihood_ratio, ...).
template <typename T, typename U>
error_type<std::string> compare_sequences(span<const T> actual, span<const U> expected)
{
  return compare_sequences(
      std::move(actual), std::move(expected), [](const T& lhs, const U& rhs) -> int { return lhs == rhs ? 0 : 1; }, 0);
}

} // namespace ocudu
