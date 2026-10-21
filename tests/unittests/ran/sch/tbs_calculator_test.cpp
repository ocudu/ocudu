// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/ran/sch/sch_constants.h"
#include "ocudu/ran/sch/tbs_calculator.h"
#include <gtest/gtest.h>
#include <map>
#include <vector>

using namespace ocudu;

TEST(tbs_calculator_dlsch_lbrm_test, default_tbs_lbrm_is_the_one_of_four_layers_256qam_and_273_prbs)
{
  ASSERT_EQ(tbs_calculator_dlsch_lbrm(4, modulation_scheme::QAM256, 273), tbs_lbrm_default);
}

TEST(tbs_calculator_dlsch_lbrm_test, tbs_lbrm_depends_on_the_layers_the_modulation_and_the_prbs)
{
  ASSERT_EQ(tbs_calculator_dlsch_lbrm(1, modulation_scheme::QAM64, 106), units::bytes(11525));
  ASSERT_EQ(tbs_calculator_dlsch_lbrm(2, modulation_scheme::QAM256, 51), units::bytes(18951));
  ASSERT_EQ(tbs_calculator_dlsch_lbrm(2, modulation_scheme::QAM64, 273), units::bytes(59448));
  ASSERT_EQ(tbs_calculator_dlsch_lbrm(4, modulation_scheme::QAM1024, 273), units::bytes(196721));
}

TEST(tbs_calculator_dlsch_lbrm_test, more_than_four_layers_count_as_four)
{
  for (unsigned nof_layers = 5; nof_layers <= 8; ++nof_layers) {
    ASSERT_EQ(tbs_calculator_dlsch_lbrm(nof_layers, modulation_scheme::QAM256, 273), tbs_lbrm_default);
  }
}

TEST(tbs_calculator_dlsch_lbrm_test, number_of_prbs_is_rounded_up_as_per_ts38212_table_5_4_2_1_1)
{
  // Values of TBS_LBRM for one layer and 64-QAM with n_PRB,LBRM of 32, 66, 107, 135, 162, 217 and 273 PRBs.
  static const std::map<unsigned, units::bytes> tbs_lbrm_per_nof_prb_lbrm = {{32, units::bytes(3457)},
                                                                             {66, units::bytes(7172)},
                                                                             {107, units::bytes(11525)},
                                                                             {135, units::bytes(14599)},
                                                                             {162, units::bytes(17422)},
                                                                             {217, units::bytes(23572)},
                                                                             {273, units::bytes(29722)}};

  // Maximum number of PRBs across the BWPs and the resulting n_PRB,LBRM, at the edges of each range of the table.
  static const std::vector<std::pair<unsigned, unsigned>> nof_prb_lbrm = {{1, 32},
                                                                          {32, 32},
                                                                          {33, 66},
                                                                          {66, 66},
                                                                          {67, 107},
                                                                          {107, 107},
                                                                          {108, 135},
                                                                          {135, 135},
                                                                          {136, 162},
                                                                          {162, 162},
                                                                          {163, 217},
                                                                          {217, 217},
                                                                          {218, 273},
                                                                          {275, 273}};

  for (const auto& [max_nof_prbs, expected_nof_prb_lbrm] : nof_prb_lbrm) {
    ASSERT_EQ(tbs_calculator_dlsch_lbrm(1, modulation_scheme::QAM64, max_nof_prbs),
              tbs_lbrm_per_nof_prb_lbrm.at(expected_nof_prb_lbrm))
        << fmt::format("max_nof_prbs={}", max_nof_prbs);
  }
}
