// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "compare_sequences.h"
#include "ocudu/adt/format.h"
#include "ocudu/ocuduvec/conversion.h"
#include <algorithm>
#include <cmath>
#include <fmt/ostream.h>
#include <gtest/gtest.h>
#include <iterator>
#include <limits>
#include <random>
#include <vector>

static std::mt19937 rgen(0);
static const float  ASSERT_CF_MAX_ERROR    = 1e-5;
static const float  ASSERT_FLOAT_MAX_ERROR = 1e-5;
/// Maximum relative error of a BF16 conversion, given by its 8-bit mantissa.
static const float ASSERT_BF16_MAX_RELATIVE_ERROR = 1.0F / 256.0F;

/// Range of the 16-bit signed integer in single precision, for saturating the conversions.
static constexpr float int16_min_float = static_cast<float>(std::numeric_limits<int16_t>::min());
static constexpr float int16_max_float = static_cast<float>(std::numeric_limits<int16_t>::max());

/// \brief Reference saturated conversion to a 16-bit signed integer.
///
/// It rounds the halfway cases to even as the SIMD conversions, regardless of the floating-point rounding mode.
static int16_t saturate_half_even(float value)
{
  float rounded = std::round(value);
  if (std::abs(value - std::trunc(value)) == 0.5F) {
    rounded = 2.0F * std::round(value / 2.0F);
  }
  return static_cast<int16_t>(std::clamp(rounded, int16_min_float, int16_max_float));
}

/// Relative error of a real or complex value with respect to its reference.
template <typename T>
static float relative_error(T actual, T expected)
{
  if (actual == expected) {
    return 0.0F;
  }
  return std::abs(actual - expected) / std::abs(expected);
}

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

  std::vector<cf_t> x;
  x.reserve(size);
  std::generate_n(std::back_inserter(x), size, [&dist]() { return cf_t(dist(rgen), dist(rgen)); });

  std::vector<int16_t> z(2 * size);

  float scale = 1000.0F;

  ocuduvec::convert(z, x, scale);

  std::vector<int16_t> expected;
  expected.reserve(2 * size);
  for (cf_t value : x) {
    expected.push_back(saturate_half_even(value.real() * scale));
    expected.push_back(saturate_half_even(value.imag() * scale));
  }

  error_type<std::string> result = compare_sequences(span<const int16_t>(z), span<const int16_t>(expected));
  ASSERT_TRUE(result.has_value()) << result.error();
}

TEST_P(OcuduvecConvertFixture, OcuduvecConvertTestComplexInt16Saturation)
{
  // Values up to twice the 16-bit range, of both signs.
  std::uniform_real_distribution<float> dist(-2.0, 2.0);

  std::vector<cf_t> x;
  x.reserve(size);
  std::generate_n(std::back_inserter(x), size, [&dist]() { return cf_t(dist(rgen), dist(rgen)); });

  std::vector<ci16_t> z(size);

  float scale = 32767.0F;

  ocuduvec::convert(z, x, scale);

  std::vector<ci16_t> expected;
  expected.reserve(size);
  std::transform(x.begin(), x.end(), std::back_inserter(expected), [scale](cf_t value) {
    return ci16_t(saturate_half_even(value.real() * scale), saturate_half_even(value.imag() * scale));
  });

  error_type<std::string> result = compare_sequences(
      span<const ci16_t>(z),
      span<const ci16_t>(expected),
      [](ci16_t actual, ci16_t value) {
        return std::max(std::abs(static_cast<int>(actual.real()) - static_cast<int>(value.real())),
                        std::abs(static_cast<int>(actual.imag()) - static_cast<int>(value.imag())));
      },
      0);
  ASSERT_TRUE(result.has_value()) << result.error();
}

TEST_P(OcuduvecConvertFixture, OcuduvecConvertTestFloatInt16Saturation)
{
  // Values up to twice the 16-bit range, of both signs.
  std::uniform_real_distribution<float> dist(-2.0, 2.0);

  std::vector<float> x;
  x.reserve(size);
  std::generate_n(std::back_inserter(x), size, [&dist]() { return dist(rgen); });

  std::vector<int16_t> z(size);

  float scale = 32767.0F;

  ocuduvec::convert(z, x, scale);

  std::vector<int16_t> expected;
  expected.reserve(size);
  std::transform(x.begin(), x.end(), std::back_inserter(expected), [scale](float value) {
    return saturate_half_even(value * scale);
  });

  error_type<std::string> result = compare_sequences(
      span<const int16_t>(z),
      span<const int16_t>(expected),
      [](int16_t actual, int16_t value) { return std::abs(static_cast<int>(actual) - static_cast<int>(value)); },
      0);
  ASSERT_TRUE(result.has_value()) << result.error();
}

