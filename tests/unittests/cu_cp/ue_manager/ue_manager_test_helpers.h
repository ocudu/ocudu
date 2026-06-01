// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../test_helpers.h"
#include "lib/cu_cp/ue_manager/ue_manager_impl.h"
#include "tests/unittests/ngap/test_helpers.h"
#include "ocudu/ran/cu_cp_types.h"
#include "ocudu/support/executors/manual_task_worker.h"
#include <gtest/gtest.h>
#include <memory>

namespace ocudu {
namespace ocucp {

/// Fixture class for UE manager creation
class ue_manager_test : public ::testing::Test
{
protected:
  ue_manager_test();
  ~ue_manager_test() override;

  ocudulog::basic_logger& test_logger   = ocudulog::fetch_basic_logger("TEST");
  ocudulog::basic_logger& ue_mng_logger = ocudulog::fetch_basic_logger("CU-UEMNG");

  unsigned ues_per_du = 1024;

  timer_manager       timers;
  manual_task_worker  cu_worker{128};
  cu_cp_configuration cu_cp_cfg;

  ue_manager ue_mng{cu_cp_cfg};

  // DU processor to RRC UE adapters
  dummy_ngap_rrc_ue_notifier rrc_ue_pdu_notifier;
};

} // namespace ocucp
} // namespace ocudu
