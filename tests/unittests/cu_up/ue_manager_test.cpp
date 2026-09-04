// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "cu_up_test_helpers.h"
#include "lib/cu_up/ue_manager.h"
#include "ocudu/cu_up/cu_up_types.h"
#include "ocudu/support/async/async_test_utils.h"
#include "ocudu/support/executors/manual_task_worker.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace ocuup;

/// Fixture class for UE manager tests
class ue_manager_test : public ::testing::Test
{
protected:
  /// \param defer_tasks Whether the executors defer their tasks instead of running them inline. Deferring makes the
  /// UE removal routines suspend, which allows the test to mutate the UE database while a removal is in flight.
  explicit ue_manager_test(bool defer_tasks = false) : worker(64, true, defer_tasks) {}

  void SetUp() override
  {
    ocudulog::fetch_basic_logger("TEST").set_level(ocudulog::basic_levels::debug);
    ocudulog::init();

    // create required objects
    gtpu_rx_demux      = std::make_unique<dummy_gtpu_demux_ctrl>();
    gtpu_ngu_allocator = std::make_unique<dummy_gtpu_teid_pool>();
    gtpu_f1u_allocator = std::make_unique<dummy_gtpu_teid_pool>();
    gtpu_tx_notifier   = std::make_unique<dummy_gtpu_network_gateway_adapter>();
    f1u_gw             = std::make_unique<dummy_f1u_gateway>(f1u_bearer);
    e1ap1              = std::make_unique<dummy_e1ap>(cu_up_e1_index_t{0});
    e1ap2              = std::make_unique<dummy_e1ap>(cu_up_e1_index_t{1});
    pdcp_ctrl_handler  = std::make_unique<dummy_cu_up_manager_pdcp_interface>();
    ngu_session_mngr   = std::make_unique<dummy_ngu_session_manager>();

    cu_up_exec_mapper = std::make_unique<dummy_cu_up_executor_mapper>(&worker);

    // Create UE cfg
    ue_cfg = {security::sec_as_config{}, activity_notification_level_t::ue, std::chrono::seconds(0), {}, 1000000000};

    // create DUT object
    ue_mng = std::make_unique<ue_manager>(ue_manager_config{max_nof_ues, ngu_config, test_mode_config},
                                          ue_manager_dependencies{{*e1ap1, *e1ap2},
                                                                  timers,
                                                                  *f1u_gw,
                                                                  *ngu_session_mngr,
                                                                  *pdcp_ctrl_handler,
                                                                  *gtpu_rx_demux,
                                                                  *gtpu_ngu_allocator,
                                                                  *gtpu_f1u_allocator,
                                                                  *cu_up_exec_mapper,
                                                                  gtpu_pcap,
                                                                  test_logger});
  }

  void TearDown() override
  {
    // flush logger after each test
    ocudulog::flush();
  }

  std::unique_ptr<gtpu_demux_ctrl>                            gtpu_rx_demux;
  std::unique_ptr<gtpu_teid_pool>                             gtpu_ngu_allocator;
  std::unique_ptr<gtpu_teid_pool>                             gtpu_f1u_allocator;
  std::unique_ptr<gtpu_tunnel_common_tx_upper_layer_notifier> gtpu_tx_notifier;
  std::unique_ptr<e1ap_interface>                             e1ap1;
  std::unique_ptr<e1ap_interface>                             e1ap2;
  std::unique_ptr<cu_up_manager_pdcp_interface>               pdcp_ctrl_handler;
  std::unique_ptr<cu_up_executor_mapper>                      cu_up_exec_mapper;
  dummy_inner_f1u_bearer                                      f1u_bearer;
  null_dlt_pcap                                               gtpu_pcap;
  std::unique_ptr<f1u_cu_up_gateway>                          f1u_gw;
  std::unique_ptr<ngu_session_manager>                        ngu_session_mngr;
  timer_manager                                               timers;
  ue_context_cfg                                              ue_cfg;
  std::unique_ptr<ue_manager_ctrl>                            ue_mng;
  ngu_interface_config                                        ngu_config;
  cu_up_test_mode_config                                      test_mode_config{};
  ocudulog::basic_logger&                                     test_logger = ocudulog::fetch_basic_logger("TEST", false);
  manual_task_worker                                          worker;

  const uint32_t                          max_nof_ues = 16384;
  async_task<void>                        t;
  std::optional<lazy_task_launcher<void>> t_launcher;
};

/// UE object handling tests (creation/deletion)
TEST_F(ue_manager_test, when_ue_db_not_full_new_ue_can_be_added)
{
  ASSERT_EQ(ue_mng->get_nof_ues(), 0);
  ue_context* ue = ue_mng->add_ue(e1ap1->get_e1_index(), ue_cfg);
  ASSERT_NE(ue, nullptr);
  ASSERT_EQ(ue_mng->get_nof_ues(), 1);
}

