// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "common/e2ap_asn1_packer.h"
#include "lib/e2/common/e2ap_asn1_helpers.h"
#include "lib/e2/common/e2ap_asn1_utils.h"
#include "tests/unittests/e2/common/e2_test_helpers.h"
#include "ocudu/e2/e2_cu_up_factory.h"
#include "ocudu/support/async/async_test_utils.h"
#include <gtest/gtest.h>

using namespace ocudu;

/// Test the initial e2ap setup procedure with own task worker
TEST_F(e2_entity_test, on_start_send_e2ap_setup_request)
{
  test_logger.info("Launch e2 setup request procedure with task worker...");
  // Deliver a minimal F1 component config so the setup coroutine proceeds without suspending.
  node_component_config_collector->deliver(e2_node_component_interface_type::f1, byte_buffer{}, byte_buffer{});
  task_worker.run_pending_tasks();
  e2agent->start();

  // Status: received E2 Setup Request.
  ASSERT_EQ(e2_client->last_tx_e2_pdu.pdu.type().value, asn1::e2ap::e2ap_pdu_c::types_opts::init_msg);
  ASSERT_EQ(e2_client->last_tx_e2_pdu.pdu.init_msg().value.type().value,
            asn1::e2ap::e2ap_elem_procs_o::init_msg_c::types_opts::e2setup_request);

  // Action 2: E2 setup response received.
  unsigned   transaction_id    = get_transaction_id(e2_client->last_tx_e2_pdu.pdu).value();
  e2_message e2_setup_response = generate_e2_setup_response(transaction_id);
  e2_setup_response.pdu.successful_outcome()
      .value.e2setup_resp()
      ->ran_functions_accepted[0]
      ->ran_function_id_item()
      .ran_function_id = e2sm_kpm_asn1_packer::ran_func_id;
  ASSERT_FALSE(e2agent->is_ric_connected());

  test_logger.info("Injecting E2SetupResponse");
  e2agent->get_e2_interface().handle_message(e2_setup_response);

  ASSERT_TRUE(e2agent->is_ric_connected());
}

TEST_F(e2_entity_test, e2ap_setup_request_du_global_node_id_is_correct)
{
  const std::string test_plmn  = "99999";
  const gnb_du_id_t test_du_id = static_cast<gnb_du_id_t>(42);

  cfg.plmn             = test_plmn;
  cfg.gnb_du_id        = test_du_id;
  cfg.e2sm_kpm_enabled = true;
  auto owned_collector = std::make_unique<e2_node_component_config_collector>(task_worker, 1);
  owned_collector->deliver(e2_node_component_interface_type::f1, byte_buffer{}, byte_buffer{});
  e2agent = create_e2_du_agent(cfg,
                               *e2_client,
                               &(*du_metrics),
                               &(*f1ap_ue_id_mapper),
                               &(*du_rc_param_configurator),
                               factory,
                               task_worker,
                               std::move(owned_collector));
  task_worker.run_pending_tasks();
  e2agent->start();

  ASSERT_EQ(e2_client->last_tx_e2_pdu.pdu.type().value, asn1::e2ap::e2ap_pdu_c::types_opts::init_msg);
  const auto& e2setup       = e2_client->last_tx_e2_pdu.pdu.init_msg().value.e2setup_request();
  const auto& gnb           = e2setup->global_e2node_id.gnb();
  uint32_t    expected_plmn = bcd_helper::plmn_string_to_bcd(test_plmn);
  ASSERT_EQ(gnb.global_gnb_id.plmn_id.to_number(), expected_plmn);
  ASSERT_TRUE(gnb.gnb_du_id_present);
  ASSERT_EQ(gnb.gnb_du_id, gnb_du_id_to_int(test_du_id));
  ASSERT_FALSE(gnb.gnb_cu_up_id_present);
}

