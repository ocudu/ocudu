// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

/// \file
/// \brief In this file, we verify the correct operation of the MAC scheduler, as a whole, for basic operations, in
/// setups without any UEs. The objective is to cover and verify the integration of the scheduler building blocks.
/// The currently covered operations include:
/// - run_slot without any event
/// - handle RACH indication + RAR allocation.

#include "lib/scheduler/scheduler_impl.h"
#include "tests/test_doubles/scheduler/scheduler_config_helper.h"
#include "tests/test_doubles/utils/test_rng.h"
#include "tests/unittests/scheduler/test_utils/config_generators.h"
#include "tests/unittests/scheduler/test_utils/dummy_test_components.h"
#include "tests/unittests/scheduler/test_utils/indication_generators.h"
#include "tests/unittests/scheduler/test_utils/scheduler_test_suite.h"
#include "ocudu/scheduler/config/scheduler_expert_config_factory.h"
#include <gtest/gtest.h>

using namespace ocudu;

namespace sched_no_ue_test {

class sched_no_ue_tester : public ::testing::Test
{};

} // namespace sched_no_ue_test

using namespace sched_no_ue_test;

TEST_F(sched_no_ue_tester, test_no_ues)
{
  scheduler_expert_config             sched_cfg = config_helpers::make_default_scheduler_expert_config();
  sched_cfg_dummy_notifier            cfg_notif;
  scheduler_ue_metrics_dummy_notifier metrics_notif;
  scheduler_impl                      sch{scheduler_config{sched_cfg, cfg_notif}};

  // Action 1: Add Cell.
  sched_cell_configuration_request_message cell_cfg_msg =
      sched_config_helper::make_default_sched_cell_configuration_request();
  test_helpers::test_sched_config_manager cfg_mng{sched_cfg};
  const cell_configuration&               cell_cfg = *cfg_mng.add_cell(cell_cfg_msg);
  sch.handle_cell_configuration_request(cell_cfg_msg);

  slot_point_extended sl_tx{subcarrier_spacing::kHz15,
                            test_rng::uniform_int<unsigned>(0, NOF_HYPER_SFNS * NOF_SFNS * 10 - 1)};

  // Action 2: Run slot.
  const sched_result& res = sch.slot_indication(sl_tx, to_du_cell_index(0));
  ASSERT_TRUE(res.success);
  test_scheduler_result_consistency(cell_cfg, sl_tx.without_hyper_sfn(), res);
  ASSERT_TRUE(res.dl.ue_grants.empty());
  ASSERT_TRUE(res.ul.puschs.empty());
}

TEST_F(sched_no_ue_tester, test_rach_indication)
{
  scheduler_expert_config             sched_cfg = config_helpers::make_default_scheduler_expert_config();
  sched_cfg_dummy_notifier            cfg_notif;
  scheduler_ue_metrics_dummy_notifier metrics_notif;
  scheduler_impl                      sch{scheduler_config{sched_cfg, cfg_notif}};

  // Action 1: Add Cell.
  sched_cell_configuration_request_message cell_cfg_msg =
      sched_config_helper::make_default_sched_cell_configuration_request();
  test_helpers::test_sched_config_manager cfg_mng{sched_cfg};
  const cell_configuration&               cell_cfg = *cfg_mng.add_cell(cell_cfg_msg);
  sch.handle_cell_configuration_request(cell_cfg_msg);

  // Action 2: Add RACH indication.
  // Note: RACH is added in a slot different than the SIB1 to avoid PDCCH conflicts.
  const unsigned      tx_delay = 2;
  slot_point_extended sl_rx{subcarrier_spacing::kHz15, 1};
  slot_point_extended sl_tx = sl_rx + tx_delay;
  sch.handle_rach_indication(test_helper::create_rach_indication(
      cell_cfg, sl_rx.without_hyper_sfn(), {test_helper::create_preamble(0, to_rnti(0x4601))}));

  // Action 3: Run slot 0.
  const sched_result& res = sch.slot_indication(sl_tx, to_du_cell_index(0));

  // TEST: Result exists. No Data allocated. A RAR has been allocated.
  ASSERT_TRUE(res.success);
  test_scheduler_result_consistency(cell_cfg, sl_tx.without_hyper_sfn(), res);
  ASSERT_TRUE(res.dl.ue_grants.empty());
  ASSERT_TRUE(not res.dl.rar_grants.empty());
}
