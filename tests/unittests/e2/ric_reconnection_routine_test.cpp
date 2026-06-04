// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "lib/e2/procedures/ric_reconnection_routine.h"
#include "tests/unittests/e2/common/e2_test_helpers.h"
#include "ocudu/support/async/async_test_utils.h"
#include <gtest/gtest.h>

using namespace ocudu;

class dummy_ric_conn_mngr : public e2_connection_manager
{
public:
  int tnl_fail_count = 0;
  int tnl_call_count = 0;

  bool handle_e2_tnl_connection_request() override
  {
    ++tnl_call_count;
    return tnl_call_count > tnl_fail_count;
  }

  async_task<void> handle_e2_node_initiated_removal_request() override
  {
    return launch_async([](coro_context<async_task<void>>& ctx) {
      CORO_BEGIN(ctx);
      CORO_RETURN();
    });
  }

  async_task<void> handle_e2_disconnection_request() override
  {
    return launch_async([](coro_context<async_task<void>>& ctx) {
      CORO_BEGIN(ctx);
      CORO_RETURN();
    });
  }

  async_task<e2_setup_response_message> handle_e2_setup_request(const e2_setup_request_message&) override
  {
    auto resp = response;
    return launch_async([resp](coro_context<async_task<e2_setup_response_message>>& ctx) mutable {
      CORO_BEGIN(ctx);
      CORO_RETURN(resp);
    });
  }

  e2_setup_response_message response = {};
};

class ric_reconnection_routine_test : public ::testing::Test
{
protected:
  void SetUp() override
  {
    ocudulog::fetch_basic_logger("TEST").set_level(ocudulog::basic_levels::debug);
    ocudulog::init();

    cfg                             = config_helpers::make_default_e2ap_config();
    cfg.gnb_du_id                   = int_to_gnb_du_id(1);
    cfg.e2sm_kpm_enabled            = true;
    cfg.ric_reconnection_retry_time = std::chrono::milliseconds{100};
    factory                         = timer_factory{timers, task_worker};

    du_meas_provider = std::make_unique<dummy_e2sm_kpm_du_meas_provider>();
    e2sm_kpm_packer  = std::make_unique<e2sm_kpm_asn1_packer>(*du_meas_provider);
    kpm_iface        = std::make_unique<e2sm_kpm_impl>(test_logger, *e2sm_kpm_packer, *du_meas_provider);
    e2sm_mngr        = std::make_unique<e2sm_manager>(test_logger);
    e2sm_mngr->add_e2sm_service(e2sm_kpm_asn1_packer::oid, std::move(kpm_iface));

    conn_mngr                   = std::make_unique<dummy_ric_conn_mngr>();
    conn_mngr->response.success = true;
    node_cfg_event              = std::make_unique<manual_event<std::vector<e2_node_component_config>>>();
    node_cfg_provider           = std::make_unique<dummy_e2_node_component_config_provider>(*node_cfg_event);

    e2_node_component_config ncfg;
    ncfg.interface_type = e2_node_component_interface_type::f1;
    node_cfg_event->set(std::vector<e2_node_component_config>{std::move(ncfg)});
  }

  void TearDown() override { ocudulog::flush(); }

  void tick()
  {
    timers.tick();
    task_worker.run_pending_tasks();
  }

  e2ap_configuration                                                   cfg;
  timer_manager                                                        timers;
  manual_task_worker                                                   task_worker{64};
  timer_factory                                                        factory;
  std::atomic<bool>                                                    stopped{false};
  std::atomic<bool>                                                    ric_connected{false};
  std::unique_ptr<dummy_e2sm_kpm_du_meas_provider>                     du_meas_provider;
  std::unique_ptr<e2sm_kpm_asn1_packer>                                e2sm_kpm_packer;
  std::unique_ptr<e2sm_interface>                                      kpm_iface;
  std::unique_ptr<e2sm_manager>                                        e2sm_mngr;
  std::unique_ptr<dummy_ric_conn_mngr>                                 conn_mngr;
  std::unique_ptr<manual_event<std::vector<e2_node_component_config>>> node_cfg_event;
  std::unique_ptr<dummy_e2_node_component_config_provider>             node_cfg_provider;
  ocudulog::basic_logger& test_logger = ocudulog::fetch_basic_logger("TEST");
};