TEST_F(e2_entity_test, e2ap_setup_request_cu_cp_global_node_id_is_correct)
{
  const std::string test_plmn = "99999";

  cfg.plmn              = test_plmn;
  cfg.e2sm_kpm_enabled  = true;
  auto owned_collector  = std::make_unique<e2_node_component_config_collector>(task_worker, 1);
  auto cu_metrics       = std::make_unique<dummy_e2_cu_metrics>();
  auto cu_configurator_ = std::make_unique<dummy_cu_configurator>();
  owned_collector->deliver(e2_node_component_interface_type::ng, byte_buffer{}, byte_buffer{});
  e2agent = create_e2_cu_cp_agent(
      cfg, *e2_client, cu_metrics.get(), cu_configurator_.get(), factory, task_worker, std::move(owned_collector));
  task_worker.run_pending_tasks();
  e2agent->start();

  ASSERT_EQ(e2_client->last_tx_e2_pdu.pdu.type().value, asn1::e2ap::e2ap_pdu_c::types_opts::init_msg);
  const auto& e2setup       = e2_client->last_tx_e2_pdu.pdu.init_msg().value.e2setup_request();
  const auto& gnb           = e2setup->global_e2node_id.gnb();
  uint32_t    expected_plmn = bcd_helper::plmn_string_to_bcd(test_plmn);
  ASSERT_EQ(gnb.global_gnb_id.plmn_id.to_number(), expected_plmn);
  ASSERT_FALSE(gnb.gnb_du_id_present);
  ASSERT_FALSE(gnb.gnb_cu_up_id_present);
}

TEST_F(e2_entity_test, e2ap_setup_request_cu_up_global_node_id_is_correct)
{
  const std::string test_plmn = "99999";

  cfg.plmn              = test_plmn;
  cfg.gnb_cu_up_id      = static_cast<gnb_cu_up_id_t>(1);
  cfg.e2sm_kpm_enabled  = true;
  auto owned_collector  = std::make_unique<e2_node_component_config_collector>(task_worker, 1);
  auto cu_metrics       = std::make_unique<dummy_e2_cu_metrics>();
  auto cu_configurator_ = std::make_unique<dummy_cu_configurator>();
  owned_collector->deliver(e2_node_component_interface_type::e1, byte_buffer{}, byte_buffer{});
  e2agent = create_e2_cu_up_agent(
      cfg, *e2_client, cu_metrics.get(), cu_configurator_.get(), factory, task_worker, std::move(owned_collector));
  task_worker.run_pending_tasks();
  e2agent->start();

  ASSERT_EQ(e2_client->last_tx_e2_pdu.pdu.type().value, asn1::e2ap::e2ap_pdu_c::types_opts::init_msg);
  const auto& e2setup       = e2_client->last_tx_e2_pdu.pdu.init_msg().value.e2setup_request();
  const auto& gnb           = e2setup->global_e2node_id.gnb();
  uint32_t    expected_plmn = bcd_helper::plmn_string_to_bcd(test_plmn);
  ASSERT_EQ(gnb.global_gnb_id.plmn_id.to_number(), expected_plmn);
  ASSERT_FALSE(gnb.gnb_du_id_present);
  ASSERT_TRUE(gnb.gnb_cu_up_id_present);
  ASSERT_EQ(gnb.gnb_cu_up_id, gnb_cu_up_id_to_uint(static_cast<gnb_cu_up_id_t>(1)));
}

/// Test successful E2 setup procedure
TEST_F(e2_test, when_e2_setup_response_received_then_e2_connected)
{
  report_fatal_error_if_not(e2->handle_e2_tnl_connection_request(), "Unable to establish dummy SCTP connection");
  // Action 1: Launch E2 setup procedure
  test_logger.info("Launch e2 setup request procedure...");
  e2_setup_request_message                      request;
  async_task<e2_setup_response_message>         t = e2->handle_e2_setup_request(request);
  lazy_task_launcher<e2_setup_response_message> t_launcher(t);

  // Status: received E2 Setup Request.
  ASSERT_EQ(e2_client->last_tx_e2_pdu.pdu.type().value, asn1::e2ap::e2ap_pdu_c::types_opts::init_msg);
  ASSERT_EQ(e2_client->last_tx_e2_pdu.pdu.init_msg().value.type().value,
            asn1::e2ap::e2ap_elem_procs_o::init_msg_c::types_opts::e2setup_request);

  // Status: Procedure not yet ready.
  ASSERT_FALSE(t.ready());
  // Action 2: E2 setup response received.
  unsigned   transaction_id    = get_transaction_id(e2_client->last_tx_e2_pdu.pdu).value();
  e2_message e2_setup_response = generate_e2_setup_response(transaction_id);
  test_logger.info("Injecting E2SetupResponse");
  e2->handle_message(e2_setup_response);

  ASSERT_TRUE(t.ready());
  ASSERT_TRUE(t.get().success);
}

