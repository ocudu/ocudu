// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "ocudu/ocuduvec/compare.h"
#include "ocudu/support/math/curve_fitting_find_max.h"
#include "ocudu/support/math/math_utils.h"
#include <gtest/gtest.h>
#include <random>

using namespace ocudu;

using CurveFittingFindMaxParams = std::tuple<float>;

class CurveFittingFindMaxFixture : public ::testing::TestWithParam<CurveFittingFindMaxParams>
{
protected:
  /// Maximum sinusoid amplitude in the tests.
  static constexpr float max_amplitude = 1e3f;
  /// Tolerance of the fractional index estimation.
  static constexpr float ESTIMATION_TOLERANCE = 1e-2f;

  void SetUp() override
  {
    CurveFittingFindMaxParams params = GetParam();
    expected_fractional_index        = std::get<0>(params);
    amplitude                        = amplitude_dist(rgen);
  }

  float expected_fractional_index = 0;
  float amplitude                 = 0;

  static std::mt19937                          rgen;
  static std::uniform_real_distribution<float> amplitude_dist;
};

std::mt19937                          CurveFittingFindMaxFixture::rgen;
std::uniform_real_distribution<float> CurveFittingFindMaxFixture::amplitude_dist(1.0f, max_amplitude);

TEST_P(CurveFittingFindMaxFixture, FindSinMax)
{
  // Angular step of the discrete sinusoid based on the sampling frequency being sixteen times the sinusoid frequency.
  static constexpr float angular_step = (M_PI_4 / 2.0f);
  // Number of samples in one period of the sinusoid.
  static constexpr unsigned nof_samples = 16;
  // For a sinusoid sampled at an angular step of PI/8, the maximum value falls in the fifth (n = 4) sample.
  static constexpr unsigned original_max_sample = 4;
  // Sample at which the maximum will be sampled after applying the expected fractional index.
  const float expected_max_sample = original_max_sample + expected_fractional_index;

  // Generate one period of a sinusoid based on the test parameters.
  std::array<float, nof_samples> samples;
  std::generate(samples.begin(), samples.end(), [this, i = 0]() mutable {
    return amplitude * std::sin(angular_step * ((i++) - expected_fractional_index));
  });

  // Get the maximum sampled element of the sequence.
  std::pair<unsigned, float> max = ocuduvec::max_element(samples);

  // Get samples around the peak.
  static constexpr unsigned           nof_samples_peak = 5;
  std::array<float, nof_samples_peak> around_peak;
  for (unsigned i = 0; i != nof_samples_peak; ++i) {
    around_peak[i] = samples[(max.first + i + samples.size() - nof_samples_peak / 2) % samples.size()];
  }

  // Get the fractional index estimation.
  float fract_idx = curve_fitting_fractional_max(around_peak);
  // Compute the value of the sinusoid for the estimated sample.
  float estimated_max_sample = fract_idx + static_cast<float>(max.first);

  // Verify that the estimated value is as expected.
  ASSERT_NEAR(estimated_max_sample, expected_max_sample, ESTIMATION_TOLERANCE);
}

INSTANTIATE_TEST_SUITE_P(CurveFittingFindMaxTest,
                         CurveFittingFindMaxFixture,
                         ::testing::Combine(::testing::Range(-0.999f, 1.0f, 0.01)));
