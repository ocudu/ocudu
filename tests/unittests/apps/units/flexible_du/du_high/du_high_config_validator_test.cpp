// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "apps/units/flexible_o_du/o_du_high/du_high/du_high_config_cli11_schema.h"
#include "apps/units/flexible_o_du/o_du_high/du_high/du_high_config_validator.h"
#include "tests/ocudu_test_requirements.h"
#include "ocudu/ran/pusch/pusch_constants.h"
#include <gtest/gtest.h>

using namespace ocudu;

// The receiver soft-combines every reception of a transport block into one buffer -- the occasions of a repetition
// bundle and the HARQ retransmissions of that bundle alike -- and admits a bounded number of them. A configuration
// asking for more is rejected at startup rather than left to discard occasions silently in the PHY.
class du_high_pusch_repetition_config_test : public ::testing::Test
{
protected:
  du_high_pusch_repetition_config_test() : app("du_high config validator test")
  {
    // Build the configuration the way the application does: several fields are filled in by the CLI11 schema and the
    // auto-derivation step rather than defaulted in the struct, and the validator expects them to be set.
    configure_cli11_with_du_high_config_schema(app, parsed_cfg);
    app.parse("");
    autoderive_du_high_parameters_after_parsing(parsed_cfg.config);
  }

  du_high_unit_pusch_config& pusch_cfg() { return parsed_cfg.config.cells_cfg.front().cell.pusch_cfg; }

  bool validate() const { return validate_du_high_config(parsed_cfg.config); }

  CLI::App              app;
  du_high_parsed_config parsed_cfg;
};

TEST_F(du_high_pusch_repetition_config_test, default_configuration_is_valid)
{
  OCUDU_TEST_REQUIREMENTS("MVP-FUNC-SVCS-16-8-d");

  ASSERT_TRUE(validate());
}

TEST_F(du_high_pusch_repetition_config_test, repetitions_within_the_soft_combining_budget_are_valid)
{
  OCUDU_TEST_REQUIREMENTS("MVP-FUNC-SVCS-16-8-d");

  // (1 + 3) * 8 == 32, exactly the budget.
  pusch_cfg().max_nof_harq_retxs = 3;
  pusch_cfg().max_nof_rep        = 8;

  ASSERT_EQ((1U + pusch_cfg().max_nof_harq_retxs) * pusch_cfg().max_nof_rep,
            pusch_constants::MAX_NOF_SOFT_COMBINED_PUSCH_TXS);
  ASSERT_TRUE(validate());
}

TEST_F(du_high_pusch_repetition_config_test, repetitions_beyond_the_soft_combining_budget_are_rejected)
{
  OCUDU_TEST_REQUIREMENTS("MVP-FUNC-SVCS-16-8-d");

  // (1 + 4) * 8 == 40, over the budget by one retransmission.
  pusch_cfg().max_nof_harq_retxs = 4;
  pusch_cfg().max_nof_rep        = 8;

  ASSERT_FALSE(validate());
}

TEST_F(du_high_pusch_repetition_config_test, the_initial_transmission_counts_towards_the_budget)
{
  OCUDU_TEST_REQUIREMENTS("MVP-FUNC-SVCS-16-8-d");

  // (1 + 1) * 16 == 32 fits, while (1 + 2) * 16 == 48 does not: the budget covers the initial transmission too, not
  // just the retransmissions.
  pusch_cfg().max_nof_rep        = 16;
  pusch_cfg().max_nof_harq_retxs = 1;
  ASSERT_TRUE(validate());

  pusch_cfg().max_nof_harq_retxs = 2;
  ASSERT_FALSE(validate());
}
