// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/span.h"
#include <array>

namespace ocudu {
namespace ocudu_ntn {

/// \brief Fit a quadratic curve y = c0 + c1*x + c2*x^2 to the given (x, y) data points,
///        with c0 fixed to the provided value.
///
/// This function performs a least-squares fit to determine the coefficients c1 and c2 such that
/// the sum of squared residuals between y and (c0 + c1*x + c2*x^2) is minimized, for a fixed c0.
/// The input vectors x and y must have the same length (at least 2).
/// \param x  The independent variable values.
/// \param y  The dependent variable values.
/// \param c0 The fixed coefficient for the constant term (offset).
/// \returns  An array {c0, c1, c2} of the fitted coefficients.
std::array<double, 3> fit_quadratic(span<const double> x, span<const double> y, double c0);

} // namespace ocudu_ntn
} // namespace ocudu
