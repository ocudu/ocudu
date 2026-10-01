// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "ocudu/support/math/curve_fitting_find_max.h"
#include "ocudu/ocuduvec/dot_prod.h"

namespace ocudu {

float curve_fitting_fractional_max(span<const float> samples)
{
  // Calculation coefficients for solving the equations.
  static constexpr std::array<float, 5> num_weights_5 = {{-0.400000, -0.200000, 0.000000, 0.200000, 0.400000}};
  static constexpr std::array<float, 5> den_weights_5 = {{0.571429, -0.285714, -0.571429, -0.285714, 0.571429}};
  static constexpr std::array<float, 3> num_weights_3 = {{-0.5, 0.0, 0.5}};
  static constexpr std::array<float, 3> den_weights_3 = {{0.5, -1.0, 0.5}};

  float             correction = 1.0F;
  span<const float> num_weights;
  span<const float> den_weights;

  // Select weight depending on the number of samples.
  if (samples.size() == 5) {
    num_weights = num_weights_5;
    den_weights = den_weights_5;
  } else if (samples.size() == 3) {
    correction  = 0.5;
    num_weights = num_weights_3;
    den_weights = den_weights_3;
  } else {
    // The size is invalid.
    return 0.0F;
  }

  // Solve equation.
  float num    = ocuduvec::dot_prod(num_weights, samples, 0.0F);
  float den    = ocuduvec::dot_prod(den_weights, samples, 0.0F);
  float result = -correction * num / den;

  // Make sure the function does not return a result greater than one, lower than minus one, infinity or NaN.
  if (std::isnan(result) || std::isinf(result) || std::abs(result) > 1.0F) {
    return 0.0F;
  }

  return result;
}

} // namespace ocudu
