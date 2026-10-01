// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "ocudu/ocuduvec/modulus_square.h"
#include "ocudu/support/math/math_utils.h"
#include <gtest/gtest.h>
#include <random>

static std::mt19937 rgen(0);
static const float  ASSERT_CF_MAX_ERROR = 1e-6;

using namespace ocudu;

namespace {

using OcuduvecModulusSquareParams = unsigned;

class OcuduvecModulusSquareFixture : public ::testing::TestWithParam<OcuduvecModulusSquareParams>
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

TEST_P(OcuduvecModulusSquareFixture, OcuduvecModulusSquareTestFloatComplex)
{
  std::uniform_real_distribution<float> dist(-1.0, 1.0);

  std::vector<cf_t> input(size);
  std::generate(input.begin(), input.end(), [&dist]() { return cf_t(dist(rgen), dist(rgen)); });

  std::vector<float> result(size);

  ocuduvec::modulus_square(result, input);

  for (size_t i = 0; i != size; ++i) {
    cf_t  gold_z = std::norm(input[i]);
    float err    = std::abs(gold_z - result[i]);
    ASSERT_TRUE(err < ASSERT_CF_MAX_ERROR);
  }
}

TEST_P(OcuduvecModulusSquareFixture, OcuduvecModulusSquareAndAddTestFloatComplex)
{
  std::uniform_real_distribution<float> dist(-1.0, 1.0);

  std::vector<cf_t> input(size);
  std::generate(input.begin(), input.end(), [&dist]() { return cf_t(dist(rgen), dist(rgen)); });

  std::vector<float> offset(size);
  std::generate(offset.begin(), offset.end(), [&dist]() { return dist(rgen); });

  std::vector<float> result(size);

  ocuduvec::modulus_square_and_add(result, input, offset);

  for (size_t i = 0; i != size; ++i) {
    cf_t  gold_z = std::norm(input[i]) + offset[i];
    float err    = std::abs(gold_z - result[i]);
    ASSERT_TRUE(err < ASSERT_CF_MAX_ERROR);
  }
}

TEST_P(OcuduvecModulusSquareFixture, OcuduvecModulusSquareTestBrainFloatComplex)
{
  std::uniform_real_distribution<float> dist(-1.0, 1.0);

  std::vector<cbf16_t> input(size);
  std::generate(input.begin(), input.end(), [&dist]() { return cbf16_t(dist(rgen), dist(rgen)); });

  std::vector<float> result(size);

  ocuduvec::modulus_square(result, input);

  for (size_t i = 0; i != size; ++i) {
    cf_t  gold_z = std::norm(to_cf(input[i]));
    float err    = std::abs(gold_z - result[i]);
    ASSERT_TRUE(err < ASSERT_CF_MAX_ERROR);
  }
}

TEST_P(OcuduvecModulusSquareFixture, OcuduvecModulusSquareAndAddTestBrainFloatComplex)
{
  std::uniform_real_distribution<float> dist(-1.0, 1.0);

  std::vector<cbf16_t> input(size);
  std::generate(input.begin(), input.end(), [&dist]() { return cbf16_t(dist(rgen), dist(rgen)); });

  std::vector<float> offset(size);
  std::generate(offset.begin(), offset.end(), [&dist]() { return dist(rgen); });

  std::vector<float> result(size);

  ocuduvec::modulus_square_and_add(result, input, offset);

  for (size_t i = 0; i != size; ++i) {
    cf_t  gold_z = std::norm(to_cf(input[i])) + offset[i];
    float err    = std::abs(gold_z - result[i]);
    ASSERT_TRUE(err < ASSERT_CF_MAX_ERROR);
  }
}

INSTANTIATE_TEST_SUITE_P(OcuduvecModulusSquareTest,
                         OcuduvecModulusSquareFixture,
                         ::testing::Values(1, 5, 7, 19, 23, 257, 1234));

} // namespace
