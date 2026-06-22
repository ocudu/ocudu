// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "lib/mac/mac_sched/rlf_detector.h"
#include "mac_test_helpers.h"
#include "tests/test_doubles/utils/test_rng.h"
#include "ocudu/support/executors/manual_task_worker.h"
#include <gtest/gtest.h>

using namespace ocudu;

class mac_rlf_detector_test : public ::testing::Test
{
protected:
  static constexpr unsigned MAX_KOS = 100;

  mac_rlf_detector_test() : rlf_handler(mac_expert_config{.configs = {{MAX_KOS, MAX_KOS, MAX_KOS}}}, worker)
  {
    ocudulog::init();
    ocudulog::fetch_basic_logger("MAC").set_level(ocudulog::basic_levels::debug);
  }
  ~mac_rlf_detector_test() override { ocudulog::flush(); }

  manual_task_worker                  worker{64};
  test_helpers::dummy_ue_rlf_notifier rlf_notif;
  rlf_detector                        rlf_handler;
};

TEST_F(mac_rlf_detector_test, when_consecutive_harq_kos_reaches_limit_then_rlf_is_triggered)
{
  const du_ue_index_t   ue_index   = to_du_ue_index(0);
  const du_cell_index_t cell_index = to_du_cell_index(0);
  rlf_handler.add_ue(ue_index, rlf_notif);

  unsigned nof_oks = test_rng::uniform_int<unsigned>(0, MAX_KOS + 1);
  for (unsigned i = 0; i != nof_oks; ++i) {
    rlf_handler.handle_ack(ue_index, cell_index, true);
  }

  for (unsigned i = 0; i != MAX_KOS; ++i) {
    ASSERT_FALSE(rlf_notif.rlf_detected);
    rlf_handler.handle_ack(ue_index, cell_index, false);
  }
  ASSERT_FALSE(rlf_notif.rlf_detected);
  worker.run_pending_tasks();
  ASSERT_TRUE(rlf_notif.rlf_detected);
  ASSERT_EQ(rlf_notif.last_cause, mac_rlf_cause::max_consecutive_harq_kos);

  // Check that the RLF is not triggered again, if more KOs are detected.
  rlf_notif.rlf_detected = false;
  for (unsigned i = 0; i != MAX_KOS; ++i) {
    rlf_handler.handle_ack(ue_index, cell_index, false);
  }
  worker.run_pending_tasks();
  ASSERT_FALSE(rlf_notif.rlf_detected);
}

TEST_F(mac_rlf_detector_test, when_consecutive_harq_kos_limit_not_reached_then_rlf_is_not_triggered)
{
  const du_ue_index_t   ue_index   = to_du_ue_index(0);
  const du_cell_index_t cell_index = to_du_cell_index(0);
  rlf_handler.add_ue(ue_index, rlf_notif);

  for (unsigned i = 0; i != MAX_KOS - 1; ++i) {
    rlf_handler.handle_ack(ue_index, cell_index, false);
  }
  rlf_handler.handle_ack(ue_index, cell_index, true);
  rlf_handler.handle_ack(ue_index, cell_index, false);
  worker.run_pending_tasks();
  ASSERT_FALSE(rlf_notif.rlf_detected);
}

TEST_F(mac_rlf_detector_test, when_consecutive_undetected_csi_reaches_limit_then_rlf_is_triggered)
{
  const du_ue_index_t   ue_index   = to_du_ue_index(0);
  const du_cell_index_t cell_index = to_du_cell_index(0);
  rlf_handler.add_ue(ue_index, rlf_notif);

  unsigned nof_oks = test_rng::uniform_int<unsigned>(0, MAX_KOS + 1);
  for (unsigned i = 0; i != nof_oks; ++i) {
    rlf_handler.handle_csi(ue_index, cell_index, true);
  }
  worker.run_pending_tasks();
  ASSERT_FALSE(rlf_notif.rlf_detected);

  for (unsigned i = 0; i != MAX_KOS; ++i) {
    rlf_handler.handle_csi(ue_index, cell_index, false);
  }
  ASSERT_FALSE(rlf_notif.rlf_detected);
  worker.run_pending_tasks();
  ASSERT_TRUE(rlf_notif.rlf_detected);
  ASSERT_EQ(rlf_notif.last_cause, mac_rlf_cause::max_consecutive_csi_dtx);

  // Check that the RLF is not triggered again, if any more undetected CSIs.
  rlf_notif.rlf_detected = false;
  for (unsigned i = 0; i != MAX_KOS; ++i) {
    rlf_handler.handle_csi(ue_index, cell_index, false);
  }
  worker.run_pending_tasks();
  ASSERT_FALSE(rlf_notif.rlf_detected);
}

TEST_F(mac_rlf_detector_test, when_consecutive_crc_kos_reaches_limit_then_rlf_is_triggered)
{
  const du_ue_index_t   ue_index   = to_du_ue_index(0);
  const du_cell_index_t cell_index = to_du_cell_index(0);
  rlf_handler.add_ue(ue_index, rlf_notif);

  unsigned nof_oks = test_rng::uniform_int<unsigned>(0, MAX_KOS + 1);
  for (unsigned i = 0; i != nof_oks; ++i) {
    rlf_handler.handle_crc(ue_index, cell_index, true);
  }

  for (unsigned i = 0; i != MAX_KOS; ++i) {
    ASSERT_FALSE(rlf_notif.rlf_detected);
    rlf_handler.handle_crc(ue_index, cell_index, false);
  }
  ASSERT_FALSE(rlf_notif.rlf_detected);
  worker.run_pending_tasks();
  ASSERT_TRUE(rlf_notif.rlf_detected);
  ASSERT_EQ(rlf_notif.last_cause, mac_rlf_cause::max_consecutive_crc_kos);
}
