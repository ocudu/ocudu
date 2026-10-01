// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "f1ap_du_test_helpers.h"
#include "lib/f1ap/f1ap_asn1_utils.h"
#include "test_doubles/f1ap/f1ap_test_messages.h"
#include "ocudu/adt/format.h"
#include "ocudu/asn1/f1ap/f1ap_pdu_contents.h"
#include "ocudu/support/async/async_task.h"
#include "ocudu/support/async/async_test_utils.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace odu;

static f1ap_message generate_f1_setup_failure_with_time_to_wait(const f1ap_message&        f1_req,
                                                                asn1::f1ap::time_to_wait_e time_to_wait)
{
  f1ap_message msg = test_helpers::generate_f1_setup_failure(f1_req);
  msg.pdu.unsuccessful_outcome().value.f1_setup_fail()->time_to_wait_present = true;
  msg.pdu.unsuccessful_outcome().value.f1_setup_fail()->time_to_wait         = time_to_wait;
  return msg;
}

/// Test successful F1 Setup procedure.
TEST_F(f1ap_du_test, when_f1_setup_response_received_then_du_connected)
{
  // Action: Launch F1 setup procedure.
  ASSERT_TRUE(f1ap->connect_to_cu_cp());
  f1_setup_request_message request_msg = generate_f1_setup_request_message();
  test_logger.info("Launch f1 setup request procedure...");
  async_task<f1_setup_result>         t = f1ap->handle_f1_setup_request(request_msg);
  lazy_task_launcher<f1_setup_result> t_launcher(t);

  // Status: CU received F1 Setup Request.
  ASSERT_EQ(f1c_gw.last_tx_pdu().pdu.type().value, asn1::f1ap::f1ap_pdu_c::types_opts::init_msg);
  ASSERT_EQ(f1c_gw.last_tx_pdu().pdu.init_msg().value.type().value,
            asn1::f1ap::f1ap_elem_procs_o::init_msg_c::types_opts::f1_setup_request);
  const auto& f1_setup_req = f1c_gw.last_tx_pdu().pdu.init_msg().value.f1_setup_request();
  ASSERT_FALSE(
      f1_setup_req->gnb_du_served_cells_list[0]->gnb_du_served_cells_item().served_cell_info.meas_timing_cfg.empty());

  // Status: Procedure not yet ready.
  ASSERT_FALSE(t.ready());

  // Action: F1 Setup Response received.
  f1ap_message f1_setup_response = test_helpers::generate_f1_setup_response(f1c_gw.last_tx_pdu());
  test_logger.info("Injecting F1SetupResponse");
  f1ap->handle_message(f1_setup_response);

  ASSERT_TRUE(t.ready());
  ASSERT_TRUE(t.get().has_value());
  ASSERT_EQ(t.get().value().cells_to_activate.size(), f1_setup_req->gnb_du_served_cells_list.size());
}

/// Test successful F1 Setup procedure:
// - first DU receives F1 Setup Failure response with Time To Wait IE,
// - then DU recieves successful F1 Setup Response.
TEST_F(f1ap_du_test, when_f1_setup_failure_with_time_to_wait_received_then_retry_with_success)
{
  // Action: Launch F1 setup procedure.
  ASSERT_TRUE(f1ap->connect_to_cu_cp());
  f1_setup_request_message request_msg = generate_f1_setup_request_message();
  test_logger.info("Launch f1 setup request procedure...");
  async_task<f1_setup_result>         t = f1ap->handle_f1_setup_request(request_msg);
  lazy_task_launcher<f1_setup_result> t_launcher(t);

  // Status: CU received F1 Setup Request.
  f1ap_message f1_setup_req_1_pdu = f1c_gw.last_tx_pdu();
  ASSERT_EQ(f1_setup_req_1_pdu.pdu.type().value, asn1::f1ap::f1ap_pdu_c::types_opts::init_msg);
  ASSERT_EQ(f1_setup_req_1_pdu.pdu.init_msg().value.type().value,
            asn1::f1ap::f1ap_elem_procs_o::init_msg_c::types_opts::f1_setup_request);
  const auto f1_setup_req = f1_setup_req_1_pdu.pdu.init_msg().value.f1_setup_request();
  ASSERT_FALSE(
      f1_setup_req->gnb_du_served_cells_list[0]->gnb_du_served_cells_item().served_cell_info.meas_timing_cfg.empty());

  // Status: Procedure not yet ready.
  ASSERT_FALSE(t.ready());

  // Action: F1 Setup Failure with Time To Wait IE received.
  unsigned     transaction_id = get_transaction_id(f1_setup_req_1_pdu.pdu).value();
  f1ap_message f1_setup_failure =
      generate_f1_setup_failure_with_time_to_wait(f1_setup_req_1_pdu, asn1::f1ap::time_to_wait_opts::v10s);
  test_logger.info("Injecting F1SetupFailure with time to wait");
  f1c_gw.clear_tx_pdus();
  f1ap->handle_message(f1_setup_failure);

  // Status: CU does not receive new F1 Setup Request until time-to-wait has ended.
  for (unsigned msec_elapsed = 0; msec_elapsed < 10000; ++msec_elapsed) {
    ASSERT_FALSE(t.ready());
    ASSERT_EQ(f1c_gw.pop_tx_pdu(), std::nullopt);

    this->tick();
  }

  // Status: CU received F1 Setup Request again.
  f1ap_message f1_setup_req_2_pdu = f1c_gw.last_tx_pdu();
  ASSERT_EQ(f1_setup_req_2_pdu.pdu.type().value, asn1::f1ap::f1ap_pdu_c::types_opts::init_msg);
  ASSERT_EQ(f1_setup_req_2_pdu.pdu.init_msg().value.type().value,
            asn1::f1ap::f1ap_elem_procs_o::init_msg_c::types_opts::f1_setup_request);

  unsigned transaction_id2 = get_transaction_id(f1_setup_req_2_pdu.pdu).value();
  EXPECT_NE(transaction_id, transaction_id2);

  // Successful outcome after reinitiated F1 Setup procedure.
  f1ap_message f1_setup_response = test_helpers::generate_f1_setup_response(f1_setup_req_2_pdu);
  test_logger.info("Injecting F1SetupResponse");
  f1ap->handle_message(f1_setup_response);

  ASSERT_TRUE(t.ready());
  ASSERT_TRUE(t.get().has_value());
  ASSERT_EQ(t.get().value().cells_to_activate.size(), f1_setup_req->gnb_du_served_cells_list.size());
}