/// A response carrying a transaction id different from the one sent in the request must not be treated as a valid
/// completion of the E2 Setup procedure.
TEST_F(e2_test, when_e2_setup_response_has_mismatched_transaction_id_then_e2_setup_failed)
{
  report_fatal_error_if_not(e2->handle_e2_tnl_connection_request(), "Unable to establish dummy SCTP connection");
  test_logger.info("Launch e2 setup request procedure...");
  e2_setup_request_message                      request;
  async_task<e2_setup_response_message>         t = e2->handle_e2_setup_request(request);
  lazy_task_launcher<e2_setup_response_message> t_launcher(t);

  ASSERT_FALSE(t.ready());

  // Response carries a transaction id that does not match the one used in the request.
  unsigned   sent_transaction_id  = get_transaction_id(e2_client->last_tx_e2_pdu.pdu).value();
  unsigned   wrong_transaction_id = sent_transaction_id + 1;
  e2_message e2_setup_response    = generate_e2_setup_response(wrong_transaction_id);
  test_logger.info("Injecting E2SetupResponse with mismatched transaction id");
  e2->handle_message(e2_setup_response);

  ASSERT_TRUE(t.ready());
  ASSERT_FALSE(t.get().success);
}

TEST_F(e2_test, when_e2_setup_failure_received_then_e2_setup_failed)
{
  report_fatal_error_if_not(e2->handle_e2_tnl_connection_request(), "Unable to establish dummy SCTP connection");
  // Action 1: Launch E2 setup procedure
  test_logger.info("Launch e2 setup request procedure...");
  e2_setup_request_message                      request;
  async_task<e2_setup_response_message>         t = e2->handle_e2_setup_request(request);
  lazy_task_launcher<e2_setup_response_message> t_launcher(t);

  // Status: received E2 Setup Request.
  ASSERT_EQ(e2_client->last_tx_e2_pdu.pdu.type().value, asn1::e2ap::e2ap_pdu_c::types_opts::init_msg);
  ASSERT_EQ(e2_client->last_tx_e2_pdu.pdu.init_msg().value.type().value,
            asn1::e2ap::e2ap_elem_procs_o::init_msg_c::types_opts::e2setup_request);

  // Status: Procedure not yet ready.
  ASSERT_FALSE(t.ready());
  // Action 2: E2 setup response received.
  unsigned   transaction_id    = get_transaction_id(e2_client->last_tx_e2_pdu.pdu).value();
  e2_message e2_setup_response = generate_e2_setup_failure(transaction_id);
  test_logger.info("Injecting E2SetupFailure");
  e2->handle_message(e2_setup_response);

  ASSERT_TRUE(t.ready());
  ASSERT_FALSE(t.get().success);
}