TEST_P(OcuduvecConvertFixture, OcuduvecConvertTestFloatInt16HalfwayCases)
{
  // Halfway values of both signs.
  std::uniform_int_distribution<int> dist(-100, 100);

  std::vector<float> x;
  x.reserve(size);
  std::generate_n(std::back_inserter(x), size, [&dist]() { return static_cast<float>(dist(rgen)) + 0.5F; });

  std::vector<int16_t> z(size);
  ocuduvec::convert(z, x, 1.0F);

  std::vector<int16_t> expected;
  expected.reserve(size);
  std::transform(x.begin(), x.end(), std::back_inserter(expected), saturate_half_even);

  error_type<std::string> result = compare_sequences(span<const int16_t>(z), span<const int16_t>(expected));
  ASSERT_TRUE(result.has_value()) << result.error();
}

TEST_P(OcuduvecConvertFixture, OcuduvecConvertTestBf16Int16HalfwayCases)
{
  // Halfway values of both signs, exactly representable in BF16.
  std::uniform_int_distribution<int> dist(-100, 100);

  std::vector<bf16_t> x;
  x.reserve(size);
  std::generate_n(std::back_inserter(x), size, [&dist]() { return to_bf16(static_cast<float>(dist(rgen)) + 0.5F); });

  std::vector<int16_t> z(size);
  ocuduvec::convert(z, x, 1.0F);

  std::vector<int16_t> expected;
  expected.reserve(size);
  std::transform(x.begin(), x.end(), std::back_inserter(expected), [](bf16_t value) {
    return saturate_half_even(to_float(value));
  });

  error_type<std::string> result = compare_sequences(span<const int16_t>(z), span<const int16_t>(expected));
  ASSERT_TRUE(result.has_value()) << result.error();
}

TEST_P(OcuduvecConvertFixture, OcuduvecConvertTestInt16Complex)
{
  std::uniform_int_distribution<int16_t> dist(INT16_MIN, INT16_MAX);

  std::vector<int16_t> x;
  x.reserve(2 * size);
  std::generate_n(std::back_inserter(x), 2 * size, [&dist]() { return dist(rgen); });

  std::vector<cf_t> z(size);

  float scale = 1000.0F;

  ocuduvec::convert(z, x, scale);

  std::vector<cf_t> expected;
  expected.reserve(size);
  for (unsigned i = 0; i != size; ++i) {
    expected.emplace_back(static_cast<float>(x[2 * i]) / scale, static_cast<float>(x[2 * i + 1]) / scale);
  }

  error_type<std::string> result = compare_sequences(
      span<const cf_t>(z),
      span<const cf_t>(expected),
      [](cf_t actual, cf_t value) { return std::abs(actual - value); },
      ASSERT_CF_MAX_ERROR);
  ASSERT_TRUE(result.has_value()) << result.error();
}

TEST_P(OcuduvecConvertFixture, OcuduvecConvertTestInt16Float)
{
  std::uniform_int_distribution<int16_t> dist(INT16_MIN, INT16_MAX);

  std::vector<int16_t> x;
  x.reserve(size);
  std::generate_n(std::back_inserter(x), size, [&dist]() { return dist(rgen); });

  std::vector<float> z(size);

  float scale = 1000.0F;

  ocuduvec::convert(z, x, scale);

  std::vector<float> expected;
  expected.reserve(size);
  std::transform(x.begin(), x.end(), std::back_inserter(expected), [scale](int16_t value) {
    return static_cast<float>(value) / scale;
  });

  error_type<std::string> result = compare_sequences(
      span<const float>(z),
      span<const float>(expected),
      [](float actual, float value) { return std::abs(actual - value); },
      ASSERT_FLOAT_MAX_ERROR);
  ASSERT_TRUE(result.has_value()) << result.error();
}

