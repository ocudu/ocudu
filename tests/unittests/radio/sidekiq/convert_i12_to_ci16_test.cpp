// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "sidekiq_helper_functions.h"
#include <gtest/gtest.h>
#include <random>

static std::mt19937 rgen(0);

using namespace ocudu;

namespace {

// Error function used to compare two ci16_t samples: returns 0 if they are exactly equal, 1 otherwise.
auto compare_ci16 = [](ci16_t left, ci16_t right) { return (left == right) ? 0 : 1; };

// Number of bits of the packed signed integer samples.
constexpr unsigned I12_NOF_BITS = 12;
// Number of bits between an int16_t and the packed 12-bit sample, i.e., the shift applied to align the sign bit.
constexpr unsigned I12_TO_I16_SHIFT = 16 - I12_NOF_BITS;
// Range of representable values of a 12-bit signed integer.
constexpr int32_t I12_MIN = -(1 << (I12_NOF_BITS - 1));
constexpr int32_t I12_MAX = (1 << (I12_NOF_BITS - 1)) - 1;

// Reference (non-vectorized) implementation used as the golden model.
ci16_t gold_extract_sample(span<const uint32_t> in, unsigned i_sample)
{
  unsigned i_word = (i_sample / 4) * 3;
  unsigned pos    = i_sample % 4;
  uint32_t word0  = in[i_word + 0];
  uint32_t word1  = in[i_word + 1];
  uint32_t word2  = in[i_word + 2];
  uint64_t word01 = (static_cast<uint64_t>(word0) << 32U) | static_cast<uint64_t>(word1);
  uint64_t word12 = (static_cast<uint64_t>(word1) << 32U) | static_cast<uint64_t>(word2);

  std::array<int16_t, 8> iq_values;
  iq_values[0] = static_cast<int16_t>((word0 >> 16U) & 0xfff0);
  iq_values[1] = static_cast<int16_t>((word0 >> 4U) & 0xfff0);
  iq_values[2] = static_cast<int16_t>((word01 >> 24U) & 0xfff0);
  iq_values[3] = static_cast<int16_t>((word1 >> 12U) & 0xfff0);
  iq_values[4] = static_cast<int16_t>((word1 >> 0U) & 0xfff0);
  iq_values[5] = static_cast<int16_t>((word12 >> 20U) & 0xfff0);
  iq_values[6] = static_cast<int16_t>((word2 >> 8U) & 0xfff0);
  iq_values[7] = static_cast<int16_t>((word2 << 4U) & 0xfff0);

  return ci16_t{iq_values[2 * pos], iq_values[2 * pos + 1]};
}

} // namespace

class SidekiqConvertI12Fixture : public ::testing::TestWithParam<unsigned>
{
protected:
  unsigned N_words = 0;

  void SetUp() override { N_words = GetParam(); }
};

TEST_P(SidekiqConvertI12Fixture, ConvertI12ToCi16)
{
  unsigned N_samples = (N_words / 3) * 4;

  std::uniform_int_distribution<uint32_t> dist(0, UINT32_MAX);

  std::vector<uint32_t> in(N_words);
  for (uint32_t& v : in) {
    v = dist(rgen);
  }

  std::vector<ci16_t> out(N_samples);

  convert_i12_to_ci16(out, in);

  std::vector<ci16_t> gold(N_samples);
  for (unsigned i = 0; i != N_samples; ++i) {
    gold[i] = gold_extract_sample(in, i);
  }

  for (unsigned i = 0; i != N_samples; ++i) {
    ASSERT_EQ(out[i].real(), gold[i].real()) << "Sample index " << i << " real part mismatch.";
    ASSERT_EQ(out[i].imag(), gold[i].imag()) << "Sample index " << i << " imaginary part mismatch.";
  }
}

TEST_P(SidekiqConvertI12Fixture, ConvertCi16ToI12)
{
  unsigned N_samples = (N_words / 3) * 4;

  // Draw samples that are representable with 12 bits (i.e., multiples of 2^I12_TO_I16_SHIFT, in the range of
  // int16_t).
  std::uniform_int_distribution<int32_t> dist(I12_MIN, I12_MAX);

  std::vector<ci16_t> in(N_samples);
  for (ci16_t& v : in) {
    v = ci16_t{static_cast<int16_t>(dist(rgen) << I12_TO_I16_SHIFT),
               static_cast<int16_t>(dist(rgen) << I12_TO_I16_SHIFT)};
  }

  std::vector<uint32_t> out(N_words);

  convert_ci16_to_i12(out, in);
  fmt::println("in=[{:016b}];", span<const uint16_t>(reinterpret_cast<const uint16_t*>(in.data()), 2 * in.size()));
  fmt::println("out=[{:032b}];", out);

  // Round-trip through convert_i12_to_ci16 and verify we recover the original samples.
  std::vector<ci16_t> roundtrip(N_samples);
  convert_i12_to_ci16(roundtrip, out);

  for (unsigned i = 0; i != N_samples; ++i) {
    ASSERT_EQ(in[i].real(), roundtrip[i].real()) << "Sample index " << i << " real part mismatch.";
    ASSERT_EQ(in[i].imag(), roundtrip[i].imag()) << "Sample index " << i << " imaginary part mismatch.";
  }
}

INSTANTIATE_TEST_SUITE_P(sidekiq, SidekiqConvertI12Fixture, ::testing::Values(3, 6, 12, 30, 300));
