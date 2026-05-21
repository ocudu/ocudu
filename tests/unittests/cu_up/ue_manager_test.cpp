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
  void SetUp() override
  {
    ocudulog::fetch_basic_logger("TEST").set_level(ocudulog::basic_levels::debug);
    ocudulog::init();

    // create required objects
    gtpu_rx_demux      = std::make_unique<dummy_gtpu_demux_ctrl>();
    gtpu_n3_allocator  = std::make_unique<dummy_gtpu_teid_pool>();
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
    ue_mng = std::make_unique<ue_manager>(ue_manager_config{max_nof_ues, n3_config, test_mode_config},
                                          ue_manager_dependencies{{*e1ap1, *e1ap2},
                                                                  timers,
                                                                  *f1u_gw,
                                                                  *ngu_session_mngr,
                                                                  *pdcp_ctrl_handler,
                                                                  *gtpu_rx_demux,
                                                                  *gtpu_n3_allocator,
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
  std::unique_ptr<gtpu_teid_pool>                             gtpu_n3_allocator;
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
  n3_interface_config                                         n3_config;
  cu_up_test_mode_config                                      test_mode_config{};
  ocudulog::basic_logger&                                     test_logger = ocudulog::fetch_basic_logger("TEST", false);
  manual_task_worker                                          worker{64};

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