TEST_F(e2_test, correctly_unpack_e2_response)
{
  report_fatal_error_if_not(e2->handle_e2_tnl_connection_request(), "Unable to establish dummy SCTP connection");
  // Action 1: Launch E2 setup procedure
  test_logger.info("Launch e2 setup request procedure...");
  e2_setup_request_message                      request;
  async_task<e2_setup_response_message>         t = e2->handle_e2_setup_request(request);
  lazy_task_launcher<e2_setup_response_message> t_launcher(t);

  // Status: received E2 Setup Request.
  ASSERT_EQ(e2_client->last_tx_e2_pdu.pdu.type().value, asn1::e2ap::e2ap_pdu_c::types_opts::init_msg);
  ASSERT_EQ(e2_client->last_tx_e2_pdu.pdu.init_msg().value.type().value,
            asn1::e2ap::e2ap_elem_procs_o::init_msg_c::types_opts::e2setup_request);

  // Status: Procedure not yet ready.
  ASSERT_FALSE(t.ready());
  // Action 2: E2 setup response received.
  uint8_t     e2_resp[]   = {0x20, 0x01, 0x00, 0x38, 0x00, 0x00, 0x04, 0x00, 0x31, 0x00, 0x02, 0x00, 0x00, 0x00, 0x04,
                             0x00, 0x07, 0x00, 0x00, 0xf1, 0x10, 0x00, 0x01, 0x90, 0x00, 0x09, 0x00, 0x0a, 0x00, 0x00,
                             0x06, 0x40, 0x05, 0x00, 0x00, 0x93, 0x00, 0x00, 0x00, 0x34, 0x00, 0x12, 0x00, 0x00, 0x00,
                             0x35, 0x00, 0x0c, 0x00, 0x00, 0xe0, 0x6e, 0x67, 0x69, 0x6e, 0x74, 0x65, 0x72, 0x66, 0x00};
  byte_buffer e2_resp_buf = byte_buffer::create(e2_resp, e2_resp + sizeof(e2_resp)).value();
  packer->handle_packed_pdu(std::move(e2_resp_buf));

  ASSERT_TRUE(t.ready());
  ASSERT_TRUE(t.get().success);
}

/// When deliver() is called with real bytes before start(), the E2 Setup Request
/// must include those bytes in the e2nodeComponentCfg request part.
TEST_F(e2_entity_test, when_node_component_config_delivered_then_real_bytes_in_e2_setup_request)
{
  uint8_t req_bytes[]  = {0x01, 0x02, 0x03, 0x04, 0x05};
  uint8_t resp_bytes[] = {0x06, 0x07, 0x08};

  byte_buffer req  = byte_buffer::create(req_bytes, req_bytes + sizeof(req_bytes)).value();
  byte_buffer resp = byte_buffer::create(resp_bytes, resp_bytes + sizeof(resp_bytes)).value();

  // Deliver real bytes before start() so the coroutine proceeds without suspending.
  node_component_config_collector->deliver(e2_node_component_interface_type::f1, std::move(req), std::move(resp));
  task_worker.run_pending_tasks();
  e2agent->start();

  ASSERT_EQ(e2_client->last_tx_e2_pdu.pdu.type().value, asn1::e2ap::e2ap_pdu_c::types_opts::init_msg);
  ASSERT_EQ(e2_client->last_tx_e2_pdu.pdu.init_msg().value.type().value,
            asn1::e2ap::e2ap_elem_procs_o::init_msg_c::types_opts::e2setup_request);

  const auto& comp_list =
      e2_client->last_tx_e2_pdu.pdu.init_msg().value.e2setup_request()->e2node_component_cfg_addition;
  ASSERT_FALSE(!comp_list.size());
  const auto& comp_cfg = comp_list[0].value().e2node_component_cfg_addition_item().e2node_component_cfg;
  ASSERT_EQ(comp_cfg.e2node_component_request_part.size(), sizeof(req_bytes));
  ASSERT_EQ(comp_cfg.e2node_component_resp_part.size(), sizeof(resp_bytes));
}

/// When the timeout fires before any deliver(), the aggregator fires with an empty vector
/// and the E2 setup must be aborted (no E2 Setup Request sent).
TEST_F(e2_entity_test, when_node_cfg_timeout_fires_with_empty_collection_then_no_e2_setup_request)
{
  e2agent->start();

  // Advance the timer past the 5-second node-component-config timeout.
  for (unsigned i = 0; i <= 5000; ++i) {
    tick();
  }

  // No E2 Setup Request must have been sent: the PDU type should still be its default (nulltype).
  ASSERT_NE(e2_client->last_tx_e2_pdu.pdu.type().value, asn1::e2ap::e2ap_pdu_c::types_opts::init_msg);
}

