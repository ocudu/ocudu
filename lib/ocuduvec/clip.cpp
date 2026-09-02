// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "ocudu/ocuduvec/clip.h"
#include "ocudu/support/math/complex_math_utils.h"

using namespace ocudu;

unsigned ocuduvec::clip(span<float> y, span<const float> x, float threshold)
{
  ocudu_ocuduvec_assert_size(x, y);

  unsigned nof_clipped_samples = 0;
  unsigned len                 = x.size();

  // Clip the output signal.
  for (unsigned i = 0; i != len; ++i) {
    float val = x[i];
    if (val > threshold) {
      val = threshold;
      ++nof_clipped_samples;
    } else if (val < -threshold) {
      val = -threshold;
      ++nof_clipped_samples;
    }
    y[i] = val;
  }

  return nof_clipped_samples;
}

unsigned ocuduvec::clip_iq(span<cf_t> y, span<const cf_t> x, float threshold)
{
  span<const float> x_fp = span<const float>(reinterpret_cast<const float*>(x.data()), 2 * x.size());
  span<float>       y_fp = span<float>(reinterpret_cast<float*>(y.data()), 2 * x.size());
  return ocuduvec::clip(y_fp, x_fp, threshold);
}

unsigned ocuduvec::clip_magnitude(span<cf_t> y, span<const cf_t> x, float threshold)
{
  ocudu_ocuduvec_assert_size(x, y);

  unsigned nof_clipped_samples = 0;
  unsigned len                 = x.size();

  // Clip the output signal.
  for (unsigned i = 0; i != len; ++i) {
    cf_t  val   = x[i];
    float x_abs = std::sqrt(abs_sq(val));
    if (x_abs > threshold) {
      val *= threshold / x_abs;
      ++nof_clipped_samples;
    }
    y[i] = val;
  }

  return nof_clipped_samples;
}