/// Test unsuccessful F1 Setup procedure:
// - first DU receives F1 Setup Failure response with Time To Wait IE,
// - then DU recieves F1 Setup Failure response without Time To Wait IE and the procedure fails.
TEST_F(f1ap_du_test, when_f1_setup_failure_with_time_to_wait_received_then_retry_without_success)
{
  // Action: Launch F1 setup procedure
  ASSERT_TRUE(f1ap->connect_to_cu_cp());
  f1_setup_request_message request_msg = generate_f1_setup_request_message();
  test_logger.info("Launch f1 setup request procedure...");
  async_task<f1_setup_result>         t = f1ap->handle_f1_setup_request(request_msg);
  lazy_task_launcher<f1_setup_result> t_launcher(t);

  // Status: CU received F1 Setup Request.
  ASSERT_EQ(f1c_gw.last_tx_pdu().pdu.type().value, asn1::f1ap::f1ap_pdu_c::types_opts::init_msg);
  ASSERT_EQ(f1c_gw.last_tx_pdu().pdu.init_msg().value.type().value,
            asn1::f1ap::f1ap_elem_procs_o::init_msg_c::types_opts::f1_setup_request);

  // Status: Procedure not yet ready.
  EXPECT_FALSE(t.ready());

  // Action: F1 Setup Failure with Time To Wait IE received.
  unsigned     transaction_id = get_transaction_id(f1c_gw.last_tx_pdu().pdu).value();
  f1ap_message f1_setup_failure =
      generate_f1_setup_failure_with_time_to_wait(f1c_gw.last_tx_pdu(), asn1::f1ap::time_to_wait_opts::v10s);
  test_logger.info("Injecting F1SetupFailure with time to wait");
  f1c_gw.clear_tx_pdus();
  f1ap->handle_message(f1_setup_failure);

  // Status: CU does not receive new F1 Setup Request until time-to-wait has ended.
  for (unsigned msec_elapsed = 0; msec_elapsed < 10000; ++msec_elapsed) {
    ASSERT_FALSE(t.ready());
    ASSERT_EQ(f1c_gw.pop_tx_pdu(), std::nullopt);

    this->tick();
  }

  // Status: CU received F1 Setup Request again.
  ASSERT_EQ(f1c_gw.last_tx_pdu().pdu.type().value, asn1::f1ap::f1ap_pdu_c::types_opts::init_msg);
  ASSERT_EQ(f1c_gw.last_tx_pdu().pdu.init_msg().value.type().value,
            asn1::f1ap::f1ap_elem_procs_o::init_msg_c::types_opts::f1_setup_request);

  unsigned transaction_id2 = get_transaction_id(f1c_gw.last_tx_pdu().pdu).value();
  EXPECT_NE(transaction_id, transaction_id2);

  // Unsuccessful outcome after reinitiated F1 Setup procedure and F1 Setup Failure without Time To Wait IE received.
  f1_setup_failure = test_helpers::generate_f1_setup_failure(f1c_gw.last_tx_pdu());
  test_logger.info("Injecting F1SetupFailure");
  f1ap->handle_message(f1_setup_failure);
  f1c_gw.clear_tx_pdus();

  ASSERT_TRUE(t.ready());
  ASSERT_FALSE(t.get().has_value());
  ASSERT_EQ(t.get().error().result, f1_setup_failure::result_code::f1_setup_failure);
  ASSERT_EQ(f1c_gw.pop_tx_pdu(), std::nullopt);

  async_task<void>         sctp_disconnect = f1ap->disconnect_from_cu_cp();
  lazy_task_launcher<void> sctp_disconnect_launcher(sctp_disconnect);
  this->tick(); // f1ap_du_connection_handler::rx_path_disconnected flag is set by defered event, tick to let it finish
}

