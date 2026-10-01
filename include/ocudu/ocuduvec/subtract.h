// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

/// \file
/// \brief Difference of two vectors.

#pragma once

#include "ocudu/ocuduvec/types.h"

namespace ocudu {
namespace ocuduvec {

///@{
/// \brief Difference of two vectors.
/// \param[out]  z Output vector.
/// \param[in]   x Minuend input vector.
/// \param[in]   y Subtrahend input vector.
/// \warning An assertion is triggered if input and output vectors have different sizes.
void subtract(span<cf_t> z, span<const cf_t> x, span<const cf_t> y);
void subtract(span<float> z, span<const float> x, span<const float> y);
void subtract(span<int16_t> z, span<const int16_t> x, span<const int16_t> y);
void subtract(span<int8_t> z, span<const int8_t> x, span<const int8_t> y);
///@}

} // namespace ocuduvec
} // namespace ocudu