TEST_P(OcuduvecConvertFixture, OcuduvecConvertTestComplexComplex16Random)
{
  static constexpr float                range = std::numeric_limits<float>::max() / 2;
  std::uniform_real_distribution<float> dist(-range, range);

  std::vector<cf_t> in;
  in.reserve(size);
  std::generate_n(std::back_inserter(in), size, [&dist]() { return cf_t(dist(rgen), dist(rgen)); });

  std::vector<cf_t>    out(size);
  std::vector<cbf16_t> data_cbf16(size);

  // Convert from single precision to BF16.
  ocuduvec::convert(data_cbf16, in);

  // Convert from BF16 to single precision.
  ocuduvec::convert(out, data_cbf16);

  // Assert conversion from single precision.
  std::vector<cbf16_t> expected_cbf16;
  expected_cbf16.reserve(size);
  std::transform(in.begin(), in.end(), std::back_inserter(expected_cbf16), [](cf_t value) { return to_cbf16(value); });

  error_type<std::string> result =
      compare_sequences(span<const cbf16_t>(data_cbf16), span<const cbf16_t>(expected_cbf16));
  ASSERT_TRUE(result.has_value()) << result.error();

  // Assert conversion from BF16.
  result = compare_sequences(
      span<const cf_t>(out), span<const cf_t>(in), relative_error<cf_t>, ASSERT_BF16_MAX_RELATIVE_ERROR);
  ASSERT_TRUE(result.has_value()) << result.error();
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

  std::vector<cf_t> in;
  in.reserve(size);
  std::generate_n(std::back_inserter(in), size, [n = 0]() mutable {
    float re = values[(n++) % values.size()];
    float im = values[(n++) % values.size()];
    return cf_t(re, im);
  });

  std::vector<cbf16_t> data_cbf16(size);

  // Convert from single precision to BF16.
  ocuduvec::convert(data_cbf16, in);

  // Verify the converted values are equal to the expected.
  std::vector<cbf16_t> expected;
  expected.reserve(size);
  std::generate_n(std::back_inserter(expected), size, [n = 0]() mutable {
    cbf16_t value;
    value.real = bf16_t(expected_values[(n++) % expected_values.size()]);
    value.imag = bf16_t(expected_values[(n++) % expected_values.size()]);
    return value;
  });

  error_type<std::string> result = compare_sequences(span<const cbf16_t>(data_cbf16), span<const cbf16_t>(expected));
  ASSERT_TRUE(result.has_value()) << result.error();
}

TEST_P(OcuduvecConvertFixture, OcuduvecConvertTestFloatFloat16Random)
{
  std::uniform_real_distribution<float> dist(-1.0, 1.0);

  std::vector<float> in;
  in.reserve(size);
  std::generate_n(std::back_inserter(in), size, [&dist]() { return dist(rgen); });

  // Convert from single precision to brain float.
  std::vector<bf16_t> data_bf16(size);
  ocuduvec::convert(data_bf16, in);

  // Assert conversion to BF16.
  std::vector<bf16_t> expected_bf16;
  expected_bf16.reserve(size);
  std::transform(in.begin(), in.end(), std::back_inserter(expected_bf16), [](float value) { return to_bf16(value); });

  error_type<std::string> result = compare_sequences(span<const bf16_t>(data_bf16), span<const bf16_t>(expected_bf16));
  ASSERT_TRUE(result.has_value()) << result.error();

  // Convert back to single precision float.
  std::vector<float> out(size);
  ocuduvec::convert(out, data_bf16);

  // Assert conversion from BF16.
  result = compare_sequences(
      span<const float>(out), span<const float>(in), relative_error<float>, ASSERT_BF16_MAX_RELATIVE_ERROR);
  ASSERT_TRUE(result.has_value()) << result.error();
}

TEST_P(OcuduvecConvertFixture, OcuduvecConvertTestInt16Float16Random)
{
  std::uniform_real_distribution<float> dist(-1.0, 1.0);

  float int16_scale = (1 << 15) - 1;

  std::vector<float> in;
  in.reserve(size);
  std::generate_n(std::back_inserter(in), size, [&dist]() { return dist(rgen); });

  // Convert from single precision to int16.
  std::vector<int16_t> in_int16(size);
  ocuduvec::convert(in_int16, in, int16_scale);

  // Convert from int16 to brain float.
  std::vector<bf16_t> data_bf16(size);
  ocuduvec::convert(data_bf16, in_int16, int16_scale);

  // Assert conversion to BF16.
  std::vector<bf16_t> expected_bf16;
  expected_bf16.reserve(size);
  std::transform(in_int16.begin(), in_int16.end(), std::back_inserter(expected_bf16), [int16_scale](int16_t value) {
    return to_bf16(value, int16_scale);
  });

  error_type<std::string> result = compare_sequences(span<const bf16_t>(data_bf16), span<const bf16_t>(expected_bf16));
  ASSERT_TRUE(result.has_value()) << result.error();

  // Convert from brain float back to int16.
  std::vector<int16_t> out_int16(size);
  ocuduvec::convert(out_int16, data_bf16, int16_scale);

  // Assert conversion from BF16.
  std::vector<int16_t> expected_int16;
  expected_int16.reserve(size);
  std::transform(data_bf16.begin(), data_bf16.end(), std::back_inserter(expected_int16), [int16_scale](bf16_t value) {
    return saturate_half_even(to_float(value) * int16_scale);
  });

  result = compare_sequences(span<const int16_t>(out_int16), span<const int16_t>(expected_int16));
  ASSERT_TRUE(result.has_value()) << result.error();

  // Convert from int16 back to single precision.
  std::vector<float> out(size);
  ocuduvec::convert(out, out_int16, int16_scale);

  // Assert the whole round trip against the original data. The data goes through BF16 and is rounded twice to int16.
  // The tolerance is the BF16 relative error plus one int16 quantization step, and the distance is normalized to it.
  result = compare_sequences(
      span<const float>(out),
      span<const float>(in),
      [int16_scale](float actual, float value) {
        return std::abs(actual - value) / (std::abs(value) * ASSERT_BF16_MAX_RELATIVE_ERROR + 1 / int16_scale);
      },
      1.0F);
  ASSERT_TRUE(result.has_value()) << result.error();
}

