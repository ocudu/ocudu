// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

/// \file
/// \brief Unit test for the clipping functions in the \c ocuduvec vector library.

#include "ocudu/ocuduvec/clip.h"
#include <gtest/gtest.h>
#include <random>

static std::mt19937 rgen(0);
static const float  ASSERT_MAX_ERROR = 1e-6;

using namespace ocudu;

namespace {

struct OcuduvecClippingParams {
  std::size_t size;
  float       max_amplitude;
  float       clip_threshold;
};

class OcuduvecClippingFixture : public ::testing::TestWithParam<OcuduvecClippingParams>
{
protected:
  std::vector<cf_t> input;
  std::size_t       nof_samples;

  void SetUp() override
  {
    auto params   = GetParam();
    nof_samples   = params.size;
    float max_amp = params.max_amplitude;

    input.resize(nof_samples);

    std::uniform_real_distribution<float> dist(-max_amp, max_amp);
    for (cf_t& v : input) {
      v = {dist(rgen), dist(rgen)};
    }
  }
};

TEST_P(OcuduvecClippingFixture, OcuduvecClippingTestMagnitude)
{
  float             clip_threshold = GetParam().clip_threshold;
  std::vector<cf_t> output(nof_samples);
  std::vector<cf_t> output_gold(nof_samples);

  unsigned nof_clipped_sps_gold = 0;
  for (std::size_t i = 0; i != nof_samples; ++i) {
    if (std::abs(input[i]) > clip_threshold) {
      output_gold[i] = std::polar(clip_threshold, std::arg(input[i]));
      ++nof_clipped_sps_gold;
    } else {
      output_gold[i] = input[i];
    }
  }

  unsigned nof_clipped_sps = ocuduvec::clip_magnitude(output, input, clip_threshold);

  for (std::size_t i = 0; i != nof_samples; ++i) {
    float err = std::abs(output_gold[i] - output[i]);
    ASSERT_LT(err, ASSERT_MAX_ERROR);
    ASSERT_EQ(nof_clipped_sps, nof_clipped_sps_gold)
        << "number of clipped samples (" << nof_clipped_sps << ") should be " << nof_clipped_sps_gold << ".";
  }
}

TEST_P(OcuduvecClippingFixture, OcuduvecClippingTestIq)
{
  float             clip_threshold = GetParam().clip_threshold;
  std::vector<cf_t> output(nof_samples);
  std::vector<cf_t> output_gold(nof_samples);

  unsigned nof_clipped_sps_gold = 0;
  for (std::size_t i = 0; i != nof_samples; ++i) {
    if (input[i].real() > clip_threshold) {
      output_gold[i].real(clip_threshold);
      ++nof_clipped_sps_gold;
    } else if (input[i].real() < -clip_threshold) {
      output_gold[i].real(-clip_threshold);
      ++nof_clipped_sps_gold;
    } else {
      output_gold[i].real(input[i].real());
    }

    if (input[i].imag() > clip_threshold) {
      output_gold[i].imag(clip_threshold);
      ++nof_clipped_sps_gold;
    } else if (input[i].imag() < -clip_threshold) {
      output_gold[i].imag(-clip_threshold);
      ++nof_clipped_sps_gold;
    } else {
      output_gold[i].imag(input[i].imag());
    }
  }

  unsigned nof_clipped_sps = ocuduvec::clip_iq(output, input, clip_threshold);

  for (std::size_t i = 0; i != nof_samples; ++i) {
    float err = std::abs(output_gold[i] - output[i]);
    ASSERT_LT(err, ASSERT_MAX_ERROR);
    ASSERT_EQ(nof_clipped_sps, nof_clipped_sps_gold)
        << "number of clipped samples (" << nof_clipped_sps << ") should be " << nof_clipped_sps_gold << ".";
  }
}

TEST_P(OcuduvecClippingFixture, OcuduvecClippingTestFloat)
{
  float             clip_threshold = GetParam().clip_threshold;
  std::vector<cf_t> output(nof_samples);
  std::vector<cf_t> output_gold(nof_samples);

  std::size_t       nof_fp_samples = nof_samples * 2;
  span<const float> input_fp       = span<const float>(reinterpret_cast<const float*>(input.data()), nof_fp_samples);
  span<float>       output_fp      = span<float>(reinterpret_cast<float*>(output.data()), nof_fp_samples);
  span<float>       output_gold_fp = span<float>(reinterpret_cast<float*>(output_gold.data()), nof_fp_samples);

  unsigned nof_clipped_sps_gold = 0;
  for (std::size_t i = 0; i != nof_fp_samples; ++i) {
    if (input_fp[i] > clip_threshold) {
      output_gold_fp[i] = clip_threshold;
      ++nof_clipped_sps_gold;
    } else if (input_fp[i] < -clip_threshold) {
      output_gold_fp[i] = -clip_threshold;
      ++nof_clipped_sps_gold;
    } else {
      output_gold_fp[i] = input_fp[i];
    }
  }

  unsigned nof_clipped_sps = ocuduvec::clip(output_fp, input_fp, clip_threshold);

  for (std::size_t i = 0; i != nof_fp_samples; ++i) {
    float err = std::abs(output_gold_fp[i] - output_fp[i]);
    ASSERT_LT(err, ASSERT_MAX_ERROR);
    ASSERT_EQ(nof_clipped_sps, nof_clipped_sps_gold)
        << "number of clipped samples (" << nof_clipped_sps << ") should be " << nof_clipped_sps_gold << ".";
  }
}

// Generate all combinations of (size, max_amplitude, clip_threshold).
static std::vector<OcuduvecClippingParams> generate_clipping_params()
{
  static const std::vector<std::size_t> sizes           = {1, 5, 7, 19, 23, 257};
  static const std::vector<float>       max_amplitudes  = {0.1F, 0.5F, 2.0F};
  static const std::vector<float>       clip_thresholds = {0.01F, 0.3F, 2.1F};

  std::vector<OcuduvecClippingParams> params;
  for (std::size_t size : sizes) {
    for (float max_amp : max_amplitudes) {
      for (float thr : clip_thresholds) {
        params.push_back({size, max_amp, thr});
      }
    }
  }
  return params;
}

INSTANTIATE_TEST_SUITE_P(OcuduvecClippingTest,
                         OcuduvecClippingFixture,
                         ::testing::ValuesIn(generate_clipping_params()));

} // namespace
