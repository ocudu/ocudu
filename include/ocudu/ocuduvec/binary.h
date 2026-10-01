// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

/// \file
/// Bitwise binary operations.

#pragma once

#include "ocudu/ocuduvec/type_traits.h"
#include "ocudu/ocuduvec/types.h"

namespace ocudu {
namespace ocuduvec {

/// \brief Binary XOR.
///
/// Applies the bitwise XOR operator to corresponding elements of the input sequences.
/// \tparam T  Type of the output sequence (must be compatible with a span of integers).
/// \tparam U  Type of the first input sequence (must be compatible with a span of integers).
/// \tparam V  Type of the second input sequence (must be compatible with a span of integers).
///
/// \param[out] z  Output sequence.
/// \param[in]  x  First input sequence.
/// \param[in]  y  Second input sequence.
/// \warning An assertion is triggered if input and output sequences have different sizes.
template <typename T, typename U, typename V>
void binary_xor(T&& z, const U& x, const V& y)
{
  static_assert(is_integral_span_compatible<T>::value, "Template type is not compatible with a span of integers");
  static_assert(is_integral_span_compatible<U>::value, "Template type is not compatible with a span of integers");
  static_assert(is_integral_span_compatible<V>::value, "Template type is not compatible with a span of integers");
  ocudu_ocuduvec_assert_size(x, y);
  ocudu_ocuduvec_assert_size(x, z);

  for (std::size_t i = 0, len = x.size(); i != len; ++i) {
    z[i] = x[i] ^ y[i];
  }
}

/// \brief Binary AND.
///
/// Applies the bitwise AND operator to corresponding elements of the input sequences.
/// \tparam T  Type of the output sequence (must be compatible with a span of integers).
/// \tparam U  Type of the first input sequence (must be compatible with a span of integers).
/// \tparam V  Type of the second input sequence (must be compatible with a span of integers).
///
/// \param[out] z  Output sequence.
/// \param[in]  x  First input sequence.
/// \param[in]  y  Second input sequence.
/// \warning An assertion is triggered if input and output sequences have different sizes.
template <typename T, typename U, typename V>
void binary_and(T&& z, const U& x, const V& y)
{
  static_assert(is_integral_span_compatible<T>::value, "Template type is not compatible with a span of integers");
  static_assert(is_integral_span_compatible<U>::value, "Template type is not compatible with a span of integers");
  static_assert(is_integral_span_compatible<V>::value, "Template type is not compatible with a span of integers");
  ocudu_ocuduvec_assert_size(x, y);
  ocudu_ocuduvec_assert_size(x, z);

  for (std::size_t i = 0, len = x.size(); i != len; ++i) {
    z[i] = x[i] & y[i];
  }
}

/// \brief Binary OR.
///
/// Applies the bitwise OR operator to corresponding elements of the input sequences.
/// \tparam T  Type of the output sequence (must be compatible with a span of integers).
/// \tparam U  Type of the first input sequence (must be compatible with a span of integers).
/// \tparam V  Type of the second input sequence (must be compatible with a span of integers).
///
/// \param[out] z  Output sequence.
/// \param[in]  x  First input sequence.
/// \param[in]  y  Second input sequence.
/// \warning An assertion is triggered if input and output sequences have different sizes.
template <typename T, typename U, typename V>
void binary_or(T&& z, const U& x, const V& y)
{
  static_assert(is_integral_span_compatible<T>::value, "Template type is not compatible with a span of integers");
  static_assert(is_integral_span_compatible<U>::value, "Template type is not compatible with a span of integers");
  static_assert(is_integral_span_compatible<V>::value, "Template type is not compatible with a span of integers");
  ocudu_ocuduvec_assert_size(x, y);
  ocudu_ocuduvec_assert_size(x, z);

  for (std::size_t i = 0, len = x.size(); i != len; ++i) {
    z[i] = x[i] | y[i];
  }
}

} // namespace ocuduvec
} // namespace ocudu
