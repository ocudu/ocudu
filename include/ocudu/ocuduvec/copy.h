// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

/// \file
/// \brief Sequence copy.

#pragma once

#include "ocudu/ocuduvec/type_traits.h"
#include "ocudu/ocuduvec/types.h"

namespace ocudu {
namespace ocuduvec {

/// \brief Copies a sequence into a different object.
///
/// \tparam     T    Type of the destination sequence. (must be compatible with a span).
/// \tparam     U    Type of the original sequence (must be compatible with a span).
/// \param[out] dst  Copied sequence.
/// \param[in]  src  Original sequence.
template <typename T, typename U>
void copy(T&& dst, const U& src)
{
  static_assert(is_span_compatible<T>::value, "Template type is not compatible with a span");
  static_assert(is_span_compatible<U>::value, "Template type is not compatible with a span");
  ocudu_ocuduvec_assert_size(dst, src);

  std::copy(src.begin(), src.end(), dst.begin());
}

} // namespace ocuduvec
} // namespace ocudu
