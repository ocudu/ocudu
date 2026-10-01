// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

/// \file
/// \brief Dot product declarations.

#pragma once

#include "ocudu/adt/span.h"
#include "ocudu/ocuduvec/type_traits.h"
#include "ocudu/ocuduvec/types.h"
#include "ocudu/support/ocudu_assert.h"
#include <numeric>

namespace ocudu {
namespace ocuduvec {

/// \brief Dot product of two spans.
///
/// Computes the dot product (a.k.a. inner product or scalar product) of the two sequences represented by the input
/// spans, adding an initial offset.
/// \tparam T         A span of an arithmetic type.
/// \tparam U         A span of an arithmetic type.
/// \tparam V         Output type (must be compatible with the product of object of type \c T and \c U).
/// \param[in] x      First span.
/// \param[in] y      Second span.
/// \param[in] init   Initialization value.
/// \return The dot product between the two spans plus \c init, i.e. \f$ x \cdot y + \textup{init} = \sum_i x_i y_i +
/// \textup{init}\f$.
/// \remark The two input spans must have the same length.
template <typename T, typename U, typename V>
inline V dot_prod(const T& x, const U& y, V init)
{
  static_assert(is_arithmetic_span_compatible<T>::value, "Template type is not compatible with a span of arithmetics");
  static_assert(is_arithmetic_span_compatible<U>::value, "Template type is not compatible with a span of arithmetics");
  ocudu_ocuduvec_assert_size(x, y);
  return std::inner_product(x.begin(), x.end(), y.begin(), init);
}

/// \brief Dot product of two complex spans.
///
/// Computes the dot product (a.k.a. inner product or scalar product) of the two complex sequences. The sequences are
/// represented by the input spans \c x and \c y, the second sequence \c y is conjugated.
///
/// The implementation is equivalent to:
/// \code
/// cf_t dot_prod(span<const cf_t> x, span<const cf_t> y) {
///   return std::inner_product(x.begin(), x.end(), y.begin(), cf_t(0.0F), std::plus<>(), [](cf_t a, cf_t b) { return a
///   * std::conj(b); });
/// }
/// \endcode
///
/// \param[in] x      First span.
/// \param[in] y      Second span.
/// \return The dot product between the two spans, i.e. \f$ x \cdot \conj{y}= \sum_i x_i \conj{y}_i \f$.
/// \remark The two input spans must have the same length.
cf_t dot_prod(span<const cf_t> x, span<const cf_t> y);

/// Dot product of two complex spans, the first span is single precision and the second brain float
cf_t dot_prod(span<const cf_t> x, span<const cbf16_t> y);

/// Dot product of two complex spans, the first span is brain float and the second single precision.
cf_t dot_prod(span<const cbf16_t> x, span<const cf_t> y);

/// \brief Estimates the average power of a complex span - linear scale.
///
/// The average power of a span is defined as its squared Euclidean norm divided by the number of its elements, i.e.
/// <tt>dot_prod(x, x) / x.size()</tt>.
float average_power(span<const cf_t> x);

/// \brief Estimates the average power of a complex span - linear scale.
///
/// The average power of a span is defined as its squared Euclidean norm divided by the number of its elements, i.e.
/// <tt>dot_prod(x, x) / x.size()</tt>.
float average_power(span<const cbf16_t> x);

/// \brief Estimates the average power of a complex int16 span - linear scale.
///
/// The result is normalized by \c scale, i.e. it matches the average power of the samples converted to complex float
/// using the same scaling factor.
float average_power(span<const ci16_t> x, float scale);

} // namespace ocuduvec
} // namespace ocudu
