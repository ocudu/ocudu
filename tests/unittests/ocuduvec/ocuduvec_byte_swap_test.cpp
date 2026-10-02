// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "ocudu/ocuduvec/byte_swap.h"
#include <gtest/gtest.h>
#include <limits>
#include <random>
#include <vector>

using namespace ocudu;

static std::mt19937 rgen(0);

/// Guard bytes after the output, which must be left untouched.
static constexpr unsigned GUARD_BYTES   = 64;
static constexpr uint8_t  GUARD_PATTERN = 0xa5;

/// Reference serialization: each component written most significant byte first.
static std::vector<uint8_t> reference_big_endian(const std::vector<ci16_t>& samples)
{
  std::vector<uint8_t> bytes;
  bytes.reserve(4 * samples.size());
  for (const ci16_t& sample : samples) {
    for (const int16_t component : {sample.real(), sample.imag()}) {
      const auto word = static_cast<uint16_t>(component);
      bytes.push_back(static_cast<uint8_t>(word >> 8));
      bytes.push_back(static_cast<uint8_t>(word & 0xffU));
    }
  }
  return bytes;
}

static std::vector<ci16_t> random_samples(unsigned nof_samples)
{
  std::uniform_int_distribution<int> dist(std::numeric_limits<int16_t>::min(), std::numeric_limits<int16_t>::max());
  std::vector<ci16_t>                samples(nof_samples);
  for (ci16_t& sample : samples) {
    sample = ci16_t(static_cast<int16_t>(dist(rgen)), static_cast<int16_t>(dist(rgen)));
  }
  return samples;
}

namespace {

class OcuduvecByteSwapFixture : public ::testing::TestWithParam<unsigned>
{};

} // namespace

TEST_P(OcuduvecByteSwapFixture, SamplesToBytesMatchesReference)
{
  const unsigned            nof_samples = GetParam();
  const std::vector<ci16_t> samples     = random_samples(nof_samples);

  std::vector<uint8_t> bytes(4 * nof_samples + GUARD_BYTES, GUARD_PATTERN);
  ocuduvec::swap_bytes(span<uint8_t>(bytes).first(4 * nof_samples), samples);

  const std::vector<uint8_t> expected = reference_big_endian(samples);
  for (unsigned i = 0, e = expected.size(); i != e; ++i) {
    ASSERT_EQ(bytes[i], expected[i]) << "Byte " << i << " of " << e;
  }
  for (unsigned i = expected.size(), e = bytes.size(); i != e; ++i) {
    ASSERT_EQ(bytes[i], GUARD_PATTERN) << "Wrote past the end of the output at byte " << i;
  }
}

TEST_P(OcuduvecByteSwapFixture, BytesToSamplesMatchesReference)
{
  const unsigned             nof_samples = GetParam();
  const std::vector<ci16_t>  expected    = random_samples(nof_samples);
  const std::vector<uint8_t> bytes       = reference_big_endian(expected);

  std::vector<ci16_t> samples(nof_samples + GUARD_BYTES / 4, ci16_t(0x5a5a, 0x5a5a));
  ocuduvec::swap_bytes(span<ci16_t>(samples).first(nof_samples), bytes);

  for (unsigned i = 0; i != nof_samples; ++i) {
    ASSERT_EQ(samples[i], expected[i]) << "Sample " << i << " of " << nof_samples;
  }
  for (unsigned i = nof_samples, e = samples.size(); i != e; ++i) {
    ASSERT_EQ(samples[i], ci16_t(0x5a5a, 0x5a5a)) << "Wrote past the end of the output at sample " << i;
  }
}

TEST_P(OcuduvecByteSwapFixture, RoundTripIsLossless)
{
  const unsigned            nof_samples = GetParam();
  const std::vector<ci16_t> samples     = random_samples(nof_samples);

  std::vector<uint8_t> bytes(4 * nof_samples);
  ocuduvec::swap_bytes(bytes, samples);

  std::vector<ci16_t> recovered(nof_samples);
  ocuduvec::swap_bytes(recovered, bytes);

  ASSERT_EQ(recovered, samples);
}

// Sizes cover every SIMD block width (16, 8 and 4 samples for 512, 256 and 128 bits), the scalar tail around each,
// a DIFI packet (1920 samples) and a 400 MHz slot at 120 kHz (61440 samples).
INSTANTIATE_TEST_SUITE_P(OcuduvecByteSwapTest,
                         OcuduvecByteSwapFixture,
                         ::testing::Values(0, 1, 3, 4, 7, 8, 15, 16, 17, 31, 32, 33, 64, 65, 1920, 61440));

TEST(OcuduvecByteSwap, KnownPattern)
{
  const std::vector<ci16_t>  samples  = {{0x1234, static_cast<int16_t>(0xabcd)}, {-1, 1}};
  const std::vector<uint8_t> expected = {0x12, 0x34, 0xab, 0xcd, 0xff, 0xff, 0x00, 0x01};

  std::vector<uint8_t> bytes(expected.size());
  ocuduvec::swap_bytes(bytes, samples);
  ASSERT_EQ(bytes, expected);
}
