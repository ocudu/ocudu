// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "ocudu/ocuduvec/compare.h"
#include "ocudu/ocuduvec/conversion.h"
#include "ocudu/support/math/complex_math_utils.h"
#include <gtest/gtest.h>
#include <limits>
#include <random>

static std::mt19937 rgen(0);

using namespace ocudu;

namespace {

using OcuduvecCompareParams = unsigned;

class OcuduvecCompareFixture : public ::testing::TestWithParam<OcuduvecCompareParams>
{
protected:
  unsigned size;

  void SetUp() override
  {
    auto params = GetParam();
    size        = params;
  }
};

TEST_P(OcuduvecCompareFixture, OcuduvecCompareTestMaxAbsCcc)
{
  std::uniform_real_distribution<float> dist(-1.0, 1.0);

  std::vector<cf_t> x(size);
  for (cf_t& v : x) {
    v = {dist(rgen), dist(rgen)};
  }

  std::pair<unsigned, float> result = ocuduvec::max_abs_element(x);

  auto     expected_it = std::max_element(x.begin(), x.end(), [](cf_t a, cf_t b) { return abs_sq(a) < abs_sq(b); });
  unsigned expected_max_index = static_cast<unsigned>(expected_it - x.begin());
  float    expected_max_value = abs_sq(*expected_it);

  ASSERT_EQ(expected_max_index, result.first);
  ASSERT_LT(std::abs(expected_max_value - result.second), 1e-6F);
}

TEST_P(OcuduvecCompareFixture, OcuduvecCompareTestMaxAbsCccSame)
{
  std::vector<cf_t> x(size);
  std::fill(x.begin(), x.end(), 0);

  std::pair<unsigned, float> result = ocuduvec::max_abs_element(x);

  ASSERT_EQ(0U, result.first);
  ASSERT_EQ(0.0F, result.second);
}

TEST_P(OcuduvecCompareFixture, OcuduvecCompareTestMaxF)
{
  std::uniform_real_distribution<float> dist(-1.0, 1.0);

  std::vector<float> x(size);
  for (float& v : x) {
    v = dist(rgen);
  }

  std::pair<unsigned, float> result = ocuduvec::max_element(x);

  auto     expected_it        = std::max_element(x.begin(), x.end());
  unsigned expected_max_index = static_cast<unsigned>(expected_it - x.begin());
  float    expected_max_value = *expected_it;

  ASSERT_EQ(expected_max_index, result.first);
  ASSERT_EQ(expected_max_value, result.second);
}

TEST_P(OcuduvecCompareFixture, OcuduvecCompareTestMaxFSame)
{
  std::vector<float> x(size);
  std::fill(x.begin(), x.end(), 0);

  std::pair<unsigned, float> result = ocuduvec::max_element(x);

  ASSERT_EQ(0U, result.first);
  ASSERT_EQ(0.0F, result.second);
}

TEST_P(OcuduvecCompareFixture, OcuduvecCompareTestCountIfPartAbsGreaterThan)
{
  float                                 threshold = 0.5;
  std::uniform_real_distribution<float> dist(-1.0, 1.0);

  std::vector<cf_t> x(size);
  std::generate(x.begin(), x.end(), [&dist]() { return cf_t{dist(rgen), dist(rgen)}; });

  unsigned result = ocuduvec::count_if_part_abs_greater_than(x, threshold);

  unsigned expected = std::count_if(x.begin(), x.end(), [threshold](cf_t sample) {
    return (std::abs(sample.real()) > threshold) || (std::abs(sample.imag()) > threshold);
  });

  ASSERT_EQ(expected, result);
}

TEST_P(OcuduvecCompareFixture, OcuduvecCompareTestMaxAbsCi16)
{
  constexpr float                    scale = static_cast<float>(std::numeric_limits<int16_t>::max());
  std::uniform_int_distribution<int> dist(-30000, 30000);

  std::vector<ci16_t> x(size);
  for (ci16_t& v : x) {
    v = {static_cast<int16_t>(dist(rgen)), static_cast<int16_t>(dist(rgen))};
  }

  std::vector<cf_t> x_cf(size);
  ocuduvec::convert(x_cf, x, scale);

  auto result_ci16 = ocuduvec::max_abs_element(x, scale);
  auto result_cf   = ocuduvec::max_abs_element(x_cf);

  ASSERT_EQ(result_cf.first, result_ci16.first);
  ASSERT_LT(std::abs(result_cf.second - result_ci16.second), 1e-4F);
}

TEST_P(OcuduvecCompareFixture, OcuduvecCompareTestCountIfPartAbsGreaterThanCi16)
{
  constexpr float                    scale     = static_cast<float>(std::numeric_limits<int16_t>::max());
  constexpr float                    threshold = 0.95F;
  std::uniform_int_distribution<int> dist(std::numeric_limits<int16_t>::min(), std::numeric_limits<int16_t>::max());

  std::vector<ci16_t> x(size);
  for (ci16_t& v : x) {
    v = {static_cast<int16_t>(dist(rgen)), static_cast<int16_t>(dist(rgen))};
  }

  std::vector<cf_t> x_cf(size);
  ocuduvec::convert(x_cf, x, scale);

  unsigned result_ci16 = ocuduvec::count_if_part_abs_greater_than(x, threshold, scale);
  unsigned result_cf   = ocuduvec::count_if_part_abs_greater_than(x_cf, threshold);

  ASSERT_EQ(result_cf, result_ci16);
}

TEST(OcuduvecCompareOverflowTest, MaxAbsCi16SimdExtremePairsMatchCf)
{
  constexpr float     scale           = static_cast<float>(std::numeric_limits<int16_t>::max());
  std::vector<ci16_t> extreme_samples = {{std::numeric_limits<int16_t>::max(), std::numeric_limits<int16_t>::min()},
                                         {std::numeric_limits<int16_t>::min(), std::numeric_limits<int16_t>::min()},
                                         {std::numeric_limits<int16_t>::max(), std::numeric_limits<int16_t>::max()},
                                         {std::numeric_limits<int16_t>::min(), std::numeric_limits<int16_t>::max()}};

  for (const ci16_t& sample : extreme_samples) {
    std::vector<ci16_t> x(8192, sample);
    std::vector<cf_t>   x_cf(x.size());
    ocuduvec::convert(x_cf, x, scale);

    auto result_ci16 = ocuduvec::max_abs_element(x, scale);
    auto result_cf   = ocuduvec::max_abs_element(x_cf);

    ASSERT_EQ(result_cf.first, result_ci16.first) << "sample re=" << sample.real() << " im=" << sample.imag();
    ASSERT_LT(std::abs(result_cf.second - result_ci16.second), 1e-4F)
        << "sample re=" << sample.real() << " im=" << sample.imag();
  }
}

TEST(OcuduvecCompareOverflowTest, CountIfCi16ProductionThresholdBoundary)
{
  constexpr float scale          = static_cast<float>(std::numeric_limits<int16_t>::max());
  constexpr float threshold      = 0.95F;
  int32_t         part_threshold = static_cast<int32_t>(std::lround(threshold * scale));

  std::vector<ci16_t> x = {
      {static_cast<int16_t>(part_threshold), 0},
      {static_cast<int16_t>(part_threshold + 1), 0},
      {0, static_cast<int16_t>(part_threshold)},
      {0, static_cast<int16_t>(part_threshold + 1)},
  };

  std::vector<cf_t> x_cf(x.size());
  ocuduvec::convert(x_cf, x, scale);

  unsigned result_ci16 = ocuduvec::count_if_part_abs_greater_than(x, threshold, scale);
  unsigned result_cf   = ocuduvec::count_if_part_abs_greater_than(x_cf, threshold);

  ASSERT_EQ(2U, result_ci16);
  ASSERT_GE(result_cf, result_ci16);
}

INSTANTIATE_TEST_SUITE_P(OcuduvecCompareTest,
                         OcuduvecCompareFixture,
                         ::testing::Values(1, 5, 7, 19, 23, 65, 130, 257, 1234));

} // namespace
