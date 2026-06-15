// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "ocudu/ocuduvec/sc_prod.h"
#include "ocudu/support/ocudu_test.h"
#include <gtest/gtest.h>
#include <random>

static std::mt19937 rgen(0);
static const float  ASSERT_MAX_ERROR = 1e-5;

using namespace ocudu;

namespace {
using OcuduvecScProdFixtureParams = unsigned;

class OcuduvecScProdFixture : public ::testing::TestWithParam<OcuduvecScProdFixtureParams>
{
protected:
  static std::uniform_real_distribution<float> random_dist;
  unsigned                                     size;

  void SetUp() override
  {
    // Get test parameters.
    auto params = GetParam();
    size        = params;
  }

  template <typename Type>
  std::vector<Type> generate_complex_random_data() const
  {
    std::vector<Type> data;
    data.reserve(size);
    std::generate_n(std::back_inserter(data), size, []() { return cf_t{random_dist(rgen), random_dist(rgen)}; });

    return data;
  }

  std::vector<float> generate_real_random_data() const
  {
    std::vector<float> data;
    data.reserve(size);
    std::generate_n(std::back_inserter(data), size, []() { return random_dist(rgen); });

    return data;
  }

  cf_t get_random_complex_coeff() const { return cf_t{random_dist(rgen), random_dist(rgen)}; }

