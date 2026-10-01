// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/adt/format.h"
#include "ocudu/mac/config/mac_config_helpers.h"
#include "fmt/format.h"
#include <gtest/gtest.h>

using namespace ocudu;

using gbr_bps = uint64_t;

struct mac_lc_config_test_params {
  gbr_bps              input;
  prioritized_bit_rate expected_output;
};

void PrintTo(const mac_lc_config_test_params& p, std::ostream* os)
{
  *os << fmt::format("input={}bps expected_pbr={}kBps", p.input, fmt::underlying(p.expected_output));
}

class mac_lc_config_test : public ::testing::TestWithParam<mac_lc_config_test_params>
{};

TEST_P(mac_lc_config_test, verify_get_pbr_ge_given_bit_rate)
{
  mac_lc_config_test_params params = GetParam();
  ASSERT_EQ(params.expected_output, get_pbr_ceil(params.input));
}

INSTANTIATE_TEST_SUITE_P(mac_config_helpers_test,
                         mac_lc_config_test,
                         testing::Values(mac_lc_config_test_params{128000, prioritized_bit_rate::kBps16},
                                         mac_lc_config_test_params{256000, prioritized_bit_rate::kBps32},
                                         mac_lc_config_test_params{512000, prioritized_bit_rate::kBps64},
                                         mac_lc_config_test_params{484000, prioritized_bit_rate::kBps64}));
