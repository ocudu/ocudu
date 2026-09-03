// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "common/e2ap_asn1_packer.h"
#include "dummy_ric.h"
#include "lib/e2/common/e2_impl.h"
#include "lib/e2/common/e2ap_asn1_helpers.h"
#include "tests/unittests/e2/common/e2_test_helpers.h"
#include "ocudu/e2/e2_node_component_config_collector.h"
#include "ocudu/e2/e2ap_configuration_helpers.h"
#include "ocudu/e2/e2sm/e2sm_manager.h"
#include "ocudu/e2/gateways/e2_connection_client.h"
#include "ocudu/e2/gateways/e2_network_client_factory.h"
#include "ocudu/support/async/async_test_utils.h"
#include "ocudu/support/executors/inline_task_executor.h"
#include "ocudu/support/executors/manual_task_worker.h"
#include "ocudu/support/executors/task_worker.h"
#include "ocudu/support/io/io_broker_factory.h"
#include "ocudu/support/timers.h"
#include <chrono>
#include <gtest/gtest.h>
#include <list>
#include <mutex>
#include <optional>
#include <thread>
#include <utility>

using namespace ocudu;

class e2ap_network_adapter_test : public ::testing::Test
{
protected:
  void tick()
  {
    timers.tick();
    task_exec.run_pending_tasks();
  }

  void SetUp() override
  {
    ocudulog::fetch_basic_logger("TEST").set_level(ocudulog::basic_levels::debug);
    ocudulog::init();

    broker = create_io_broker(io_broker_type::epoll);

    // simulate RIC side
    ric_rx_e2_sniffer = std::make_unique<e2_sniffer>();
    ric_pcap          = std::make_unique<dummy_e2ap_pcap>();
    ric_sctp_gateway_config ric_server_sctp_cfg{{}, *broker, rx_executor, ctrl_executor, *ric_pcap};
    ric_server_sctp_cfg.sctp.if_name        = "E2";
    ric_server_sctp_cfg.sctp.ppid           = NGAP_PPID;
    ric_server_sctp_cfg.sctp.bind_addresses = {"127.0.0.1"};
    ric_server_sctp_cfg.sctp.bind_port      = 0;
    ric_server_sctp_cfg.rx_sniffer          = ric_rx_e2_sniffer.get();

    ric_net_adapter = create_e2_gateway_server(ric_server_sctp_cfg);
    ric             = std::make_unique<near_rt_ric>();
    ric_net_adapter->attach_ric(ric->get_ric_e2_handler());

    std::optional<uint16_t> ric_gw_port = ric_net_adapter->get_listen_port();
    ASSERT_TRUE(ric_gw_port.has_value());
    ASSERT_NE(ric_gw_port.value(), 0);

    // create E2 agent
    sctp_network_connector_config e2agent_config;
    e2agent_config.dest_name         = "NearRT-RIC";
    e2agent_config.if_name           = "E2";
    e2agent_config.connect_addresses = {"127.0.0.1"};
    e2agent_config.connect_port      = ric_gw_port.value();
    e2agent_config.bind_addresses    = {"127.0.0.101"};
    e2agent_config.bind_port         = 0;

    cfg                  = config_helpers::make_default_e2ap_config();
    cfg.e2sm_kpm_enabled = true;

    pcap      = std::make_unique<dummy_e2ap_pcap>();
    e2_client = create_e2_gateway_client(
        e2_sctp_gateway_config{e2agent_config, *broker, rx_executor, *pcap, ocudulog::fetch_basic_logger("E2")});
    du_metrics       = std::make_unique<dummy_e2_du_metrics>();
    du_meas_provider = std::make_unique<dummy_e2sm_kpm_du_meas_provider>();
    e2sm_packer      = std::make_unique<e2sm_kpm_asn1_packer>(*du_meas_provider);
    e2sm_iface       = std::make_unique<e2sm_kpm_impl>(test_logger, *e2sm_packer, *du_meas_provider);
    e2sm_mngr        = std::make_unique<e2sm_manager>(test_logger);
    e2sm_mngr->add_e2sm_service(e2sm_kpm_asn1_packer::oid, std::move(e2sm_iface));
    e2_subscription_mngr = std::make_unique<e2_subscription_manager_impl>(*e2sm_mngr);
    factory              = timer_factory{timers, task_exec};
    e2agent_notifier     = std::make_unique<dummy_e2_agent_mng>();
    e2ap                 = std::make_unique<e2_impl>(e2_impl_dependencies{.logger            = ocudulog::fetch_basic_logger("E2"),
                                                                          .agent_notifier    = *e2agent_notifier,
                                                                          .timers            = factory,
                                                                          .e2_client         = *e2_client,
                                                                          .subscription_mngr = *e2_subscription_mngr,
                                                                          .e2sm_mngr         = *e2sm_mngr,
                                                                          .task_exec         = task_exec});
  }

  void TearDown() override
  {
    ric_net_adapter->stop();
    // flush logger after each test
    ocudulog::flush();
  }

  class e2_sniffer : public e2_message_notifier
  {
  public:
    void on_new_message(const e2_message& msg) override
    {
      std::lock_guard<std::mutex> lock(rx_mutex);
      e2_msg_queue.push_back(msg);
    }

    std::optional<e2_message> try_get_last_e2_msg()
    {
      std::lock_guard<std::mutex> lock(rx_mutex);
      if (e2_msg_queue.empty()) {
        return std::nullopt;
      }
      e2_message msg = std::move(e2_msg_queue.front());
      e2_msg_queue.pop_front();
      return msg;
    }

  private:
    std::mutex            rx_mutex;
    std::list<e2_message> e2_msg_queue;
  };

