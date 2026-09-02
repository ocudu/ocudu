// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "ocudu/adt/format.h"
#include "ocudu/ocuduvec/conversion.h"
#include <fmt/ostream.h>
#include <gtest/gtest.h>
#include <random>

static std::mt19937 rgen(0);
static const float  ASSERT_CF_MAX_ERROR    = 1e-5;
static const float  ASSERT_FLOAT_MAX_ERROR = 1e-5;
/// Note: SIMD and non-SIMD float-to-integer conversions may produce different results.
/// For instance, Intel’s intrinsics round to the nearest integer with ties to even (2.5 -> 2), while std::round rounds
/// halfway cases away from zero (2.5 -> 3). A +/-1 tolerance is applied to account for these differing rounding
/// behaviors.
static const float ASSERT_ROUNDING_MAX_ERROR = 1;

using namespace ocudu;

namespace ocudu {

std::ostream& operator<<(std::ostream& os, const cbf16_t& value)
{
  fmt::print(os, "{}", value);
  return os;
}

} // namespace ocudu

namespace {

using OcuduvecConvertParams = unsigned;

class OcuduvecConvertFixture : public ::testing::TestWithParam<OcuduvecConvertParams>
{
protected:
  unsigned size;

  void SetUp() override
  {
    // Get test parameters.
    auto params = GetParam();
    size        = params;
  }
};

using namespace ocudu;

TEST_P(OcuduvecConvertFixture, OcuduvecConvertTestComplexInt16)
{
  std::uniform_real_distribution<float> dist(-1.0, 1.0);

  std::vector<cf_t> x(size);
  for (cf_t& v : x) {
    v = cf_t(dist(rgen), dist(rgen));
  }

  std::vector<int16_t> z(2 * size);

  float scale = 1000.0F;

  ocuduvec::convert(z, x, scale);

  for (size_t i = 0; i != size; ++i) {
    int16_t gold_re = static_cast<int16_t>(std::round(x[i].real() * scale));
    int16_t gold_im = static_cast<int16_t>(std::round(x[i].imag() * scale));
    ASSERT_NEAR(gold_re, z[2 * i + 0], ASSERT_ROUNDING_MAX_ERROR);
    ASSERT_NEAR(gold_im, z[2 * i + 1], ASSERT_ROUNDING_MAX_ERROR);
  }
}

TEST_P(OcuduvecConvertFixture, OcuduvecConvertTestInt16Complex)
{
  std::uniform_int_distribution<int16_t> dist(INT16_MIN, INT16_MAX);

  std::vector<int16_t> x(2 * size);
  for (int16_t& v : x) {
    v = dist(rgen);
  }

  std::vector<cf_t> z(size);

  float scale = 1000.0F;

  ocuduvec::convert(z, x, scale);

  for (size_t i = 0; i != size; ++i) {
    cf_t  gold = {static_cast<float>(x[2 * i]) / scale, static_cast<float>(x[2 * i + 1]) / scale};
    float err  = std::abs(gold - z[i]);
    ASSERT_TRUE(err < ASSERT_CF_MAX_ERROR);
  }
}

TEST_P(OcuduvecConvertFixture, OcuduvecConvertTestFloatInt16)
{
  std::uniform_real_distribution<float> dist(-1, 1);

  std::vector<float> x(size);
  for (float& v : x) {
    v = dist(rgen);
  }

  std::vector<int16_t> z(size);

  float scale = 1000.0F;

  ocuduvec::convert(z, x, scale);

  for (size_t i = 0; i != size; ++i) {
    int16_t gold = static_cast<int16_t>(std::round(x[i] * scale));
    ASSERT_NEAR(gold, z[i], ASSERT_ROUNDING_MAX_ERROR);
  }
}

TEST_P(OcuduvecConvertFixture, OcuduvecConvertTestInt16Float)
{
  std::uniform_int_distribution<int16_t> dist(INT16_MIN, INT16_MAX);

  std::vector<int16_t> x(size);
  for (int16_t& v : x) {
    v = dist(rgen);
  }

  std::vector<float> z(size);

  float scale = 1000.0F;

  ocuduvec::convert(z, x, scale);

  for (size_t i = 0; i != size; ++i) {
    float gold = static_cast<float>(x[i]) / scale;
    float err  = std::abs(gold - z[i]);
    ASSERT_TRUE(err < ASSERT_FLOAT_MAX_ERROR);
  }
}

TEST_P(OcuduvecConvertFixture, OcuduvecConvertTestComplexComplex16Random)
{
  static constexpr float                range = std::numeric_limits<float>::max() / 2;
  std::uniform_real_distribution<float> dist(-range, range);

  std::vector<cf_t> in(size);
  std::generate(in.begin(), in.end(), [&dist]() { return cf_t(dist(rgen), dist(rgen)); });

  std::vector<cf_t>    out(size);
  std::vector<cbf16_t> data_cbf16(size);

  // Convert from single precission to BF16.
  ocuduvec::convert(data_cbf16, in);

  // Convert from BF16 to single precision.
  ocuduvec::convert(out, data_cbf16);

  // Assert converstion from single precision.
  for (size_t i = 0; i != size; ++i) {
    ASSERT_EQ(data_cbf16[i], to_cbf16(in[i]));
  }

  // Assert conversion from BF16.
  for (size_t i = 0; i != size; ++i) {
    float tolerance = std::abs(in[i]) / 256.0F;
    ASSERT_LT(std::abs(in[i] - out[i]), tolerance);
  }
}

TEST_P(OcuduvecConvertFixture, OcuduvecConvertTestComplexComplex16Special)
{
  static const float nan            = std::numeric_limits<float>::quiet_NaN();
  static const float infinity       = std::numeric_limits<float>::infinity();
  static const float neg_infinity   = -infinity;
  static const float one_round_down = 1.0F + std::pow(2.0F, -8.0F);
  static const float one_round_up   = one_round_down + std::pow(2.0F, -7.0F);

  static const std::vector<float>    values          = {nan, infinity, neg_infinity, one_round_down, one_round_up};
  static const std::vector<uint16_t> expected_values = {0x7fc0, 0x7f80, 0xff80, 0x3f80, 0x3f82};

  std::vector<cf_t> in(size);
  std::generate(in.begin(), in.end(), [n = 0]() mutable {
    float re = values[(n++) % values.size()];
    float im = values[(n++) % values.size()];
    return cf_t(re, im);
  });

  std::vector<cbf16_t> data_cbf16(size);

  // Convert from single precission to BF16.
  ocuduvec::convert(data_cbf16, in);

  // Verify the converted values are equal to the expected.
  for (unsigned i = 0; i != size; ++i) {
    cbf16_t expected;
    expected.real = bf16_t(expected_values[(2 * i) % expected_values.size()]);
    expected.imag = bf16_t(expected_values[(2 * i + 1) % expected_values.size()]);
    ASSERT_EQ(expected, data_cbf16[i]);
  }
}

TEST_P(OcuduvecConvertFixture, OcuduvecConvertTestFloatFloat16Random)
{
  std::uniform_real_distribution<float> dist(-1.0, 1.0);

  std::vector<float> in(size);
  std::generate(in.begin(), in.end(), [&dist]() { return dist(rgen); });

  // Convert from single precision to brain float.
  std::vector<bf16_t> data_bf16(size);
  ocuduvec::convert(data_bf16, in);

  // Assert conversion to BF16.
  for (size_t i = 0; i != size; ++i) {
    ASSERT_EQ(data_bf16[i], to_bf16(in[i]));
  }

  // Convert back to single precision float.
  std::vector<float> out(size);
  ocuduvec::convert(out, data_bf16);

  // Assert conversion from BF16.
  for (size_t i = 0; i != size; ++i) {
    float tolerance = std::abs(in[i]) / 256.0F;
    ASSERT_LT(std::abs(in[i] - out[i]), tolerance);
  }
}

TEST_P(OcuduvecConvertFixture, OcuduvecConvertTestInt16Float16Random)
{
  std::uniform_real_distribution<float> dist(-1.0, 1.0);

  float int16_scale = (1 << 15) - 1;

  std::vector<float> in(size);
  std::generate(in.begin(), in.end(), [&dist]() { return dist(rgen); });

  // Convert from single precision to int16.
  std::vector<int16_t> in_int16(size);
  ocuduvec::convert(in_int16, in, int16_scale);

  // Convert from int16 to brain float.
  std::vector<bf16_t> data_bf16(size);
  ocuduvec::convert(data_bf16, in_int16, int16_scale);

  // Assert conversion to BF16.
  for (size_t i = 0; i != size; ++i) {
    ASSERT_EQ(data_bf16[i], to_bf16(in_int16[i], int16_scale));
  }

  // Convert from brain float back to int16.
  std::vector<int16_t> out_int16(size);
  ocuduvec::convert(out_int16, data_bf16, int16_scale);

  // Assert conversion from BF16.
  for (size_t i = 0; i != size; ++i) {
    ASSERT_EQ(out_int16[i], to_int16(data_bf16[i], int16_scale));
  }

  // Convert int16 to float and compare with original data.
  std::vector<float> out(size);
  ocuduvec::convert(out, out_int16, int16_scale);

  for (size_t i = 0; i != size; ++i) {
    float tolerance = std::abs(in[i]) / 256.0F + 1 / int16_scale;
    ASSERT_LT(std::abs(in[i] - out[i]), tolerance);
  }
}

TEST_P(OcuduvecConvertFixture, OcuduvecConvertTestScaledInt16ComplexFloat16Random)
{
  constexpr float int16_gain = 1.0 / ((1 << 15) - 1);

  std::uniform_int_distribution<int16_t> dist_i(-32768, 32767);
  std::uniform_int_distribution<int16_t> dist_f(1, 128);

  const unsigned size_i16 = size * 2;

  std::vector<int16_t> in(size_i16);
  std::vector<float>   gain(size_i16);

  std::generate(in.begin(), in.end(), [&dist_i]() { return dist_i(rgen); });
  std::generate(gain.begin(), gain.end(), [&dist_f]() { return int16_gain * float(dist_f(rgen)); });

  // Convert from int16 to brain float.
  std::vector<cbf16_t> data_cbf16(size);
  ocuduvec::convert(data_cbf16, in, gain);

  // Assert conversion to cbf16.
  for (size_t i = 0; i != size; ++i) {
    ASSERT_EQ(data_cbf16[i].real, to_bf16(in[i * 2], 1 / gain[i * 2]));
    ASSERT_EQ(data_cbf16[i].imag, to_bf16(in[i * 2 + 1], 1 / gain[i * 2 + 1]));
  }
}

TEST_P(OcuduvecConvertFixture, OcuduvecConvertTestComplexFloat16ComplexInt16Random)
{
  constexpr float scaling_factor = 1.0 / ((1 << 15) - 1);

  std::uniform_int_distribution<int16_t> dist_i(-32768, 32767);
  std::uniform_int_distribution<int16_t> dist_f(1, 128);

  std::vector<ci16_t> in(size);
  std::generate(in.begin(), in.end(), [&dist_i]() { return ci16_t(dist_i(rgen), dist_i(rgen)); });

  std::vector<ci16_t>  out(size);
  std::vector<cbf16_t> data_cbf16(size);

  // Convert from 16-bit complex integer to complex BF16.
  ocuduvec::convert(data_cbf16, in, scaling_factor);

  // Convert from complex BF16 to 16-bit complex integer.
  ocuduvec::convert(out, data_cbf16, scaling_factor);

  // Assert conversion from 16-bit complex integer to complex BF16.
  for (size_t i = 0; i != size; ++i) {
    ASSERT_EQ(data_cbf16[i], to_cbf16(cf_t(in[i].real(), in[i].imag()) / scaling_factor));
  }

  // Assert conversion from complex BF16 to 16-bit complex integer.
  for (size_t i = 0; i != size; ++i) {
    float tolerance = std::abs(in[i]) / 256.0F;
    ASSERT_LT(std::abs(in[i] - out[i]), tolerance);
  }
}

INSTANTIATE_TEST_SUITE_P(OcuduvecConvertTest, OcuduvecConvertFixture, ::testing::Values(1, 5, 7, 19, 23, 257, 1234));

} // namespace