TEST_F(ue_manager_test, when_ue_db_is_full_new_ue_cannot_be_added)
{
  // add maximum number of UE objects
  for (uint32_t i = 0; i < max_nof_ues; i++) {
    ue_context* ue = ue_mng->add_ue(e1ap1->get_e1_index(), ue_cfg);
    ASSERT_NE(ue, nullptr);
  }
  ASSERT_EQ(ue_mng->get_nof_ues(), max_nof_ues);

  // try to add one more
  ue_context* ue = ue_mng->add_ue(e1ap1->get_e1_index(), ue_cfg);
  ASSERT_EQ(ue, nullptr);
}

TEST_F(ue_manager_test, when_ue_are_deleted_ue_db_is_empty)
{
  // add maximum number of UE objects
  for (uint32_t i = 0; i < max_nof_ues; i++) {
    ue_context* ue = ue_mng->add_ue(e1ap1->get_e1_index(), ue_cfg);
    ASSERT_NE(ue, nullptr);
  }
  ASSERT_EQ(ue_mng->get_nof_ues(), max_nof_ues);

  // delete all UE objects
  for (uint32_t i = 0; i < max_nof_ues; i++) {
    t = ue_mng->remove_ue(int_to_ue_index(i));
    t_launcher.emplace(t);
  }
  ASSERT_EQ(ue_mng->get_nof_ues(), 0);
}

TEST_F(ue_manager_test, when_ues_come_from_different_cps_different_e1_indexes_are_used)
{
  ue_context* ue1 = ue_mng->add_ue(e1ap1->get_e1_index(), ue_cfg);
  ASSERT_NE(ue1, nullptr);
  ASSERT_EQ(ue1->get_e1_index(), e1ap1->get_e1_index());

  ue_context* ue2 = ue_mng->add_ue(e1ap2->get_e1_index(), ue_cfg);
  ASSERT_NE(ue2, nullptr);
  ASSERT_EQ(ue2->get_e1_index(), e1ap2->get_e1_index());
}

// Add two bearer contexts to E1AP 1 and one to E1AP 2.
// Make sure that when E1AP 1 loses connection, only the bearer
// contexts associated with it are removed.
TEST_F(ue_manager_test, when_e1_is_lost_only_ues_from_that_e1_are_removed)
{
  ue_context* ue1_1 = ue_mng->add_ue(e1ap1->get_e1_index(), ue_cfg);
  ASSERT_NE(ue1_1, nullptr);
  ASSERT_EQ(ue1_1->get_e1_index(), e1ap1->get_e1_index());

  ue_context* ue1_2 = ue_mng->add_ue(e1ap1->get_e1_index(), ue_cfg);
  ASSERT_NE(ue1_2, nullptr);
  ASSERT_EQ(ue1_2->get_e1_index(), e1ap1->get_e1_index());

  ue_context* ue2_1 = ue_mng->add_ue(e1ap2->get_e1_index(), ue_cfg);
  ASSERT_NE(ue2_1, nullptr);
  ASSERT_EQ(ue2_1->get_e1_index(), e1ap2->get_e1_index());

  cu_up_ue_index_t ue1_1_index = ue1_1->get_index();
  cu_up_ue_index_t ue1_2_index = ue1_2->get_index();
  cu_up_ue_index_t ue2_1_index = ue2_1->get_index();

  // Remove all UEs from E1AP 1.
  t = ue_mng->remove_e1_ues(ue1_1->get_e1_index());
  t_launcher.emplace(t);

  // We should not be able to find UEs associated with E1AP 1.
  ue_context* ret = ue_mng->find_ue(ue1_1_index);
  ASSERT_EQ(ret, nullptr);
  ret = ue_mng->find_ue(ue1_2_index);
  ASSERT_EQ(ret, nullptr);

  // Bearer contexts associated with with E1AP 2 should still be present.
  ret = ue_mng->find_ue(ue2_1_index);
  ASSERT_NE(ret, nullptr);
}

// A partial E1 Reset may list UEs that got released in the meantime. Make sure the already released UEs are skipped
// and the remaining ones are still removed.
TEST_F(ue_manager_test, when_ue_list_contains_already_removed_ue_remaining_ues_are_removed)
{
  ue_context* ue1 = ue_mng->add_ue(e1ap1->get_e1_index(), ue_cfg);
  ASSERT_NE(ue1, nullptr);
  ue_context* ue2 = ue_mng->add_ue(e1ap1->get_e1_index(), ue_cfg);
  ASSERT_NE(ue2, nullptr);

  const std::vector<cu_up_ue_index_t> ue_indexes = {ue1->get_index(), ue2->get_index()};

  // Release the first UE, as a Bearer Context Release Command would.
  {
    async_task<void>         remove_ue_task = ue_mng->remove_ue(ue_indexes[0]);
    lazy_task_launcher<void> launcher(remove_ue_task);
    ASSERT_TRUE(remove_ue_task.ready());
  }
  ASSERT_EQ(ue_mng->get_nof_ues(), 1);

  // Removing both UEs must skip the already released one.
  {
    async_task<void>         remove_ues_task = ue_mng->remove_ues(ue_indexes);
    lazy_task_launcher<void> launcher(remove_ues_task);
    ASSERT_TRUE(remove_ues_task.ready());
  }
  ASSERT_EQ(ue_mng->get_nof_ues(), 0);
}