TEST_P(OcuduvecConvertFixture, OcuduvecConvertTestScaledInt16ComplexFloat16Random)
{
  constexpr float int16_gain = 1.0 / ((1 << 15) - 1);

  std::uniform_int_distribution<int16_t> dist_i(-32768, 32767);
  std::uniform_int_distribution<int16_t> dist_f(1, 128);

  const unsigned size_i16 = size * 2;

  std::vector<int16_t> in;
  in.reserve(size_i16);
  std::generate_n(std::back_inserter(in), size_i16, [&dist_i]() { return dist_i(rgen); });

  std::vector<float> gain;
  gain.reserve(size_i16);
  std::generate_n(std::back_inserter(gain), size_i16, [&dist_f]() { return int16_gain * float(dist_f(rgen)); });

  // Convert from int16 to brain float.
  std::vector<cbf16_t> data_cbf16(size);
  ocuduvec::convert(data_cbf16, in, gain);

  // Assert conversion to cbf16.
  std::vector<cbf16_t> expected;
  expected.reserve(size);
  for (unsigned i = 0; i != size; ++i) {
    cbf16_t value;
    value.real = to_bf16(in[i * 2], 1 / gain[i * 2]);
    value.imag = to_bf16(in[i * 2 + 1], 1 / gain[i * 2 + 1]);
    expected.push_back(value);
  }

  error_type<std::string> result = compare_sequences(span<const cbf16_t>(data_cbf16), span<const cbf16_t>(expected));
  ASSERT_TRUE(result.has_value()) << result.error();
}

TEST_P(OcuduvecConvertFixture, OcuduvecConvertTestComplexFloat16ComplexInt16Random)
{
  constexpr float scaling_factor = 1.0 / ((1 << 15) - 1);

  std::uniform_int_distribution<int16_t> dist_i(-32768, 32767);

  std::vector<ci16_t> in;
  in.reserve(size);
  std::generate_n(std::back_inserter(in), size, [&dist_i]() { return ci16_t(dist_i(rgen), dist_i(rgen)); });

  std::vector<ci16_t>  out(size);
  std::vector<cbf16_t> data_cbf16(size);

  // Convert from 16-bit complex integer to complex BF16.
  ocuduvec::convert(data_cbf16, in, scaling_factor);

  // Convert from complex BF16 to 16-bit complex integer.
  ocuduvec::convert(out, data_cbf16, scaling_factor);

  // Assert conversion from 16-bit complex integer to complex BF16.
  std::vector<cbf16_t> expected_cbf16;
  expected_cbf16.reserve(size);
  std::transform(in.begin(), in.end(), std::back_inserter(expected_cbf16), [scaling_factor](ci16_t value) {
    return to_cbf16(to_cf(value) / scaling_factor);
  });

  error_type<std::string> result =
      compare_sequences(span<const cbf16_t>(data_cbf16), span<const cbf16_t>(expected_cbf16));
  ASSERT_TRUE(result.has_value()) << result.error();

  // Assert conversion from complex BF16 to 16-bit complex integer.
  // The errors are computed in single precision, because the magnitudes overflow the 16-bit range.
  result = compare_sequences(
      span<const ci16_t>(out),
      span<const ci16_t>(in),
      [](ci16_t actual, ci16_t value) { return relative_error(to_cf(actual), to_cf(value)); },
      ASSERT_BF16_MAX_RELATIVE_ERROR);
  ASSERT_TRUE(result.has_value()) << result.error();
}

INSTANTIATE_TEST_SUITE_P(OcuduvecConvertTest, OcuduvecConvertFixture, ::testing::Values(1, 5, 7, 19, 23, 257, 1234));

} // namespace
