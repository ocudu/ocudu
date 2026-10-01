// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

/// \file
/// \brief Circular shifts.

#pragma once

#include "ocudu/ocuduvec/type_traits.h"
#include "ocudu/ocuduvec/types.h"

namespace ocudu {
namespace ocuduvec {

/// \brief Circularly shifts a sequence in the forward direction.
///
/// The element at position \c i is moved to position <tt>i + shift</tt>. Elements that fall beyond the end of the
/// sequence are reintroduced at its start.
/// \tparam     T         Type of the output sequence (must be compatible with a span).
/// \tparam     U         Type of the input sequence (must be compatible with a span).
/// \param[out] out       Shifted output sequence.
/// \param[in]  in        Original input sequence.
/// \param[in]  shift     The number of positions the sequence is shifted by.
/// \remark Cannot be used to override memory.
/// \warning An assertion is triggered if input and output vectors have different sizes.
template <typename T, typename U>
void circ_shift_forward(T&& out, const U& in, unsigned shift)
{
  static_assert(is_span_compatible<T>::value, "Template type is not compatible with a span");
  static_assert(is_span_compatible<U>::value, "Template type is not compatible with a span");
  ocudu_ocuduvec_assert_size(out, in);

  unsigned length = out.size();
  ocudu_assert(std::abs(in.data() - out.data()) >= length, "Input and output memory overlap.");

  std::copy_n(in.begin() + length - shift, shift, out.begin());
  std::copy_n(in.begin(), length - shift, out.begin() + shift);
}

/// \brief Circularly shifts a sequence in the backward direction.
///
/// The element at position \c i is moved to position <tt>i - shift</tt>. Elements that fall beyond the beginning of the
/// sequence are reintroduced at its end.
/// \tparam     T         Type of the output sequence (must be compatible with a span).
/// \tparam     U         Type of the input sequence (must be compatible with a span).
/// \param[out] out       Shifted output sequence.
/// \param[in]  in        Original input sequence.
/// \param[in]  shift     The number of positions the sequence is shifted by.
/// \remark Cannot be used to override memory.
/// \warning An assertion is triggered if input and output vectors have different sizes.
template <typename T, typename U>
void circ_shift_backward(T&& out, const U& in, unsigned shift)
{
  static_assert(is_span_compatible<T>::value, "Template type is not compatible with a span");
  static_assert(is_span_compatible<U>::value, "Template type is not compatible with a span");
  ocudu_ocuduvec_assert_size(out, in);

  unsigned length = out.size();
  ocudu_assert(std::abs(in.data() - out.data()) >= length, "Input and output memory overlap.");

  std::copy_n(in.begin() + shift, length - shift, out.begin());
  std::copy_n(in.begin(), shift, out.begin() + length - shift);
}

} // namespace ocuduvec
} // namespace ocudu
