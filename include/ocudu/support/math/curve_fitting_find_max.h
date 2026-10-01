// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/adt/span.h"

namespace ocudu {

/// \brief Estimates a fractional peak from samples around a maximum.
///
/// It uses a quadratic curve fitting of the input values to estimate the peak position. The result is expressed as a
/// fraction of the sampling time with respect to the center input sample.
///
/// When the peak estimation is not meaningful, the function returns zero, meaning that one should consider the
/// maximum input as the peak and no fractional refinement is possible. This happens if:
///   - the number of input samples is neither three nor five; or
///   - the estimation results in NaN or infinity.
///
/// \param[in] samples Odd number of samples containing a peak maximum in the center.
/// \return The fractional sample estimation where the maximum is located if the result is valid.
float curve_fitting_fractional_max(span<const float> samples);

} // namespace ocudu
