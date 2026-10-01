// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "../support/task_executor_test_doubles.h"
#include "lib/rlc/rlc_pdu_recycler.h"
#include "tests/test_doubles/pdcp/pdcp_pdu_generator.h"
#include "ocudu/ran/du_types.h"
#include <gtest/gtest.h>

using namespace ocudu;

/// Fixture class for RLC PDU recycler tests
class rlc_pdu_recycler_test : public ::testing::Test
{
protected:
  void SetUp() override
  {
    // init test's logger
    ocudulog::init();
    logger.set_level(ocudulog::basic_levels::debug);

    // init RLC logger
    ocudulog::fetch_basic_logger("RLC", false).set_level(ocudulog::basic_levels::debug);
    ocudulog::fetch_basic_logger("RLC", false).set_hex_dump_max_size(-1);

    logger.info("Creating RLC PDU recycler");

    // Create RLC PDU recycler
    pdu_recycler = std::make_unique<rlc_pdu_recycler>(rlc_window_size, rlc_logger);
  }

  void TearDown() override
  {
    // flush logger after each test
    ocudulog::flush();
  }

  ocudulog::basic_logger& logger     = ocudulog::fetch_basic_logger("TEST", false);
  rlc_bearer_logger       rlc_logger = rlc_bearer_logger("RLC", {gnb_du_id_t::min, du_ue_index_t{}, rb_id_t{}, "DL"});
  std::unique_ptr<rlc_pdu_recycler> pdu_recycler;

  manual_task_worker_always_enqueue_tasks other_worker{8};

  const size_t rlc_window_size = 2;
};

TEST_F(rlc_pdu_recycler_test, recycler_memory_reserved)
{
  auto& recycle_bin = pdu_recycler->get_recycle_queue();
  EXPECT_EQ(recycle_bin.size(), 0);
  EXPECT_EQ(recycle_bin.capacity(), rlc_window_size);
}

TEST_F(rlc_pdu_recycler_test, clear_by_executor)
{
  byte_buffer pdu = test_helpers::create_pdcp_pdu(pdcp_sn_size::size12bits, /* is_srb = */ false, 0xaa, 3, 0xaa);
  EXPECT_TRUE(pdu_recycler->add_discarded_pdu(pdu.deep_copy().value()));
  // Check the PDU is stored in first recycle bin
  auto& recycle_bin = pdu_recycler->get_recycle_queue();
  ASSERT_EQ(recycle_bin.size(), 1);

  // Schedule the cleanup in the other_worker task queue. Nothing will be deleted yet.
  pdu_recycler->clear_by_executor(other_worker);
  ASSERT_EQ(recycle_bin.size(), 1);

  // Perform the cleanup by the other_worker. Everything will be deleted.
  other_worker.run_pending_tasks();
  EXPECT_EQ(recycle_bin.size(), 0);
}

TEST_F(rlc_pdu_recycler_test, clear_multiple_times)
{
  byte_buffer pdu1 = test_helpers::create_pdcp_pdu(pdcp_sn_size::size12bits, /* is_srb = */ false, 0xaa, 3, 0xaa);
  byte_buffer pdu2 = test_helpers::create_pdcp_pdu(pdcp_sn_size::size12bits, /* is_srb = */ false, 0xbb, 3, 0xbb);
  byte_buffer pdu3 = test_helpers::create_pdcp_pdu(pdcp_sn_size::size12bits, /* is_srb = */ false, 0xcc, 3, 0xcc);
  byte_buffer pdu4 = test_helpers::create_pdcp_pdu(pdcp_sn_size::size12bits, /* is_srb = */ false, 0xdd, 3, 0xdd);

  auto& recycle_bin = pdu_recycler->get_recycle_queue();

  EXPECT_TRUE(pdu_recycler->add_discarded_pdu(pdu1.deep_copy().value()));

  // Check the PDU is stored in first recycle bin
  ASSERT_EQ(recycle_bin.size(), 1);

  // Schedule the cleanup in the other_worker task queue. Nothing will be deleted yet.
  pdu_recycler->clear_by_executor(other_worker);
  ASSERT_EQ(recycle_bin.size(), 1);

  // Discard PDU2, check its stored in second recycle bin
  EXPECT_TRUE(pdu_recycler->add_discarded_pdu(pdu2.deep_copy().value()));
  ASSERT_EQ(recycle_bin.size(), 2);

  // Perform the cleanup by the other_worker. The PDUs will be deleted
  other_worker.run_pending_tasks();
  ASSERT_EQ(recycle_bin.size(), 0);

  // Discard PDU3, check its stored in third recycle bin
  EXPECT_TRUE(pdu_recycler->add_discarded_pdu(pdu3.deep_copy().value()));

  // Schedule the cleanup in the other_worker task queue. Nothing will be deleted yet.
  pdu_recycler->clear_by_executor(other_worker);
  ASSERT_EQ(recycle_bin.size(), 1);

  // Perform the cleanup by the other_worker. The second bin will be deleted
  other_worker.run_pending_tasks();
  ASSERT_EQ(recycle_bin.size(), 0);
}

