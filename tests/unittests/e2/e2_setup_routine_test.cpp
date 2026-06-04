// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "lib/e2/procedures/e2_setup_routine.h"
#include "tests/unittests/e2/common/e2_test_helpers.h"
#include "ocudu/support/async/async_test_utils.h"
#include <gtest/gtest.h>

using namespace ocudu;

/// Minimal e2_connection_manager stub that captures request metadata and returns a pre-set response.
class dummy_e2_conn_mngr_for_routine : public e2_connection_manager
{
public:
  bool handle_e2_tnl_connection_request() override { return true; }

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

  async_task<e2_setup_response_message> handle_e2_setup_request(const e2_setup_request_message& req) override
  {
    nof_ran_funcs = req.request->ran_functions_added.size();
    ran_func_ids.clear();
    for (const auto& rf : req.request->ran_functions_added) {
      ran_func_ids.push_back(rf.value().ran_function_item().ran_function_id);
    }
    auto resp = response;
    return launch_async([resp](coro_context<async_task<e2_setup_response_message>>& ctx) mutable {
      CORO_BEGIN(ctx);
      CORO_RETURN(resp);
    });
  }

  size_t                    nof_ran_funcs = 0;
  std::vector<uint16_t>     ran_func_ids;
  e2_setup_response_message response = {};
};

class e2_setup_routine_test : public ::testing::Test
{
protected:
  void SetUp() override
  {
    ocudulog::fetch_basic_logger("TEST").set_level(ocudulog::basic_levels::debug);
    ocudulog::init();

    cfg           = config_helpers::make_default_e2ap_config();
    cfg.gnb_du_id = int_to_gnb_du_id(1);
    factory       = timer_factory{timers, task_worker};

    du_meas_provider = std::make_unique<dummy_e2sm_kpm_du_meas_provider>();
    e2sm_kpm_packer  = std::make_unique<e2sm_kpm_asn1_packer>(*du_meas_provider);
    kpm_iface        = std::make_unique<e2sm_kpm_impl>(test_logger, *e2sm_kpm_packer, *du_meas_provider);
    e2sm_rc_packer   = std::make_unique<e2sm_rc_asn1_packer>();
    rc_iface         = std::make_unique<e2sm_rc_impl>(test_logger, *e2sm_rc_packer);
    e2sm_mngr        = std::make_unique<e2sm_manager>(test_logger);
    e2sm_mngr->add_e2sm_service(e2sm_kpm_asn1_packer::oid, std::move(kpm_iface));
    e2sm_mngr->add_e2sm_service(e2sm_rc_asn1_packer::oid, std::move(rc_iface));

    conn_mngr                   = std::make_unique<dummy_e2_conn_mngr_for_routine>();
    conn_mngr->response.success = true;
    node_cfg_event              = std::make_unique<manual_event<std::vector<e2_node_component_config>>>();
    node_cfg_provider           = std::make_unique<dummy_e2_node_component_config_provider>(*node_cfg_event);
  }

  void TearDown() override { ocudulog::flush(); }

  void deliver_node_cfg()
  {
    e2_node_component_config ncfg;
    ncfg.interface_type = e2_node_component_interface_type::f1;
    node_cfg_event->set(std::vector<e2_node_component_config>{std::move(ncfg)});
    task_worker.run_pending_tasks();
  }

  e2ap_configuration cfg;
  timer_manager      timers;
  manual_task_worker task_worker{64};
  timer_factory      factory;

  std::unique_ptr<dummy_e2sm_kpm_du_meas_provider>                     du_meas_provider;
  std::unique_ptr<e2sm_kpm_asn1_packer>                                e2sm_kpm_packer;
  std::unique_ptr<e2sm_rc_asn1_packer>                                 e2sm_rc_packer;
  std::unique_ptr<e2sm_interface>                                      kpm_iface;
  std::unique_ptr<e2sm_interface>                                      rc_iface;
  std::unique_ptr<e2sm_manager>                                        e2sm_mngr;
  std::unique_ptr<dummy_e2_conn_mngr_for_routine>                      conn_mngr;
  std::unique_ptr<manual_event<std::vector<e2_node_component_config>>> node_cfg_event;
  std::unique_ptr<dummy_e2_node_component_config_provider>             node_cfg_provider;

