// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "ocudu/ocuduvec/division.h"
#include <gtest/gtest.h>
#include <random>

static std::mt19937 rgen(0);
static const float  ASSERT_MAX_ERROR = 1e-3;

using namespace ocudu;

namespace {

using OcuduvecDivisionParams = unsigned;

class OcuduvecDivisionFixture : public ::testing::TestWithParam<OcuduvecDivisionParams>
{
protected:
  unsigned size;

  void SetUp() override
  {
    auto params = GetParam();
    size        = params;
  }
};

TEST_P(OcuduvecDivisionFixture, OcuduvecDivisionTestDivideFff)
{
  std::uniform_real_distribution<float> dist(-1.0, 1.0);

  std::vector<float> num(size);
  for (float& v : num) {
    v = dist(rgen);
  }

  std::vector<float> den(size);
  for (float& v : den) {
    v = dist(rgen);
  }

  std::vector<float> result(size);

  ocuduvec::divide(result, num, den);

  for (size_t i = 0; i != size; ++i) {
    float gold_z = 0.0F;
    if (std::isnormal(num[i]) && std::isnormal(den[i])) {
      gold_z = num[i] / den[i];
    }
    float err = std::abs(gold_z - result[i]);
    if (std::isnormal(gold_z)) {
      err /= std::abs(gold_z);
    }

    ASSERT_LT(err, ASSERT_MAX_ERROR) << fmt::format("gold={} result={} err={}", gold_z, result[i], err);
  }
}

INSTANTIATE_TEST_SUITE_P(OcuduvecDivisionTest, OcuduvecDivisionFixture, ::testing::Values(1, 5, 7, 19, 23, 257));

} // namespace