TEST_F(rlc_pdu_recycler_test, clear_multiple_times_without_running_task)
{
  byte_buffer pdu1 = test_helpers::create_pdcp_pdu(pdcp_sn_size::size12bits, /* is_srb = */ false, 0xaa, 3, 0xaa);
  byte_buffer pdu2 = test_helpers::create_pdcp_pdu(pdcp_sn_size::size12bits, /* is_srb = */ false, 0xbb, 3, 0xbb);
  byte_buffer pdu3 = test_helpers::create_pdcp_pdu(pdcp_sn_size::size12bits, /* is_srb = */ false, 0xcc, 3, 0xcc);
  byte_buffer pdu4 = test_helpers::create_pdcp_pdu(pdcp_sn_size::size12bits, /* is_srb = */ false, 0xdd, 3, 0xdd);
  EXPECT_TRUE(pdu_recycler->add_discarded_pdu(pdu1.deep_copy().value()));
  // Check the PDU is stored in first recycle bin
  auto& recycle_bin = pdu_recycler->get_recycle_queue();
  ASSERT_EQ(recycle_bin.size(), 1);

  // Schedule the cleanup in the other_worker task queue. Nothing will be deleted yet.
  pdu_recycler->clear_by_executor(other_worker);
  ASSERT_EQ(recycle_bin.size(), 1);

  // Don't perform the cleanup by the other_worker.
  // Discard one more PDU and confirm 2 PDUs in the queue.

  // Discard PDU2, check its stored in the recycle bin
  EXPECT_TRUE(pdu_recycler->add_discarded_pdu(pdu2.deep_copy().value()));
  ASSERT_EQ(recycle_bin.size(), 2);

  // Schedule the cleanup in the other_worker task queue. Nothing will be deleted yet.
  pdu_recycler->clear_by_executor(other_worker);
  ASSERT_EQ(recycle_bin.size(), 2);

  // Finally run pending tasks
  other_worker.run_pending_tasks();
  ASSERT_EQ(recycle_bin.size(), 0);
}

TEST_F(rlc_pdu_recycler_test, full_recycle_bin)
{
  byte_buffer pdu1 = test_helpers::create_pdcp_pdu(pdcp_sn_size::size12bits, /* is_srb = */ false, 0xaa, 3, 0xaa);
  byte_buffer pdu2 = test_helpers::create_pdcp_pdu(pdcp_sn_size::size12bits, /* is_srb = */ false, 0xbb, 3, 0xbb);
  byte_buffer pdu3 = test_helpers::create_pdcp_pdu(pdcp_sn_size::size12bits, /* is_srb = */ false, 0xcc, 3, 0xcc);

  EXPECT_TRUE(pdu_recycler->add_discarded_pdu(pdu1.deep_copy().value()));
  EXPECT_TRUE(pdu_recycler->add_discarded_pdu(pdu2.deep_copy().value()));

  // Recycle bin is full - any extra PDU will be deleted on the spot and the function returns false
  EXPECT_FALSE(pdu_recycler->add_discarded_pdu(pdu3.deep_copy().value()));

  // Check the PDUs are stored in the recycle bin
  auto& recycle_bin = pdu_recycler->get_recycle_queue();
  ASSERT_EQ(recycle_bin.size(), 2);

  // Schedule the cleanup in the other_worker task queue. Nothing will be deleted yet.
  pdu_recycler->clear_by_executor(other_worker);
  ASSERT_EQ(recycle_bin.size(), 2);

  // Perform the cleanup by the other_worker. Everything will be deleted.
  other_worker.run_pending_tasks();
  ASSERT_EQ(recycle_bin.size(), 0);
}

int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
