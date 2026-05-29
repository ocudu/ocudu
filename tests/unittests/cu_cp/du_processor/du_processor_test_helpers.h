// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../test_helpers.h"
#include "lib/cu_cp/du_processor/du_configuration_manager.h"
#include "lib/cu_cp/du_processor/du_processor.h"
#include "lib/cu_cp/du_processor/du_processor_factory.h"
#include "lib/cu_cp/ue_manager/ue_manager_impl.h"
#include "tests/unittests/f1ap/common/test_helpers.h"
#include "tests/unittests/rrc/test_helpers.h"
#include "ocudu/ran/cu_cp_types.h"
#include "ocudu/support/async/async_test_utils.h"
#include "ocudu/support/executors/manual_task_worker.h"
#include <gtest/gtest.h>

namespace ocudu::ocucp {

/// Fixture class for DU processor creation
class du_processor_test : public ::testing::Test
{
protected:
  du_processor_test();
  ~du_processor_test() override;

  ocudulog::basic_logger& test_logger  = ocudulog::fetch_basic_logger("TEST");
  ocudulog::basic_logger& cu_cp_logger = ocudulog::fetch_basic_logger("CU-CP");

  timer_manager       timers;
  manual_task_worker  ctrl_worker{128};
  cu_cp_configuration cu_cp_cfg;

  ue_manager                              ue_mng{cu_cp_cfg};
  dummy_du_processor_cu_cp_notifier       cu_cp_notifier{&ue_mng};
  dummy_du_connection_notifier            du_conn_notifier;
  dummy_f1ap_pdu_notifier                 f1ap_pdu_notifier;
  dummy_rrc_ue_cu_cp_adapter              rrc_ue_cu_cp_notifier;
  std::unique_ptr<cu_cp_du_event_handler> cu_cp_event_handler;
  std::unique_ptr<async_task_scheduler>   common_task_sched;
  du_configuration_manager                du_cfg_mgr;
  std::unique_ptr<du_processor>           du_processor_obj;

  async_task<cu_cp_ue_index_t>                        t;
  std::optional<lazy_task_launcher<cu_cp_ue_index_t>> t_launcher;
};

} // namespace ocudu::ocucp
