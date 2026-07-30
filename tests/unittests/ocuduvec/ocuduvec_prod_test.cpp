// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

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

using namespace ocudu;

class OcuduVecProdFixture : public ::testing::TestWithParam<unsigned>
{
protected:
  unsigned N = 0;

  void SetUp() override { N = GetParam(); }
};

TEST_P(OcuduVecProdFixture, ProdCCC)
{
  std::uniform_real_distribution<float> dist(-1.0, 1.0);

  std::vector<cf_t> x(N);
  for (cf_t& v : x) {
    v = {dist(rgen), dist(rgen)};
  }

  std::vector<cf_t> y(N);
  for (cf_t& v : y) {
    v = {dist(rgen), dist(rgen)};
  }

  std::vector<cf_t> z(N);

  ocuduvec::prod(z, x, y);

  for (size_t i = 0; i != N; ++i) {
    cf_t  gold_z = x[i] * y[i];
    float err    = std::abs(gold_z - z[i]);
    ASSERT_LT(err, ASSERT_MAX_ERROR);
  }
}

TEST_P(OcuduVecProdFixture, ProdCCCBF16)
{
  std::uniform_real_distribution<float> dist(-1.0, 1.0);

  std::vector<cf_t> x(N);
  for (cf_t& v : x) {
    v = {dist(rgen), dist(rgen)};
  }

  std::vector<cf_t> y(N);
  for (cf_t& v : y) {
    v = {dist(rgen), dist(rgen)};
  }

  std::vector<cbf16_t> z(N);

  ocuduvec::prod(z, x, y);

  for (size_t i = 0; i != N; ++i) {
    cf_t  gold_z = x[i] * y[i];
    float err    = std::abs(gold_z - to_cf(z[i]));
    ASSERT_LT(err, ASSERT_MAX_ERROR_BF16);
  }
}

TEST_P(OcuduVecProdFixture, ProdFFF)
{
  std::uniform_real_distribution<float> dist(-1.0, 1.0);

  std::vector<float> x(N);
  for (float& v : x) {
    v = dist(rgen);
  }

  std::vector<float> y(N);
  for (float& v : y) {
    v = dist(rgen);
  }

  std::vector<float> z(N);

  ocuduvec::prod(z, x, y);

  for (size_t i = 0; i != N; ++i) {
    cf_t  gold_z = x[i] * y[i];
    float err    = std::abs(gold_z - z[i]);
    ASSERT_LT(err, ASSERT_MAX_ERROR);
  }
}

TEST_P(OcuduVecProdFixture, ProdCexp)
{
  std::uniform_real_distribution<float> dist(-1.0, 1.0);

  std::vector<cf_t> x(N);
  for (cf_t& v : x) {
    v = {dist(rgen), dist(rgen)};
  }

  float cfo           = dist(rgen);
  float initial_phase = M_PI * dist(rgen);

  std::vector<cf_t> z(N);

  ocuduvec::prod_cexp(z, x, cfo, initial_phase);

  cf_t osc   = std::exp(std::complex<float>(0.0F, TWOPI * cfo));
  cf_t phase = std::polar(1.0F, initial_phase);
  for (size_t i = 0; i != N; ++i) {
    cf_t gold_z = x[i] * phase;
    phase *= osc;
    float err = std::abs(gold_z - z[i]);
    ASSERT_LT(err, ASSERT_MAX_ERROR_CEXP) << fmt::format("Sample index {} do not match {} != {}.", i, gold_z, z[i]);
  }
}

TEST_P(OcuduVecProdFixture, ProdCbf16Cexp)
{
  std::uniform_real_distribution<float> dist(-1.0, 1.0);

  std::vector<cbf16_t> x(N);
  for (cbf16_t& v : x) {
    v = {dist(rgen), dist(rgen)};
  }

  float cfo           = dist(rgen);
  float initial_phase = M_PI * dist(rgen);

  std::vector<cbf16_t> z(N);

  ocuduvec::prod_cexp(z, x, cfo, initial_phase);

  cf_t osc   = std::exp(std::complex<float>(0.0F, TWOPI * cfo));
  cf_t phase = std::polar(1.0F, initial_phase);
  for (size_t i = 0; i != N; ++i) {
    cf_t gold_z = to_cf(x[i]) * phase;
    phase *= osc;
    float err = std::abs(gold_z - to_cf(z[i]));
    ASSERT_LT(err, ASSERT_MAX_ERROR_CBF16_CEXP)
        << fmt::format("Sample index {} do not match {} != {}.", i, gold_z, z[i]);
  }
}

TEST_P(OcuduVecProdFixture, ProdCfCbf16Cexp)
{
  std::uniform_real_distribution<float> dist(-1.0, 1.0);

  std::vector<cf_t> x(N);
  for (cf_t& v : x) {
    v = {dist(rgen), dist(rgen)};
  }

  float cfo           = dist(rgen);
  float initial_phase = M_PI * dist(rgen);

  std::vector<cbf16_t> z(N);

  ocuduvec::prod_cexp(z, x, cfo, initial_phase);

  cf_t osc   = std::exp(std::complex<float>(0.0F, TWOPI * cfo));
  cf_t phase = std::polar(1.0F, initial_phase);
  for (size_t i = 0; i != N; ++i) {
    cf_t gold_z = to_cf(x[i]) * phase;
    phase *= osc;
    float err = std::abs(gold_z - to_cf(z[i]));
    ASSERT_LT(err, ASSERT_MAX_ERROR_CBF16_CEXP)
        << fmt::format("Sample index {} do not match {} != {}.", i, gold_z, z[i]);
  }
}

TEST_P(OcuduVecProdFixture, ProdCi16Cexp)
{
  std::uniform_real_distribution<float> dist(-1.0, 1.0);

  float scale = 1000.0F;

  std::vector<ci16_t> x(N);
  for (ci16_t& v : x) {
    v = to_ci16(cf_t(dist(rgen), dist(rgen)) * scale);
  }

  float cfo           = dist(rgen);
  float initial_phase = M_PI * dist(rgen);

  std::vector<ci16_t> z(N);

  ocuduvec::prod_cexp(z, x, cfo, initial_phase);

  cf_t osc   = std::exp(std::complex<float>(0.0F, TWOPI * cfo));
  cf_t phase = std::polar(1.0F, initial_phase);
  for (size_t i = 0; i != N; ++i) {
    cf_t gold_z = to_cf(x[i]) * phase;
    phase *= osc;
    ASSERT_NEAR(std::round(gold_z.real()), z[i].real(), ASSERT_ROUNDING_MAX_ERROR)
        << fmt::format("Sample index {} do not match {} != {}.", i, gold_z, z[i]);
    ASSERT_NEAR(std::round(gold_z.imag()), z[i].imag(), ASSERT_ROUNDING_MAX_ERROR)
        << fmt::format("Sample index {} do not match {} != {}.", i, gold_z, z[i]);
  }
}

INSTANTIATE_TEST_SUITE_P(ocuduvec, OcuduVecProdFixture, ::testing::Values(1, 5, 7, 19, 23, 123, 257));
