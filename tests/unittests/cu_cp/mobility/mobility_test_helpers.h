// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "tests/unittests/cu_cp/test_helpers.h"
#include "ocudu/cu_cp/cu_cp_configuration.h"
#include "ocudu/support/executors/manual_task_worker.h"
#include <gtest/gtest.h>

namespace ocudu {
namespace ocucp {

/// Fixture class for CU-CP mobility tests
class mobility_test : public ::testing::Test
{
protected:
  mobility_test();
  ~mobility_test() override;

  ue_manager* get_ue_manager() { return &ue_mng; }

  ocudulog::basic_logger& test_logger  = ocudulog::fetch_basic_logger("TEST");
  ocudulog::basic_logger& cu_cp_logger = ocudulog::fetch_basic_logger("CU-CP");

  manual_task_worker      ctrl_worker{128};
  timer_manager           timers;
  cu_cp_configuration     cu_cp_cfg;
  ue_manager_config       ue_cfg;
  ue_manager_dependencies ue_dependencies;

  ue_manager                                  ue_mng{ue_cfg, ue_dependencies};
  dummy_cu_cp_ue_context_manipulation_handler cu_cp_handler;
};

} // namespace ocucp
} // namespace ocudu