/// Test unsuccessful F1 Setup procedure with F1 Setup Failure responses with Time To Wait IE and retry limit reached.
TEST_F(f1ap_du_test, when_retry_limit_reached_then_du_not_connected)
{
  // Action : Launch F1 setup procedure.
  ASSERT_TRUE(f1ap->connect_to_cu_cp());
  f1_setup_request_message request_msg = generate_f1_setup_request_message();
  test_logger.info("Launch f1 setup request procedure...");
  async_task<f1_setup_result>         t = f1ap->handle_f1_setup_request(request_msg);
  lazy_task_launcher<f1_setup_result> t_launcher(t);

  // Status: CU received F1 Setup Request.
  ASSERT_EQ(f1c_gw.last_tx_pdu().pdu.type().value, asn1::f1ap::f1ap_pdu_c::types_opts::init_msg);
  ASSERT_EQ(f1c_gw.last_tx_pdu().pdu.init_msg().value.type().value,
            asn1::f1ap::f1ap_elem_procs_o::init_msg_c::types_opts::f1_setup_request);

  // Status: Procedure not yet ready.
  ASSERT_FALSE(t.ready());

  for (unsigned i = 0; i < request_msg.max_setup_retries; i++) {
    // Status: F1 Setup Failure with Time To Wait IE received.
    f1ap_message f1_setup_response_msg =
        generate_f1_setup_failure_with_time_to_wait(f1c_gw.last_tx_pdu(), asn1::f1ap::time_to_wait_opts::v10s);
    f1c_gw.clear_tx_pdus();
    f1ap->handle_message(f1_setup_response_msg);

    // Status: CU does not receive new F1 Setup Request until time-to-wait has ended.
    for (unsigned msec_elapsed = 0; msec_elapsed < 10000; ++msec_elapsed) {
      ASSERT_FALSE(t.ready());
      ASSERT_EQ(f1c_gw.pop_tx_pdu(), std::nullopt);

      this->tick();
    }
  }

  // Unsuccessful outcome after F1 Setup Failure with Time To Wait IE received due to retry limit reached.
  f1ap_message f1_setup_response_msg =
      generate_f1_setup_failure_with_time_to_wait(f1c_gw.last_tx_pdu(), asn1::f1ap::time_to_wait_opts::v10s);
  f1c_gw.clear_tx_pdus();
  f1ap->handle_message(f1_setup_response_msg);

  ASSERT_TRUE(t.ready());
  ASSERT_FALSE(t.get().has_value());
  ASSERT_EQ(t.get().error().result, f1_setup_failure::result_code::f1_setup_failure);
  ASSERT_EQ(f1c_gw.pop_tx_pdu(), std::nullopt);

  async_task<void>         sctp_disconnect = f1ap->disconnect_from_cu_cp();
  lazy_task_launcher<void> sctp_disconnect_launcher(sctp_disconnect);
  this->tick(); // f1ap_du_connection_handler::rx_path_disconnected flag is set by defered event, tick to let it finish
}

/// Test unsuccessful F1 Setup procedure with F1 Setup Response timeout.
TEST_F(f1ap_du_test, when_timeout_then_du_not_connected)
{
  // Action : Launch F1 setup procedure.
  ASSERT_TRUE(f1ap->connect_to_cu_cp());
  f1_setup_request_message request_msg = generate_f1_setup_request_message();
  test_logger.info("Launch f1 setup request procedure...");
  async_task<f1_setup_result>         t = f1ap->handle_f1_setup_request(request_msg);
  lazy_task_launcher<f1_setup_result> t_launcher(t);

  // Status: CU received F1 Setup Request.
  ASSERT_EQ(f1c_gw.last_tx_pdu().pdu.type().value, asn1::f1ap::f1ap_pdu_c::types_opts::init_msg);
  ASSERT_EQ(f1c_gw.last_tx_pdu().pdu.init_msg().value.type().value,
            asn1::f1ap::f1ap_elem_procs_o::init_msg_c::types_opts::f1_setup_request);
  f1c_gw.clear_tx_pdus();

  // Status: Procedure not yet ready.
  ASSERT_FALSE(t.ready());

  // Status: Wait for F1 Setup Response timeout.
  for (unsigned msec_elapsed = 0; msec_elapsed < 3000; ++msec_elapsed) {
    ASSERT_FALSE(t.ready());
    ASSERT_EQ(f1c_gw.pop_tx_pdu(), std::nullopt);

    this->tick();
  }

  // Unsuccessful outcome after F1 Setup Response timeout.
  ASSERT_TRUE(t.ready());
  ASSERT_FALSE(t.get().has_value());
  ASSERT_EQ(t.get().error().result, f1_setup_failure::result_code::timeout);
  ASSERT_EQ(f1c_gw.pop_tx_pdu(), std::nullopt);

  async_task<void>         sctp_disconnect = f1ap->disconnect_from_cu_cp();
  lazy_task_launcher<void> sctp_disconnect_launcher(sctp_disconnect);
  this->tick(); // f1ap_du_connection_handler::rx_path_disconnected flag is set by defered event, tick to let it finish
}
