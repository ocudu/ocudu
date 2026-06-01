// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "ocudu/ocuduvec/compare.h"
#include "ocudu/support/math/math_utils.h"
#include <gtest/gtest.h>
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

INSTANTIATE_TEST_SUITE_P(OcuduvecCompareTest,
                         OcuduvecCompareFixture,
                         ::testing::Values(1, 5, 7, 19, 23, 65, 130, 257, 1234));

} // namespace