  ocudulog::basic_logger& test_logger = ocudulog::fetch_basic_logger("TEST");
};

/// When only KPM is enabled the routine must send exactly one RAN function with the KPM ID.
TEST_F(e2_setup_routine_test, kpm_only_request_has_one_ran_function)
{
  cfg.e2sm_kpm_enabled = true;
  cfg.e2sm_rc_enabled  = false;

  async_task<bool> t =
      launch_async<e2_setup_routine>(cfg, *node_cfg_provider, *e2sm_mngr, *conn_mngr, factory, test_logger);
  lazy_task_launcher<bool> launcher(t);

  ASSERT_FALSE(t.ready());
  deliver_node_cfg();

  ASSERT_TRUE(t.ready());
  ASSERT_TRUE(t.get());
  ASSERT_EQ(conn_mngr->nof_ran_funcs, 1U);
  ASSERT_EQ(conn_mngr->ran_func_ids[0], e2sm_kpm_asn1_packer::ran_func_id);
}

/// When both KPM and RC are enabled the routine must send two RAN functions in order: KPM then RC.
TEST_F(e2_setup_routine_test, kpm_and_rc_request_has_two_ran_functions)
{
  cfg.e2sm_kpm_enabled = true;
  cfg.e2sm_rc_enabled  = true;

  async_task<bool> t =
      launch_async<e2_setup_routine>(cfg, *node_cfg_provider, *e2sm_mngr, *conn_mngr, factory, test_logger);
  lazy_task_launcher<bool> launcher(t);

  ASSERT_FALSE(t.ready());
  deliver_node_cfg();

  ASSERT_TRUE(t.ready());
  ASSERT_TRUE(t.get());
  ASSERT_EQ(conn_mngr->nof_ran_funcs, 2U);
  ASSERT_EQ(conn_mngr->ran_func_ids[0], e2sm_kpm_asn1_packer::ran_func_id);
  ASSERT_EQ(conn_mngr->ran_func_ids[1], e2sm_rc_asn1_packer::ran_func_id);
}

/// When the RIC accepts a RAN function the routine must register it in the E2SM manager.
TEST_F(e2_setup_routine_test, accepted_ran_function_registered_in_e2sm_manager)
{
  cfg.e2sm_kpm_enabled = true;

  // Pre-configure a success response that accepts the KPM RAN function.
  conn_mngr->response.success                                  = true;
  conn_mngr->response.response->ran_functions_accepted_present = true;
  asn1::protocol_ie_single_container_s<asn1::e2ap::ran_function_id_item_ies_o> item;
  item.value().ran_function_id_item().ran_function_id       = e2sm_kpm_asn1_packer::ran_func_id;
  item.value().ran_function_id_item().ran_function_revision = 0;
  conn_mngr->response.response->ran_functions_accepted.push_back(item);

  // KPM is not yet reachable by numeric ID.
  ASSERT_EQ(e2sm_mngr->get_e2sm_interface(static_cast<uint16_t>(e2sm_kpm_asn1_packer::ran_func_id)), nullptr);

  async_task<bool> t =
      launch_async<e2_setup_routine>(cfg, *node_cfg_provider, *e2sm_mngr, *conn_mngr, factory, test_logger);
  lazy_task_launcher<bool> launcher(t);
  deliver_node_cfg();

  ASSERT_TRUE(t.ready());
  ASSERT_TRUE(t.get());
  // After a successful response the routine registers the accepted RAN function.
  ASSERT_NE(e2sm_mngr->get_e2sm_interface(static_cast<uint16_t>(e2sm_kpm_asn1_packer::ran_func_id)), nullptr);
}
