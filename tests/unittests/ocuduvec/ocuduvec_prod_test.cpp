// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "support/compare_sequences.h"
#include "ocudu/adt/format.h"
#include "ocudu/ocuduvec/prod.h"
#include "ocudu/support/math/math_utils.h"
#include <gtest/gtest.h>
#include <random>

static std::mt19937 rgen(0);
static const float  ASSERT_MAX_ERROR            = 1e-6;
static const float  ASSERT_MAX_ERROR_BF16       = 1e-2;
static const float  ASSERT_MAX_ERROR_CEXP       = 1e-5;
static const float  ASSERT_MAX_ERROR_CBF16_CEXP = ASSERT_MAX_ERROR_BF16;
/// Note: SIMD and non-SIMD float-to-integer conversions may produce different results.
/// For instance, Intel’s intrinsics round to the nearest integer with ties to even (2.5 -> 2), while std::round rounds
/// halfway cases away from zero (2.5 -> 3). A +/-1 tolerance is applied to account for these differing rounding
/// behaviors.
static const float ASSERT_ROUNDING_MAX_ERROR = 1;
/// Maximum error for 16-bit integer complex numbers. It bounds the magnitude of a deviation of one quantization step in
/// each of the components.
static const float ASSERT_MAX_ERROR_CI16_CEXP = 2 * ASSERT_ROUNDING_MAX_ERROR;

using namespace ocudu;

/// Distance between two real values.
static const auto f_distance = [](float actual, float expected) { return std::abs(actual - expected); };

/// \brief Distance between two complex values.
///
/// The actual value is converted to complex float, so that every complex type is compared against the expected value in
/// the same units.
static const auto cf_distance = [](const auto& actual, cf_t expected) { return std::abs(to_cf(actual) - expected); };

class OcuduVecProdFixture : public ::testing::TestWithParam<unsigned>
{
protected:
  unsigned N = 0;

  void SetUp() override { N = GetParam(); }
};

TEST_P(OcuduVecProdFixture, ProdCCC)
{
  std::uniform_real_distribution<float> dist(-1.0, 1.0);

  std::vector<cf_t> x;
  x.reserve(N);
  std::generate_n(std::back_inserter(x), N, [&dist]() { return cf_t{dist(rgen), dist(rgen)}; });

  std::vector<cf_t> y;
  y.reserve(N);
  std::generate_n(std::back_inserter(y), N, [&dist]() { return cf_t{dist(rgen), dist(rgen)}; });

  std::vector<cf_t> z(N);

  ocuduvec::prod(z, x, y);

  std::vector<cf_t> expected;
  expected.reserve(N);
  std::transform(
      x.begin(), x.end(), y.begin(), std::back_inserter(expected), [](cf_t left, cf_t right) { return left * right; });

  error_type<std::string> result =
      compare_sequences(span<const cf_t>(z), span<const cf_t>(expected), cf_distance, ASSERT_MAX_ERROR);
  ASSERT_TRUE(result.has_value()) << result.error();
}

TEST_P(OcuduVecProdFixture, ProdCCCBF16)
{
  std::uniform_real_distribution<float> dist(-1.0, 1.0);

  std::vector<cf_t> x;
  x.reserve(N);
  std::generate_n(std::back_inserter(x), N, [&dist]() { return cf_t{dist(rgen), dist(rgen)}; });

  std::vector<cf_t> y;
  y.reserve(N);
  std::generate_n(std::back_inserter(y), N, [&dist]() { return cf_t{dist(rgen), dist(rgen)}; });

  std::vector<cbf16_t> z(N);

  ocuduvec::prod(z, x, y);

  std::vector<cf_t> expected;
  expected.reserve(N);
  std::transform(
      x.begin(), x.end(), y.begin(), std::back_inserter(expected), [](cf_t left, cf_t right) { return left * right; });

  error_type<std::string> result =
      compare_sequences(span<const cbf16_t>(z), span<const cf_t>(expected), cf_distance, ASSERT_MAX_ERROR_BF16);
  ASSERT_TRUE(result.has_value()) << result.error();
}

TEST_P(OcuduVecProdFixture, ProdFFF)
{
  std::uniform_real_distribution<float> dist(-1.0, 1.0);

  std::vector<float> x;
  x.reserve(N);
  std::generate_n(std::back_inserter(x), N, [&dist]() { return dist(rgen); });

  std::vector<float> y;
  y.reserve(N);
  std::generate_n(std::back_inserter(y), N, [&dist]() { return dist(rgen); });

  std::vector<float> z(N);

  ocuduvec::prod(z, x, y);

  std::vector<float> expected;
  expected.reserve(N);
  std::transform(x.begin(), x.end(), y.begin(), std::back_inserter(expected), [](float left, float right) {
    return left * right;
  });

  error_type<std::string> result =
      compare_sequences(span<const float>(z), span<const float>(expected), f_distance, ASSERT_MAX_ERROR);
  ASSERT_TRUE(result.has_value()) << result.error();
}