TEST_F(e2_test, when_e2_setup_failure_with_time_to_wait_received_then_retry_is_sent)
{
  report_fatal_error_if_not(e2->handle_e2_tnl_connection_request(), "Unable to establish dummy SCTP connection");

  e2_setup_request_message                      request;
  async_task<e2_setup_response_message>         t = e2->handle_e2_setup_request(request);
  lazy_task_launcher<e2_setup_response_message> t_launcher(t);

  ASSERT_EQ(e2_client->last_tx_e2_pdu.pdu.type().value, asn1::e2ap::e2ap_pdu_c::types_opts::init_msg);
  unsigned initial_transaction_id = get_transaction_id(e2_client->last_tx_e2_pdu.pdu).value();

  // Inject failure with TimeToWait = 1 second.
  e2_message failure_msg     = generate_e2_setup_failure(initial_transaction_id);
  auto&      fail            = failure_msg.pdu.unsuccessful_outcome().value.e2setup_fail();
  fail->time_to_wait_present = true;
  fail->time_to_wait.value   = asn1::e2ap::time_to_wait_opts::v1s;
  e2->handle_message(failure_msg);

  // Before the delay expires: task is suspended, no new request sent.
  ASSERT_FALSE(t.ready());
  for (unsigned i = 0; i < 999; ++i) {
    tick();
  }
  ASSERT_FALSE(t.ready());
  ASSERT_EQ(get_transaction_id(e2_client->last_tx_e2_pdu.pdu).value(), initial_transaction_id);

  // After the delay expires: a new E2SetupRequest with a fresh transaction ID is sent.
  for (unsigned i = 0; i < 2; ++i) {
    tick();
  }
  ASSERT_EQ(e2_client->last_tx_e2_pdu.pdu.type().value, asn1::e2ap::e2ap_pdu_c::types_opts::init_msg);
  ASSERT_EQ(e2_client->last_tx_e2_pdu.pdu.init_msg().value.type().value,
            asn1::e2ap::e2ap_elem_procs_o::init_msg_c::types_opts::e2setup_request);
  ASSERT_NE(get_transaction_id(e2_client->last_tx_e2_pdu.pdu).value(), initial_transaction_id);
}

TEST_F(e2_test, when_max_e2_setup_retries_exceeded_then_procedure_fails_without_retry)
{
  report_fatal_error_if_not(e2->handle_e2_tnl_connection_request(), "Unable to establish dummy SCTP connection");

  e2_setup_request_message request;
  request.max_setup_retries = 0;

  async_task<e2_setup_response_message>         t = e2->handle_e2_setup_request(request);
  lazy_task_launcher<e2_setup_response_message> t_launcher(t);

  unsigned   transaction_id  = get_transaction_id(e2_client->last_tx_e2_pdu.pdu).value();
  e2_message failure_msg     = generate_e2_setup_failure(transaction_id);
  auto&      fail            = failure_msg.pdu.unsuccessful_outcome().value.e2setup_fail();
  fail->time_to_wait_present = true;
  fail->time_to_wait.value   = asn1::e2ap::time_to_wait_opts::v1s;
  e2->handle_message(failure_msg);

  // With max_setup_retries = 0 the retry guard fires immediately: task completes without waiting.
  ASSERT_TRUE(t.ready());
  ASSERT_FALSE(t.get().success);
}

