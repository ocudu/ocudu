// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/adt/complex.h"
#include "ocudu/ocuduvec/type_traits.h"
#include "ocudu/support/ocudu_assert.h"
#include <numeric>

namespace ocudu {
namespace ocuduvec {

/// \brief Computes the mean of the values in the given sequence.
///
/// \tparam T Type of the input sequence, compatible with a span of arithmetic or cf_t typed values.
/// \param  x Sequence of values to compute the mean of.
/// \return   The mean value of the given sequence.
/// \remark For sequences of floating-point (including complex) values, the output type is the same as the value type
/// of the input. For integer-valued sequences, the output type is \c float.
template <typename T>
auto mean(const T& x)
{
  static_assert(is_arithmetic_span_compatible<T>::value || is_complex_span_compatible<T>::value,
                "The input type is not compatible with a span of arithmetic/cf_t values.");

  using DataType = typename T::value_type;

  using MeanType1 = typename std::conditional<is_complex<typename T::value_type>::value, DataType, float>::type;
  using MeanType  = typename std::conditional<std::is_floating_point<DataType>::value, DataType, MeanType1>::type;

  ocudu_assert(!x.empty(), "The input span cannot be empty.");

  return std::accumulate(x.begin(), x.end(), static_cast<MeanType>(0)) / static_cast<MeanType>(x.size());
}

} // namespace ocuduvec
} // namespace ocudu