  inline_task_executor       rx_executor;
  task_worker                ctrl_worker{"sctp server", 1024};
  task_worker_executor       ctrl_executor{ctrl_worker};
  std::unique_ptr<io_broker> broker;

  // dummy RIC
  std::unique_ptr<near_rt_ric>          ric;
  std::unique_ptr<dummy_e2ap_pcap>      ric_pcap;
  std::unique_ptr<e2_connection_server> ric_net_adapter;
  std::unique_ptr<e2_sniffer>           ric_rx_e2_sniffer;

  // E2 agent
  e2ap_config                              cfg;
  timer_factory                            factory;
  timer_manager                            timers;
  std::unique_ptr<e2ap_e2agent_notifier>   e2agent_notifier;
  std::unique_ptr<e2_connection_client>    e2_client;
  manual_task_worker                       task_exec{128};
  std::unique_ptr<dummy_e2ap_pcap>         pcap;
  std::unique_ptr<e2_subscription_manager> e2_subscription_mngr;
  std::unique_ptr<e2sm_handler>            e2sm_packer;
  std::unique_ptr<e2sm_manager>            e2sm_mngr;
  std::unique_ptr<e2_du_metrics_interface> du_metrics;
  std::unique_ptr<e2sm_kpm_meas_provider>  du_meas_provider;
  std::unique_ptr<e2sm_interface>          e2sm_iface;
  std::unique_ptr<e2_interface>            e2ap;
  ocudulog::basic_logger&                  test_logger = ocudulog::fetch_basic_logger("TEST");
};

/// Test successful e2 setup procedure
TEST_F(e2ap_network_adapter_test, when_e2_setup_response_received_then_ric_connected)
{
  report_fatal_error_if_not(e2ap->handle_e2_tnl_connection_request(), "Unable to establish connection to RIC");

  // Action 1: Launch E2 setup procedure (sends E2 Setup Request to RIC).
  test_logger.info("Launching E2 setup procedure...");
  e2_setup_request_message request;
  {
    std::vector<e2_node_component_config> node_cfgs;
    e2_node_component_config              dummy_cfg;
    dummy_cfg.interface_type = e2_node_component_interface_type::ng;
    node_cfgs.push_back(std::move(dummy_cfg));
    fill_asn1_e2ap_setup_request(test_logger, request.request, cfg, *e2sm_mngr, node_cfgs);
  }
  async_task<e2_setup_response_message>         t1 = e2ap->handle_e2_setup_request(request);
  lazy_task_launcher<e2_setup_response_message> t1_launcher(t1);
  ASSERT_FALSE(t1.ready());

  // Status: RIC received E2 Setup Request.
  std::optional<e2_message> last_e2_msg;
  {
    auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(5);
    while (!last_e2_msg && std::chrono::steady_clock::now() < deadline) {
      last_e2_msg = ric_rx_e2_sniffer->try_get_last_e2_msg();
      if (!last_e2_msg) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
      }
    }
  }
  ASSERT_TRUE(last_e2_msg.has_value());
  ASSERT_EQ(last_e2_msg.value().pdu.type().value, asn1::e2ap::e2ap_pdu_c::types_opts::init_msg);
  ASSERT_EQ(last_e2_msg.value().pdu.init_msg().value.type().value,
            asn1::e2ap::e2ap_elem_procs_o::init_msg_c::types_opts::e2setup_request);

  // Action 2: RIC sends E2 Setup Request Response.
  test_logger.info("Injecting E2SetupResponse");
  e2_message e2_setup_response = {};
  e2_setup_response.pdu.set_successful_outcome();
  e2_setup_response.pdu.successful_outcome().load_info_obj(ASN1_E2AP_ID_E2SETUP);

  auto& setup                           = e2_setup_response.pdu.successful_outcome().value.e2setup_resp();
  setup->transaction_id                 = last_e2_msg.value().pdu.init_msg().value.e2setup_request()->transaction_id;
  setup->ran_functions_accepted_present = true;
  asn1::protocol_ie_single_container_s<asn1::e2ap::ran_function_id_item_ies_o> ran_func_item;
  ran_func_item.value().ran_function_id_item().ran_function_id       = e2sm_kpm_asn1_packer::ran_func_id;
  ran_func_item.value().ran_function_id_item().ran_function_revision = 0;
  setup->ran_functions_accepted.push_back(ran_func_item);
  setup->global_ric_id.plmn_id.from_number(1);
  setup->global_ric_id.ric_id.from_number(1);
  // fill the required part with dummy data
  setup->e2node_component_cfg_addition_ack.resize(1);
  asn1::e2ap::e2node_component_cfg_addition_ack_item_s& e2node_component_cfg_addition_ack_item =
      setup->e2node_component_cfg_addition_ack[0].value().e2node_component_cfg_addition_ack_item();
  e2node_component_cfg_addition_ack_item.e2node_component_interface_type =
      asn1::e2ap::e2node_component_interface_type_e::e2node_component_interface_type_opts::e1;
  e2node_component_cfg_addition_ack_item.e2node_component_id.set_e2node_component_interface_type_e1().gnb_cu_up_id =
      123;
  e2node_component_cfg_addition_ack_item.e2node_component_cfg_ack.upd_outcome =
      asn1::e2ap::e2node_component_cfg_ack_s::upd_outcome_opts::success;

  ric->send_msg(0, e2_setup_response);

  // Status: E2 Agent processed E2 Setup Response (t1 coroutine completes).
  {
    auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(5);
    while (!t1.ready() && std::chrono::steady_clock::now() < deadline) {
      tick();
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
  }
  ASSERT_TRUE(t1.ready());

  async_task<void>         t2 = e2ap->handle_e2_disconnection_request();
  lazy_task_launcher<void> t2_launcher(t2);
  while (not t2.ready()) {
    this->tick();
  }

  test_logger.info("Test finished.");
}