TEST_F(e2_test, when_e2_setup_succeeds_after_retry_then_result_is_success)
{
  report_fatal_error_if_not(e2->handle_e2_tnl_connection_request(), "Unable to establish dummy SCTP connection");

  e2_setup_request_message request;
  request.max_setup_retries = 1;

  async_task<e2_setup_response_message>         t = e2->handle_e2_setup_request(request);
  lazy_task_launcher<e2_setup_response_message> t_launcher(t);

  unsigned   initial_transaction_id = get_transaction_id(e2_client->last_tx_e2_pdu.pdu).value();
  e2_message failure_msg            = generate_e2_setup_failure(initial_transaction_id);
  auto&      fail                   = failure_msg.pdu.unsuccessful_outcome().value.e2setup_fail();
  fail->time_to_wait_present        = true;
  fail->time_to_wait.value          = asn1::e2ap::time_to_wait_opts::v1s;
  e2->handle_message(failure_msg);

  ASSERT_FALSE(t.ready());

  // Advance past the 1-second retry delay.
  for (unsigned i = 0; i < 1001; ++i) {
    tick();
  }

  // A new E2SetupRequest should have been sent with a fresh transaction ID.
  unsigned retry_transaction_id = get_transaction_id(e2_client->last_tx_e2_pdu.pdu).value();
  ASSERT_NE(retry_transaction_id, initial_transaction_id);

  // Inject success on the retry transaction.
  e2_message e2_setup_response = generate_e2_setup_response(retry_transaction_id);
  e2->handle_message(e2_setup_response);

  ASSERT_TRUE(t.ready());
  ASSERT_TRUE(t.get().success);
}

/// fill_asn1_e2ap_setup_request: with a vector containing a real node_cfg entry the component
/// bytes in the ASN.1 struct equal those provided; with an empty vector no entries are created.
TEST_F(e2_test, fill_e2ap_setup_request_uses_real_bytes_when_node_cfg_vector_provided)
{
  using namespace asn1::e2ap;

  cfg           = config_helpers::make_default_e2ap_config();
  cfg.gnb_du_id = static_cast<gnb_du_id_t>(42);

  uint8_t req_bytes[]  = {0xaa, 0xbb, 0xee};
  uint8_t resp_bytes[] = {0xdd, 0xee};

  e2_node_component_config node_cfg;
  node_cfg.interface_type = e2_node_component_interface_type::f1;
  node_cfg.request_part   = byte_buffer::create(req_bytes, req_bytes + sizeof(req_bytes)).value();
  node_cfg.response_part  = byte_buffer::create(resp_bytes, resp_bytes + sizeof(resp_bytes)).value();

  std::vector<e2_node_component_config> cfgs_real;
  cfgs_real.push_back(std::move(node_cfg));

  e2setup_request_s setup_real{};
  fill_asn1_e2ap_setup_request(test_logger, setup_real, cfg, *e2sm_mngr, cfgs_real);

  e2setup_request_s setup_empty{};
  fill_asn1_e2ap_setup_request(test_logger, setup_empty, cfg, *e2sm_mngr, {});

  ASSERT_FALSE(!setup_real->e2node_component_cfg_addition.size());
  // Empty vector: no component entries.
  ASSERT_TRUE(!setup_empty->e2node_component_cfg_addition.size());

  const auto& real_comp =
      setup_real->e2node_component_cfg_addition[0].value().e2node_component_cfg_addition_item().e2node_component_cfg;

  // Real-bytes path must reproduce the exact sizes supplied.
  ASSERT_EQ(real_comp.e2node_component_request_part.size(), sizeof(req_bytes));
  ASSERT_EQ(real_comp.e2node_component_resp_part.size(), sizeof(resp_bytes));

  // Verify all global_e2node_id fields are correctly encoded from e2ap_configuration.
  const auto& gnb = setup_real->global_e2node_id.gnb();
  ASSERT_EQ(gnb.global_gnb_id.plmn_id.to_number(), bcd_helper::plmn_string_to_bcd(cfg.plmn));
  ASSERT_EQ(gnb.global_gnb_id.gnb_id.gnb_id().to_number(), static_cast<uint64_t>(cfg.gnb_id.id));
  ASSERT_EQ(gnb.global_gnb_id.gnb_id.gnb_id().length(), static_cast<uint32_t>(cfg.gnb_id.bit_length));
  ASSERT_TRUE(gnb.gnb_du_id_present);
  ASSERT_EQ(gnb.gnb_du_id, gnb_du_id_to_int(cfg.gnb_du_id.value()));
  ASSERT_FALSE(gnb.gnb_cu_up_id_present);
}