TEST_P(OcuduVecProdFixture, ProdCexp)
{
  std::uniform_real_distribution<float> dist(-1.0, 1.0);

  std::vector<cf_t> x;
  x.reserve(N);
  std::generate_n(std::back_inserter(x), N, [&dist]() { return cf_t{dist(rgen), dist(rgen)}; });

  float cfo           = dist(rgen);
  float initial_phase = M_PI * dist(rgen);

  std::vector<cf_t> z(N);

  ocuduvec::prod_cexp(z, x, cfo, initial_phase);

  cf_t osc   = std::exp(std::complex<float>(0.0F, TWOPI * cfo));
  cf_t phase = std::polar(1.0F, initial_phase);

  std::vector<cf_t> expected;
  expected.reserve(N);
  std::transform(x.begin(), x.end(), std::back_inserter(expected), [&phase, osc](const auto& value) {
    cf_t sample = to_cf(value) * phase;
    phase *= osc;
    return sample;
  });

  error_type<std::string> result =
      compare_sequences(span<const cf_t>(z), span<const cf_t>(expected), cf_distance, ASSERT_MAX_ERROR_CEXP);
  ASSERT_TRUE(result.has_value()) << result.error();
}

TEST_P(OcuduVecProdFixture, ProdCbf16Cexp)
{
  std::uniform_real_distribution<float> dist(-1.0, 1.0);

  std::vector<cbf16_t> x;
  x.reserve(N);
  std::generate_n(std::back_inserter(x), N, [&dist]() { return cbf16_t{dist(rgen), dist(rgen)}; });

  float cfo           = dist(rgen);
  float initial_phase = M_PI * dist(rgen);

  std::vector<cbf16_t> z(N);

  ocuduvec::prod_cexp(z, x, cfo, initial_phase);

  cf_t osc   = std::exp(std::complex<float>(0.0F, TWOPI * cfo));
  cf_t phase = std::polar(1.0F, initial_phase);

  std::vector<cf_t> expected;
  expected.reserve(N);
  std::transform(x.begin(), x.end(), std::back_inserter(expected), [&phase, osc](const auto& value) {
    cf_t sample = to_cf(value) * phase;
    phase *= osc;
    return sample;
  });

  error_type<std::string> result =
      compare_sequences(span<const cbf16_t>(z), span<const cf_t>(expected), cf_distance, ASSERT_MAX_ERROR_CBF16_CEXP);
  ASSERT_TRUE(result.has_value()) << result.error();
}

TEST_P(OcuduVecProdFixture, ProdCfCbf16Cexp)
{
  std::uniform_real_distribution<float> dist(-1.0, 1.0);

  std::vector<cf_t> x;
  x.reserve(N);
  std::generate_n(std::back_inserter(x), N, [&dist]() { return cf_t{dist(rgen), dist(rgen)}; });

  float cfo           = dist(rgen);
  float initial_phase = M_PI * dist(rgen);

  std::vector<cbf16_t> z(N);

  ocuduvec::prod_cexp(z, x, cfo, initial_phase);

  cf_t osc   = std::exp(std::complex<float>(0.0F, TWOPI * cfo));
  cf_t phase = std::polar(1.0F, initial_phase);

  std::vector<cf_t> expected;
  expected.reserve(N);
  std::transform(x.begin(), x.end(), std::back_inserter(expected), [&phase, osc](const auto& value) {
    cf_t sample = to_cf(value) * phase;
    phase *= osc;
    return sample;
  });

  error_type<std::string> result =
      compare_sequences(span<const cbf16_t>(z), span<const cf_t>(expected), cf_distance, ASSERT_MAX_ERROR_CBF16_CEXP);
  ASSERT_TRUE(result.has_value()) << result.error();
}

TEST_P(OcuduVecProdFixture, ProdCi16Cexp)
{
  std::uniform_real_distribution<float> dist(-1.0, 1.0);

  float scale = 1000.0F;

  std::vector<ci16_t> x;
  x.reserve(N);
  std::generate_n(std::back_inserter(x), N, [&dist, scale]() { return to_ci16(cf_t{dist(rgen), dist(rgen)} * scale); });

  float cfo           = dist(rgen);
  float initial_phase = M_PI * dist(rgen);

  std::vector<ci16_t> z(N);

  ocuduvec::prod_cexp(z, x, cfo, initial_phase);

  cf_t osc   = std::exp(std::complex<float>(0.0F, TWOPI * cfo));
  cf_t phase = std::polar(1.0F, initial_phase);

  std::vector<cf_t> expected;
  expected.reserve(N);
  std::transform(x.begin(), x.end(), std::back_inserter(expected), [&phase, osc](const auto& value) {
    cf_t sample = to_cf(value) * phase;
    phase *= osc;
    return sample;
  });

  error_type<std::string> result =
      compare_sequences(span<const ci16_t>(z), span<const cf_t>(expected), cf_distance, ASSERT_MAX_ERROR_CI16_CEXP);
  ASSERT_TRUE(result.has_value()) << result.error();
}

INSTANTIATE_TEST_SUITE_P(ocuduvec, OcuduVecProdFixture, ::testing::Values(1, 5, 7, 19, 23, 123, 257));