/// Fixture class for UE manager tests that require the UE removal routines to suspend.
class ue_manager_deferred_task_test : public ue_manager_test
{
protected:
  ue_manager_deferred_task_test() : ue_manager_test(true) {}

  /// Run pending tasks until the given task completes.
  void run_until_completed(const async_task<void>& task)
  {
    while (not task.ready()) {
      ASSERT_TRUE(worker.run_pending_tasks()) << "No pending tasks left, but task did not complete";
    }
  }
};

// E1AP PDUs are handled in the same CU-UP control executor the removal routines return to, so a Bearer Context Setup
// Request can be handled while a removal routine is suspended. Make sure that the UEs added in the meantime neither
// disturb the pending removals nor get removed themselves.
TEST_F(ue_manager_deferred_task_test, when_ues_are_added_while_ues_are_removed_then_pending_ues_are_removed)
{
  const unsigned nof_initial_ues = 100;
  const unsigned nof_added_ues   = 200;

  for (unsigned i = 0; i != nof_initial_ues; ++i) {
    ASSERT_NE(ue_mng->add_ue(e1ap1->get_e1_index(), ue_cfg), nullptr);
  }

  const cu_up_ue_index_t              first_ue_index = int_to_ue_index(nof_initial_ues - 1);
  const std::vector<cu_up_ue_index_t> ue_indexes     = {
      first_ue_index, int_to_ue_index(nof_initial_ues - 2), int_to_ue_index(nof_initial_ues - 3)};

  async_task<void>         remove_ues_task = ue_mng->remove_ues(ue_indexes);
  lazy_task_launcher<void> launcher(remove_ues_task);
  ASSERT_FALSE(remove_ues_task.ready()) << "UE removal routine did not suspend";

  // Add UEs while the removal routine is suspended. This grows the UE database and thus reallocates it.
  for (unsigned i = 0; i != nof_added_ues; ++i) {
    ASSERT_NE(ue_mng->add_ue(e1ap1->get_e1_index(), ue_cfg), nullptr);
  }

  run_until_completed(remove_ues_task);

  // The UEs that were still pending got removed. The first UE index was released before the added UEs were created, so
  // it is taken by one of them.
  ASSERT_EQ(ue_mng->find_ue(ue_indexes[1]), nullptr);
  ASSERT_EQ(ue_mng->find_ue(ue_indexes[2]), nullptr);
  ASSERT_NE(ue_mng->find_ue(first_ue_index), nullptr);
  ASSERT_EQ(ue_mng->get_nof_ues(), nof_initial_ues - ue_indexes.size() + nof_added_ues);
}

// The UEs of a removal routine are flagged for removal upfront. This keeps another trigger from releasing them, and
// thus freeing their UE index for reuse, while the routine is still in flight.
TEST_F(ue_manager_deferred_task_test, when_ues_are_removed_then_all_of_them_are_flagged_upfront)
{
  for (unsigned i = 0; i != 3; ++i) {
    ASSERT_NE(ue_mng->add_ue(e1ap1->get_e1_index(), ue_cfg), nullptr);
  }
  const std::vector<cu_up_ue_index_t> ue_indexes = {int_to_ue_index(0), int_to_ue_index(1), int_to_ue_index(2)};

  async_task<void>         remove_ues_task = ue_mng->remove_ues(ue_indexes);
  lazy_task_launcher<void> launcher(remove_ues_task);
  ASSERT_FALSE(remove_ues_task.ready()) << "UE removal routine did not suspend";

  // The UEs that the routine has not reached yet are flagged, so cu_up_manager_impl would discard a Bearer Context
  // Release Command for them and their UE index cannot be taken by a new UE.
  for (cu_up_ue_index_t ue_index : {ue_indexes[1], ue_indexes[2]}) {
    ue_context* ue_ctxt = ue_mng->find_ue(ue_index);
    ASSERT_NE(ue_ctxt, nullptr);
    ASSERT_TRUE(ue_ctxt->remove_pending()) << "ue=" << fmt::underlying(ue_index) << " was not flagged for removal";
  }

  // A second removal routine for the same UEs must not schedule anything.
  {
    async_task<void>         second_removal = ue_mng->remove_ues(ue_indexes);
    lazy_task_launcher<void> second_launcher(second_removal);
    ASSERT_TRUE(second_removal.ready()) << "Second removal routine scheduled a redundant removal";
  }

  run_until_completed(remove_ues_task);
  ASSERT_EQ(ue_mng->get_nof_ues(), 0);
}
