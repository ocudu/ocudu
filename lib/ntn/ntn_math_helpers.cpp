// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ntn_math_helpers.h"
#include "ocudu/support/error_handling.h"
#include <cmath>
#include <vector>

using namespace ocudu;
using namespace ocudu_ntn;

std::array<double, 3> ocudu::ocudu_ntn::fit_quadratic(span<const double> x, span<const double> y, double c0)
{
  if (x.size() != y.size() || x.size() < 2) {
    report_error("Input vectors must have same size and at least two points.");
  }

  double Sx2 = 0.0, Sx3 = 0.0, Sx4 = 0.0;
  double Sy1 = 0.0, Sy2 = 0.0;

  for (unsigned i = 0, e = x.size(); i != e; ++i) {
    double xi = x[i];
    double yi = y[i] - c0;

    double xi2 = xi * xi;
    double xi3 = xi2 * xi;
    double xi4 = xi2 * xi2;

    Sx2 += xi2;
    Sx3 += xi3;
    Sx4 += xi4;

    Sy1 += yi * xi;
    Sy2 += yi * xi2;
  }

  // Solve linear system:
  // [Sx2  Sx3] [c1] = [Sy1]
  // [Sx3  Sx4] [c2]   [Sy2]

  double det = Sx2 * Sx4 - Sx3 * Sx3;
  if (std::abs(det) < 1e-12) {
    report_error("Singular matrix - x values may be degenerate.");
  }

  double c1 = (Sy1 * Sx4 - Sx3 * Sy2) / det;
  double c2 = (Sx2 * Sy2 - Sy1 * Sx3) / det;

  return {c0, c1, c2};
}