  float get_random_real_coeff() const { return random_dist(rgen); }
};

std::uniform_real_distribution<float> OcuduvecScProdFixture::random_dist(-1.0, 1.0);

using namespace ocudu;

TEST_P(OcuduvecScProdFixture, OcuduvecScProdFloat)
{
  std::vector<float> x = generate_real_random_data();
  float              h = get_random_real_coeff();

  std::vector<float> z(size);

  ocuduvec::sc_prod(z, x, h);

  std::vector<float> expected(size);
  std::transform(x.begin(), x.end(), expected.begin(), [h](float value) { return h * value; });

  for (size_t i = 0; i != size; i++) {
    float err = std::abs(expected[i] - z[i]);
    ASSERT_LT(err, ASSERT_MAX_ERROR) << fmt::format("expected={} z={}", expected[i], z[i]);
  }
}

TEST_P(OcuduvecScProdFixture, OcuduvecScProdFloatComplex)
{
  std::vector<cf_t> x = generate_complex_random_data<cf_t>();
  cf_t              h = get_random_complex_coeff();

  std::vector<cf_t> z(size);

  ocuduvec::sc_prod(z, x, h);

  std::vector<cf_t> expected(size);
  std::transform(x.begin(), x.end(), expected.begin(), [h](cf_t value) { return h * value; });

  for (size_t i = 0; i != size; i++) {
    float err = std::abs(expected[i] - z[i]);
    ASSERT_LT(err, ASSERT_MAX_ERROR) << fmt::format("expected={} z={}", expected[i], z[i]);
  }
}

TEST_P(OcuduvecScProdFixture, OcuduvecScProdFloatComplexReal)
{
  std::vector<cf_t> x = generate_complex_random_data<cf_t>();
  float             h = get_random_real_coeff();

  std::vector<cf_t> z(size);

  ocuduvec::sc_prod(z, x, h);

  std::vector<cf_t> expected(size);
  std::transform(x.begin(), x.end(), expected.begin(), [h](cf_t value) { return h * value; });

  for (size_t i = 0; i != size; i++) {
    float err = std::abs(expected[i] - z[i]);
    ASSERT_LT(err, ASSERT_MAX_ERROR) << fmt::format("expected={} z={}", expected[i], z[i]);
  }
}

TEST_P(OcuduvecScProdFixture, OcuduvecScProdBrainFloatComplex)
{
  std::vector<cbf16_t> x = generate_complex_random_data<cbf16_t>();
  cf_t                 h = get_random_complex_coeff();

  std::vector<cbf16_t> z(size);

  ocuduvec::sc_prod(z, x, h);

  std::vector<cbf16_t> expected(size);
  std::transform(x.begin(), x.end(), expected.begin(), [h](cbf16_t value) { return h * to_cf(value); });

  for (size_t i = 0; i != size; i++) {
    float err = std::abs(to_cf(expected[i]) - to_cf(z[i]));
    ASSERT_LT(err, ASSERT_MAX_ERROR) << fmt::format("expected={} z={}", expected[i], z[i]);
  }
}

TEST_P(OcuduvecScProdFixture, OcuduvecScProdMixFloatComplex)
{
  std::vector<cf_t> x = generate_complex_random_data<cf_t>();
  cf_t              h = get_random_complex_coeff();

  std::vector<cbf16_t> z(size);

  ocuduvec::sc_prod(z, x, h);

  std::vector<cbf16_t> expected(size);
  std::transform(x.begin(), x.end(), expected.begin(), [h](cf_t value) { return h * to_cf(value); });

  for (size_t i = 0; i != size; i++) {
    float err = std::abs(to_cf(expected[i]) - to_cf(z[i]));
    ASSERT_LT(err, ASSERT_MAX_ERROR) << fmt::format("expected={} z={}", expected[i], z[i]);
  }
}

TEST_P(OcuduvecScProdFixture, OcuduvecScProdMixFloatComplexReal)
{
  std::vector<cf_t> x = generate_complex_random_data<cf_t>();
  float             h = get_random_real_coeff();

  std::vector<cbf16_t> z(size);

  ocuduvec::sc_prod(z, x, h);

  std::vector<cbf16_t> expected(size);
  std::transform(x.begin(), x.end(), expected.begin(), [h](cf_t value) { return h * to_cf(value); });

  for (size_t i = 0; i != size; i++) {
    float err = std::abs(to_cf(expected[i]) - to_cf(z[i]));
    ASSERT_LT(err, ASSERT_MAX_ERROR) << fmt::format("expected={} z={}", expected[i], z[i]);
  }
}

TEST_P(OcuduvecScProdFixture, OcuduvecScProdBrainFloatComplexReal)
{
  std::vector<cbf16_t> x = generate_complex_random_data<cbf16_t>();
  float                h = get_random_real_coeff();

  std::vector<cbf16_t> z(size);

  ocuduvec::sc_prod(z, x, h);

  std::vector<cbf16_t> expected(size);
  std::transform(x.begin(), x.end(), expected.begin(), [h](cbf16_t value) { return h * to_cf(value); });

  for (size_t i = 0; i != size; i++) {
    float err = std::abs(to_cf(expected[i]) - to_cf(z[i]));
    ASSERT_LT(err, ASSERT_MAX_ERROR) << fmt::format("expected={} z={}", expected[i], z[i]);
  }
}

TEST_P(OcuduvecScProdFixture, OcuduvecScAndAddProdFloat)
{
  std::vector<float> x = generate_real_random_data();
  std::vector<float> y = generate_real_random_data();
  float              h = get_random_real_coeff();

  std::vector<float> z(size);

  ocuduvec::sc_prod_and_add(z, x, y, h);

  std::vector<float> expected(size);
  std::transform(x.begin(), x.end(), y.begin(), expected.begin(), [h](float a, float b) { return h * a + b; });

  for (size_t i = 0; i != size; i++) {
    float err = std::abs(expected[i] - z[i]);
    ASSERT_LT(err, ASSERT_MAX_ERROR) << fmt::format("expected={} z={}", expected[i], z[i]);
  }
}

TEST_P(OcuduvecScProdFixture, OcuduvecScProdAndAddFloatComplex)
{
  std::vector<cf_t> x = generate_complex_random_data<cf_t>();
  std::vector<cf_t> y = generate_complex_random_data<cf_t>();
  cf_t              h = get_random_complex_coeff();

  std::vector<cf_t> z(size);

  ocuduvec::sc_prod_and_add(z, x, y, h);

  std::vector<cf_t> expected(size);
  std::transform(
      x.begin(), x.end(), y.begin(), expected.begin(), [h](cf_t a, cf_t b) { return h * to_cf(a) + to_cf(b); });

  for (size_t i = 0; i != size; i++) {
    float err = std::abs(expected[i] - z[i]);
    ASSERT_LT(err, ASSERT_MAX_ERROR) << fmt::format("expected={} z={}", expected[i], z[i]);
  }
}

TEST_P(OcuduvecScProdFixture, OcuduvecScProdAndAddFloatComplexReal)
{
  std::vector<cf_t> x = generate_complex_random_data<cf_t>();
  std::vector<cf_t> y = generate_complex_random_data<cf_t>();
  float             h = get_random_real_coeff();

  std::vector<cf_t> z(size);

  ocuduvec::sc_prod_and_add(z, x, y, h);

  std::vector<cf_t> expected(size);
  std::transform(
      x.begin(), x.end(), y.begin(), expected.begin(), [h](cf_t a, cf_t b) { return h * to_cf(a) + to_cf(b); });

  for (size_t i = 0; i != size; i++) {
    float err = std::abs(expected[i] - z[i]);
    ASSERT_LT(err, ASSERT_MAX_ERROR) << fmt::format("expected={} z={}", expected[i], z[i]);
  }
}

TEST_P(OcuduvecScProdFixture, OcuduvecScProdAndAddBrainFloatComplex)
{
  std::vector<cbf16_t> x = generate_complex_random_data<cbf16_t>();
  std::vector<cbf16_t> y = generate_complex_random_data<cbf16_t>();
  cf_t                 h = get_random_complex_coeff();

  std::vector<cbf16_t> z(size);

  ocuduvec::sc_prod_and_add(z, x, y, h);

  std::vector<cbf16_t> expected(size);
  std::transform(
      x.begin(), x.end(), y.begin(), expected.begin(), [h](cbf16_t a, cbf16_t b) { return h * to_cf(a) + to_cf(b); });

  for (size_t i = 0; i != size; i++) {
    float err = std::abs(to_cf(expected[i]) - to_cf(z[i]));
    ASSERT_LT(err, ASSERT_MAX_ERROR);
  }
}

TEST_P(OcuduvecScProdFixture, OcuduvecScProdAndAddBrainFloatComplexReal)
{
  std::vector<cbf16_t> x = generate_complex_random_data<cbf16_t>();
  std::vector<cbf16_t> y = generate_complex_random_data<cbf16_t>();
  float                h = get_random_real_coeff();

  std::vector<cbf16_t> z(size);

  ocuduvec::sc_prod_and_add(z, x, y, h);

  std::vector<cbf16_t> expected(size);
  std::transform(
      x.begin(), x.end(), y.begin(), expected.begin(), [h](cbf16_t a, cbf16_t b) { return h * to_cf(a) + to_cf(b); });

  for (size_t i = 0; i != size; i++) {
    float err = std::abs(to_cf(expected[i]) - to_cf(z[i]));
    ASSERT_LT(err, ASSERT_MAX_ERROR) << fmt::format("expected={} z={}", expected[i], z[i]);
  }
}

TEST_P(OcuduvecScProdFixture, OcuduvecScProdAndAddMixFloatComplex)
{
  std::vector<cbf16_t> x = generate_complex_random_data<cbf16_t>();
  std::vector<cf_t>    y = generate_complex_random_data<cf_t>();
  cf_t                 h = get_random_complex_coeff();

  std::vector<cf_t> z(size);

  ocuduvec::sc_prod_and_add(z, x, y, h);

  std::vector<cf_t> expected(size);
  std::transform(x.begin(), x.end(), y.begin(), expected.begin(), [h](cbf16_t a, cf_t b) { return h * to_cf(a) + b; });

  for (size_t i = 0; i != size; i++) {
    float err = std::abs(to_cf(expected[i]) - to_cf(z[i]));
    ASSERT_LT(err, ASSERT_MAX_ERROR) << fmt::format("expected={} z={}", expected[i], z[i]);
  }
}

TEST_P(OcuduvecScProdFixture, OcuduvecScProdAndAddMixFloatComplexReal)
{
  std::vector<cbf16_t> x = generate_complex_random_data<cbf16_t>();
  std::vector<cf_t>    y = generate_complex_random_data<cf_t>();
  float                h = get_random_real_coeff();

  std::vector<cf_t> z(size);

  ocuduvec::sc_prod_and_add(z, x, y, h);

  std::vector<cf_t> expected(size);
  std::transform(x.begin(), x.end(), y.begin(), expected.begin(), [h](cbf16_t a, cf_t b) { return h * to_cf(a) + b; });

  for (size_t i = 0; i != size; i++) {
    float err = std::abs(to_cf(expected[i]) - to_cf(z[i]));
    ASSERT_LT(err, ASSERT_MAX_ERROR) << fmt::format("expected={} z={}", expected[i], z[i]);
  }
}

INSTANTIATE_TEST_SUITE_P(OcuduvecScProdTest, OcuduvecScProdFixture, ::testing::Values(1, 5, 7, 19, 23, 257, 1234));

} // namespace