/// The routine must wait the initial timer before making the first TNL attempt.
TEST_F(ric_reconnection_routine_test, initial_timer_fires_before_tnl_attempt)
{
  async_task<bool> t = launch_async<ric_reconnection_routine>(
      cfg, *node_cfg_provider, *e2sm_mngr, *conn_mngr, factory, test_logger, stopped, ric_connected);
  lazy_task_launcher<bool> launcher(t);
  task_worker.run_pending_tasks();

  // Not ready - waiting on initial timer.
  ASSERT_FALSE(t.ready());
  ASSERT_EQ(conn_mngr->tnl_call_count, 0);

  // Advance past the initial timer; TNL succeeds -> setup succeeds -> done.
  for (unsigned ms = 0; ms <= 100; ++ms) {
    tick();
  }

  ASSERT_TRUE(t.ready());
  ASSERT_TRUE(t.get());
  ASSERT_TRUE(ric_connected.load());
  ASSERT_EQ(conn_mngr->tnl_call_count, 1);
}

/// When TNL fails the routine must retry after each timer period.
TEST_F(ric_reconnection_routine_test, tnl_retries_until_success_returns_true)
{
  conn_mngr->tnl_fail_count = 2;

  async_task<bool> t = launch_async<ric_reconnection_routine>(
      cfg, *node_cfg_provider, *e2sm_mngr, *conn_mngr, factory, test_logger, stopped, ric_connected);
  lazy_task_launcher<bool> launcher(t);
  task_worker.run_pending_tasks();

  // Advance through initial delay + 2 failing retries + 1 success.
  for (unsigned i = 0; i < 3; ++i) {
    ASSERT_FALSE(t.ready());
    for (unsigned ms = 0; ms <= 100; ++ms) {
      tick();
    }
  }

  ASSERT_TRUE(t.ready());
  ASSERT_TRUE(t.get());
  ASSERT_TRUE(ric_connected.load());
  ASSERT_EQ(conn_mngr->tnl_call_count, 3);
}

/// When TNL succeeds but setup is rejected the routine must return false.
TEST_F(ric_reconnection_routine_test, setup_rejected_returns_false)
{
  conn_mngr->response.success = false;

  async_task<bool> t = launch_async<ric_reconnection_routine>(
      cfg, *node_cfg_provider, *e2sm_mngr, *conn_mngr, factory, test_logger, stopped, ric_connected);
  lazy_task_launcher<bool> launcher(t);

  for (unsigned ms = 0; ms <= 100; ++ms) {
    tick();
  }

  ASSERT_TRUE(t.ready());
  ASSERT_FALSE(t.get());
  ASSERT_FALSE(ric_connected.load());
}

/// When stopped is set the routine must exit without completing a TNL attempt.
TEST_F(ric_reconnection_routine_test, stopped_flag_causes_early_exit)
{
  async_task<bool> t = launch_async<ric_reconnection_routine>(
      cfg, *node_cfg_provider, *e2sm_mngr, *conn_mngr, factory, test_logger, stopped, ric_connected);
  lazy_task_launcher<bool> launcher(t);
  task_worker.run_pending_tasks();

  stopped = true;

  for (unsigned ms = 0; ms <= 100; ++ms) {
    tick();
  }

  ASSERT_TRUE(t.ready());
  ASSERT_FALSE(t.get());
  ASSERT_FALSE(ric_connected.load());
  ASSERT_EQ(conn_mngr->tnl_call_count, 0);
}
