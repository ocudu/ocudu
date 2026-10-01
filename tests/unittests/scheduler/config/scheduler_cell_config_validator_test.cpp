// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "tests/test_doubles/scheduler/scheduler_config_helper.h"
#include "ocudu/scheduler/config/scheduler_cell_config_validator.h"
#include "ocudu/scheduler/config/scheduler_expert_config_factory.h"
#include <gtest/gtest.h>

using namespace ocudu;

static error_type<std::string> validate(unsigned max_nof_ue_contexts)
{
  sched_cell_configuration_request_message msg = sched_config_helper::make_default_sched_cell_configuration_request();
  msg.max_nof_ue_contexts                      = max_nof_ue_contexts;
  return config_validators::validate_sched_cell_configuration_request_message(
      msg, config_helpers::make_default_scheduler_expert_config());
}

TEST(scheduler_cell_config_validator_test, default_nof_ue_contexts_is_valid)
{
  ASSERT_TRUE(
      validate(sched_config_helper::make_default_sched_cell_configuration_request().max_nof_ue_contexts).has_value());
}

TEST(scheduler_cell_config_validator_test, nof_ue_contexts_at_cell_limit_is_valid)
{
  ASSERT_TRUE(validate(MAX_NOF_DU_UES_PER_CELL).has_value());
}

TEST(scheduler_cell_config_validator_test, zero_nof_ue_contexts_is_invalid)
{
  ASSERT_FALSE(validate(0).has_value());
}

TEST(scheduler_cell_config_validator_test, nof_ue_contexts_above_cell_limit_is_invalid)
{
  ASSERT_FALSE(validate(MAX_NOF_DU_UES_PER_CELL + 1).has_value());
}
