// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/ocuduvec/type_traits.h"
#include "ocudu/ocuduvec/types.h"

namespace ocudu {
namespace ocuduvec {

/// \brief Sets all elements of a sequence to zero.
///
/// \tparam T Type of the sequence container, must be span-compatible.
/// \param  x Sequence container.
template <typename T>
void zero(T&& x)
{
  static_assert(is_span_compatible<T>::value, "Template type is not compatible with a span.");
  std::fill(x.begin(), x.end(), 0);
}

} // namespace ocuduvec
} // namespace ocudu
