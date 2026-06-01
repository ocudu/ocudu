// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "ocudu/ocuduvec/dot_prod.h"
#include "fmt/ostream.h"
#include <gtest/gtest.h>
#include <random>

static std::mt19937 rgen(0);
static const float  ASSERT_MAX_ERROR = 1e-5;

using namespace ocudu;

namespace {

using OcuduvecDotProdParams = unsigned;

class OcuduvecDotProdFixture : public ::testing::TestWithParam<OcuduvecDotProdParams>
{
protected:
  unsigned size;

  void SetUp() override
  {
    auto params = GetParam();
    size        = params;
  }
};

TEST_P(OcuduvecDotProdFixture, OcuduvecDotProdTestCcc)
{
  std::uniform_real_distribution<float> dist(-1.0, 1.0);

  std::vector<cf_t> x(size);
  for (cf_t& v : x) {
    v = {dist(rgen), dist(rgen)};
  }

  std::vector<cf_t> y(size);
  for (cf_t& v : y) {
    v = {dist(rgen), dist(rgen)};
  }

  cf_t z = ocuduvec::dot_prod(x, y);

  cf_t gold_z = std::inner_product(
      x.begin(), x.end(), y.begin(), cf_t(0.0F), std::plus<>(), [](cf_t a, cf_t b) { return a * std::conj(b); });

  float err = std::abs(z - gold_z);
  ASSERT_LT(err, ASSERT_MAX_ERROR) << fmt::format(
      "Error {} is too high (max {}) for size of {} samples. Expected z={} but got z={}.",
      err,
      ASSERT_MAX_ERROR,
      size,
      gold_z,
      z);
}

TEST_P(OcuduvecDotProdFixture, OcuduvecDotProdTestCcCbf16)
{
  std::uniform_real_distribution<float> dist(-1.0, 1.0);

  std::vector<cf_t> x(size);
  for (cf_t& v : x) {
    v = {dist(rgen), dist(rgen)};
  }

  std::vector<cbf16_t> y(size);
  for (cbf16_t& v : y) {
    v = {dist(rgen), dist(rgen)};
  }

  cf_t z1 = ocuduvec::dot_prod(x, y);
  cf_t z2 = ocuduvec::dot_prod(y, x);

  cf_t gold_z1 = std::inner_product(x.begin(), x.end(), y.begin(), cf_t(0.0F), std::plus<>(), [](cf_t a, cbf16_t b) {
    return a * std::conj(to_cf(b));
  });
  cf_t gold_z2 = std::conj(z1);

  float err1 = std::abs(z1 - gold_z1);
  ASSERT_LT(err1, ASSERT_MAX_ERROR) << fmt::format(
      "Error {} is too high (max {}) for size of {} samples. Expected z={} but got z={}.",
      err1,
      ASSERT_MAX_ERROR,
      size,
      gold_z1,
      z1);

  float err2 = std::abs(z2 - gold_z2);
  ASSERT_LT(err2, ASSERT_MAX_ERROR) << fmt::format(
      "Error {} is too high (max {}) for size of {} samples. Expected z={} but got z={}.",
      err2,
      ASSERT_MAX_ERROR,
      size,
      gold_z2,
      z2);
}

TEST_P(OcuduvecDotProdFixture, OcuduvecDotProdTestAvgPowerCf)
{
  std::uniform_real_distribution<float> dist(-1.0, 1.0);

  std::vector<cf_t> x(size);
  for (cf_t& v : x) {
    v = {dist(rgen), dist(rgen)};
  }

  float z = ocuduvec::average_power(x);

  float expected =
      std::accumulate(
          x.begin(), x.end(), 0.0F, [](float acc, cf_t in) { return acc + std::real(in * std::conj(in)); }) /
      static_cast<float>(size);

  float err = std::abs(z - expected);
  ASSERT_LT(err, ASSERT_MAX_ERROR) << fmt::format(
      "Error {} is too high (max {}) for size of {} samples. Expected z={} but got z={}.",
      err,
      ASSERT_MAX_ERROR,
      size,
      expected,
      z);
}

TEST_P(OcuduvecDotProdFixture, OcuduvecDotProdTestAvgPowerCbf16)
{
  std::uniform_real_distribution<float> dist(-1.0, 1.0);

  std::vector<cbf16_t> x(size);
  for (cbf16_t& v : x) {
    v = cbf16_t(dist(rgen), dist(rgen));
  }

  float z = ocuduvec::average_power(x);

  float expected =
      std::accumulate(x.begin(),
                      x.end(),
                      0.0F,
                      [](float acc, cbf16_t in) { return acc + std::real(to_cf(in) * std::conj(to_cf(in))); }) /
      static_cast<float>(size);

  float err = std::abs(z - expected);
  ASSERT_LT(err, ASSERT_MAX_ERROR) << fmt::format(
      "Error {} is too high (max {}) for size of {} samples. Expected z={} but got z={}.",
      err,
      ASSERT_MAX_ERROR,
      size,
      expected,
      z);
}

INSTANTIATE_TEST_SUITE_P(OcuduvecDotProdTest, OcuduvecDotProdFixture, ::testing::Values(1, 5, 7, 19, 23, 65, 130, 257));

} // namespace
