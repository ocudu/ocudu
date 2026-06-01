// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "ocudu/ocuduvec/add.h"
#include <gtest/gtest.h>
#include <random>

static std::mt19937 rgen(0);
static const float  ASSERT_CF_MAX_ERROR    = 1e-6;
static const float  ASSERT_FLOAT_MAX_ERROR = 1e-6;

using namespace ocudu;

namespace {

using OcuduvecAddParams = unsigned;

class OcuduvecAddFixture : public ::testing::TestWithParam<OcuduvecAddParams>
{
protected:
  unsigned size;

  void SetUp() override
  {
    auto params = GetParam();
    size        = params;
  }
};

TEST_P(OcuduvecAddFixture, OcuduvecAddTestCf)
{
  std::uniform_real_distribution<float> dist(-1.0, 1.0);

  std::vector<cf_t> x(size);
  for (cf_t& v : x) {
    v = cf_t(dist(rgen), dist(rgen));
  }

  std::vector<cf_t> y(size);
  for (cf_t& v : y) {
    v = cf_t(dist(rgen), dist(rgen));
  }

  std::vector<cf_t> z(size);

  ocuduvec::add(z, x, y);

  for (size_t i = 0; i != size; i++) {
    cf_t  gold_z = x[i] + y[i];
    float err    = std::abs(gold_z - z[i]);
    ASSERT_LT(err, ASSERT_CF_MAX_ERROR);
  }
}

TEST_P(OcuduvecAddFixture, OcuduvecAddTestFloat)
{
  std::uniform_real_distribution<float> dist(-1.0, 1.0);

  std::vector<float> x(size);
  for (float& v : x) {
    v = dist(rgen);
  }

  std::vector<float> y(size);
  for (float& v : y) {
    v = dist(rgen);
  }

  std::vector<float> z(size);

  ocuduvec::add(z, x, y);

  for (size_t i = 0; i != size; i++) {
    float gold_z = x[i] + y[i];
    float err    = std::abs(gold_z - z[i]);
    ASSERT_LT(err, ASSERT_FLOAT_MAX_ERROR);
  }
}

TEST_P(OcuduvecAddFixture, OcuduvecAddTestInt16)
{
  std::uniform_int_distribution<int16_t> dist(INT16_MIN / 2, INT16_MAX / 2);

  std::vector<int16_t> x(size);
  for (int16_t& v : x) {
    v = dist(rgen);
  }

  std::vector<int16_t> y(size);
  for (int16_t& v : y) {
    v = dist(rgen);
  }

  std::vector<int16_t> z(size);

  ocuduvec::add(z, x, y);

  for (size_t i = 0; i != size; i++) {
    ASSERT_EQ(x[i] + y[i], z[i]);
  }
}

TEST_P(OcuduvecAddFixture, OcuduvecAddTestInt8)
{
  std::uniform_int_distribution<int8_t> dist(INT8_MIN / 2, INT8_MAX / 2);

  std::vector<int8_t> x(size);
  for (int8_t& v : x) {
    v = dist(rgen);
  }

  std::vector<int8_t> y(size);
  for (int8_t& v : y) {
    v = dist(rgen);
  }

  std::vector<int8_t> z(size);

  ocuduvec::add(z, x, y);

  for (size_t i = 0; i != size; i++) {
    ASSERT_EQ(x[i] + y[i], z[i]);
  }
}

INSTANTIATE_TEST_SUITE_P(OcuduvecAddTest, OcuduvecAddFixture, ::testing::Values(1, 5, 7, 19, 23, 257));

} // namespace
