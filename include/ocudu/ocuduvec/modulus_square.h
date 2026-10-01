// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/ocuduvec/types.h"

namespace ocudu {
namespace ocuduvec {

/// \brief Calculates the element-wise modulus square of a sequence of complex values.
/// \param[out] result      Output non-negative real values.
/// \param[in]  input       Input complex values.
/// \remark An assertion is triggered if \c input and \c result do not have the same number of elements.
void modulus_square(span<float> result, span<const cf_t> input);

/// \brief Calculates the element-wise modulus square, as float, of a sequence of 16-bit brain float complex values.
/// \param[out] result      Output non-negative real values.
/// \param[in]  input       Input complex values.
/// \remark An assertion is triggered if \c input and \c result do not have the same number of elements.
void modulus_square(span<float> result, span<const cbf16_t> input);

/// \brief Calculates the element-wise modulus square of a sequence of complex values and adds an offset.
/// \param[out] result      Output values: <c>result[i] = input[i] * conj(input[i]) + offset[i]</c>.
/// \param[in]  input       Input complex values.
/// \param[in]  offset      Input real-valued offsets.
/// \remark An assertion is triggered if \c input, \c result and \c offset do not have the same number of elements.
void modulus_square_and_add(span<float> result, span<const cf_t> input, span<const float> offset);

/// \brief Calculates the element-wise modulus square, as float, of a sequence of 16-bit brain float complex values and
/// adds an offset.
/// \param[out] result      Output values: <c>result[i] = input[i] * conj(input[i]) + offset[i]</c>.
/// \param[in]  input       Input complex values.
/// \param[in]  offset      Input real-valued offsets.
/// \remark An assertion is triggered if \c input, \c result and \c offset do not have the same number of elements.
void modulus_square_and_add(span<float> result, span<const cbf16_t> input, span<const float> offset);

} // namespace ocuduvec
} // namespace ocudu
