// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "lib/cu_cp/cu_up_processor/cu_up_processor.h"
#include "lib/cu_cp/ue_manager/ue_manager_impl.h"
#include "tests/unittests/e1ap/cu_cp/e1ap_cu_cp_test_helpers.h"
#include "ocudu/support/executors/manual_task_worker.h"
#include <gtest/gtest.h>

namespace ocudu::ocucp {

/// Fixture class for DU processor creation
class cu_up_processor_test : public ::testing::Test
{
protected:
  cu_up_processor_test();
  ~cu_up_processor_test() override;

  ocudulog::basic_logger& test_logger  = ocudulog::fetch_basic_logger("TEST");
  ocudulog::basic_logger& cu_cp_logger = ocudulog::fetch_basic_logger("CU-CP");

  timer_manager               timers;
  dummy_e1ap_message_notifier e1ap_notifier;
  manual_task_worker          ctrl_worker{128};
  cu_cp_configuration         cu_cp_cfg;

  ue_manager                            ue_mng{cu_cp_cfg};
  dummy_e1ap_cu_cp_notifier             cu_cp_notifier{ue_mng};
  std::unique_ptr<async_task_scheduler> common_task_sched;
  std::unique_ptr<cu_up_processor>      cu_up_processor_obj;
};

} // namespace ocudu::ocucp
