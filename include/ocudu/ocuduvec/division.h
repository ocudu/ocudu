// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/ocuduvec/types.h"

namespace ocudu {
namespace ocuduvec {

/// \brief Element-wise division between two sequences.
///
/// The result of the division is zero if the numerator or the denominator is abnormal.
///
/// \param[out] result      Division result.
/// \param[in]  numerator   Division numerator.
/// \param[in]  denominator Division denominator.
/// \remark An assertion is triggered if \c numerator, \c denominator, and \c result do not have the same number of
/// elements.
void divide(span<float> result, span<const float> numerator, span<const float> denominator);

} // namespace ocuduvec
} // namespace ocudu
