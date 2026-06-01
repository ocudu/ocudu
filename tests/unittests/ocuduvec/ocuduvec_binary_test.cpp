// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "ocudu/ocuduvec/binary.h"
#include <gtest/gtest.h>
#include <random>

static std::mt19937 rgen(0);

using namespace ocudu;

namespace {

using OcuduvecBinaryParams = unsigned;

class OcuduvecBinaryFixture : public ::testing::TestWithParam<OcuduvecBinaryParams>
{
protected:
  unsigned size;

  void SetUp() override
  {
    auto params = GetParam();
    size        = params;
  }
};

TEST_P(OcuduvecBinaryFixture, OcuduvecBinaryTestXorUint8)
{
  std::uniform_int_distribution<uint8_t> dist(0, UINT8_MAX);

  std::vector<uint8_t> x(size);
  for (uint8_t& v : x) {
    v = dist(rgen);
  }

  std::vector<uint8_t> y(size);
  for (uint8_t& v : y) {
    v = dist(rgen);
  }

  std::vector<uint8_t> z(size);

  ocuduvec::binary_xor(z, x, y);

  for (size_t i = 0; i != size; ++i) {
    uint8_t gold_z = x[i] ^ y[i];
    ASSERT_EQ(gold_z, z[i]);
  }
}

TEST_P(OcuduvecBinaryFixture, OcuduvecBinaryTestAndUint8)
{
  std::uniform_int_distribution<uint8_t> dist(0, UINT8_MAX);

  std::vector<uint8_t> x(size);
  for (uint8_t& v : x) {
    v = dist(rgen);
  }

  std::vector<uint8_t> y(size);
  for (uint8_t& v : y) {
    v = dist(rgen);
  }

  std::vector<uint8_t> z(size);

  ocuduvec::binary_and(z, x, y);

  for (size_t i = 0; i != size; ++i) {
    uint8_t gold_z = x[i] & y[i];
    ASSERT_EQ(gold_z, z[i]);
  }
}

TEST_P(OcuduvecBinaryFixture, OcuduvecBinaryTestOrUint8)
{
  std::uniform_int_distribution<uint8_t> dist(0, UINT8_MAX);

  std::vector<uint8_t> x(size);
  for (uint8_t& v : x) {
    v = dist(rgen);
  }

  std::vector<uint8_t> y(size);
  for (uint8_t& v : y) {
    v = dist(rgen);
  }

  std::vector<uint8_t> z(size);

  ocuduvec::binary_or(z, x, y);

  for (size_t i = 0; i != size; ++i) {
    uint8_t gold_z = x[i] | y[i];
    ASSERT_EQ(gold_z, z[i]);
  }
}

INSTANTIATE_TEST_SUITE_P(OcuduvecBinaryTest, OcuduvecBinaryFixture, ::testing::Values(1, 5, 7, 19, 23, 257));

} // namespace
