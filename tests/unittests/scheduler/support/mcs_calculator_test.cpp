// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "lib/scheduler/support/mcs_calculator.h"
#include "fmt/format.h"
#include <gtest/gtest.h>

using namespace ocudu;

TEST(test_manual_values, test)
{
  ASSERT_EQ(0, map_cqi_to_mcs(1, ocudu::pdsch_mcs_table::qam64).value());
  ASSERT_EQ(0, map_cqi_to_mcs(2, ocudu::pdsch_mcs_table::qam64).value());
  ASSERT_EQ(2, map_cqi_to_mcs(3, ocudu::pdsch_mcs_table::qam64).value());
  ASSERT_EQ(4, map_cqi_to_mcs(4, ocudu::pdsch_mcs_table::qam64).value());
  ASSERT_EQ(6, map_cqi_to_mcs(5, ocudu::pdsch_mcs_table::qam64).value());
  ASSERT_EQ(8, map_cqi_to_mcs(6, ocudu::pdsch_mcs_table::qam64).value());
  ASSERT_EQ(11, map_cqi_to_mcs(7, ocudu::pdsch_mcs_table::qam64).value());
  ASSERT_EQ(13, map_cqi_to_mcs(8, ocudu::pdsch_mcs_table::qam64).value());
  ASSERT_EQ(15, map_cqi_to_mcs(9, ocudu::pdsch_mcs_table::qam64).value());
  ASSERT_EQ(18, map_cqi_to_mcs(10, ocudu::pdsch_mcs_table::qam64).value());
  ASSERT_EQ(20, map_cqi_to_mcs(11, ocudu::pdsch_mcs_table::qam64).value());
  ASSERT_EQ(22, map_cqi_to_mcs(12, ocudu::pdsch_mcs_table::qam64).value());
  ASSERT_EQ(24, map_cqi_to_mcs(13, ocudu::pdsch_mcs_table::qam64).value());
  ASSERT_EQ(26, map_cqi_to_mcs(14, ocudu::pdsch_mcs_table::qam64).value());
  ASSERT_EQ(28, map_cqi_to_mcs(15, ocudu::pdsch_mcs_table::qam64).value());
}

struct snr_to_ul_mcs_test_params {
  double                 snr;
  ocudu::pusch_mcs_table mcs_table;
  sch_mcs_index          expected_mcs;
};

void PrintTo(const snr_to_ul_mcs_test_params& p, std::ostream* os)
{
  *os << fmt::format(
      "snr={}dB mcs_table={} expected_mcs={}", p.snr, pusch_mcs_table_to_string(p.mcs_table), p.expected_mcs.value());
}

class snr_to_ul_mcs_tester : public ::testing::TestWithParam<snr_to_ul_mcs_test_params>
{};

TEST_P(snr_to_ul_mcs_tester, compare_bin_search_with_linear)
{
  ASSERT_EQ(GetParam().expected_mcs, map_snr_to_mcs_ul(GetParam().snr, GetParam().mcs_table, false));
}

// Compare the binary search with linear search for a given interval.
INSTANTIATE_TEST_SUITE_P(test_snr_range,
                         snr_to_ul_mcs_tester,
                         testing::Values(snr_to_ul_mcs_test_params{-20.0, ocudu::pusch_mcs_table::qam64, {0}},
                                         snr_to_ul_mcs_test_params{40.0, ocudu::pusch_mcs_table::qam64, {28}},
                                         snr_to_ul_mcs_test_params{0.0133, ocudu::pusch_mcs_table::qam64, {4}},
                                         snr_to_ul_mcs_test_params{7.8, ocudu::pusch_mcs_table::qam64, {13}}));
