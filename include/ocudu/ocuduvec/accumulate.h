// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

/// \file
/// \brief Cumulative sum of sequences.

#pragma once

#include "ocudu/adt/complex.h"
#include "ocudu/adt/span.h"
#include <numeric>

namespace ocudu {
namespace ocuduvec {

///@{
/// \brief Cumulative sum of a sequence.
/// \param[in] x  Input sequence.
/// \return The sum of all elements of the input sequence.
float accumulate(span<const float> x);

inline cf_t accumulate(span<const cf_t> x)
{
  return std::accumulate(x.begin(), x.end(), cf_t());
}
///@}

} // namespace ocuduvec
} // namespace ocudu
