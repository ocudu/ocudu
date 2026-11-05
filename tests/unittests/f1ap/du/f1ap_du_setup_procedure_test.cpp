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

/// Test that a cell with more than 6 PLMNs reports PLMNs 7 onwards in the Extended Served PLMNs List.
TEST_F(f1ap_du_test, when_cell_has_more_than_6_plmns_then_the_extra_plmns_go_in_the_extended_served_plmns_list)
{
  ASSERT_TRUE(f1ap->connect_to_cu_cp());
  f1_setup_request_message request_msg = generate_f1_setup_request_message();
  du_served_cell_info&     cell_info   = request_msg.served_cells[0].cell_info;
  for (unsigned i = 0; i != 7; ++i) {
    cell_info.additional_plmns.push_back(plmn_identity::parse(fmt::format("9990{}", i + 1)).value());
  }
  async_task<f1_setup_result>         t = f1ap->handle_f1_setup_request(request_msg);
  lazy_task_launcher<f1_setup_result> t_launcher(t);

  // TS 38.473 section 9.3.1.10: Served PLMNs holds up to maxnoofBPLMNs (6) PLMNs, the others go in the Extended Served
  // PLMNs List.
  const auto& f1_setup_req     = f1c_gw.last_tx_pdu().pdu.init_msg().value.f1_setup_request();
  const auto& served_cell_info = f1_setup_req->gnb_du_served_cells_list[0]->gnb_du_served_cells_item().served_cell_info;
  ASSERT_EQ(served_cell_info.served_plmns.size(), 6U);
  ASSERT_EQ(plmn_identity::from_bytes(served_cell_info.served_plmns[0].plmn_id.to_bytes()).value(),
            cell_info.nr_cgi.plmn_id);
  ASSERT_TRUE(served_cell_info.ie_exts_present);
  ASSERT_TRUE(served_cell_info.ie_exts.extended_served_plmns_list_present);
  ASSERT_EQ(served_cell_info.ie_exts.extended_served_plmns_list.size(), 2U);
  ASSERT_EQ(
      plmn_identity::from_bytes(served_cell_info.ie_exts.extended_served_plmns_list[1].plmn_id.to_bytes()).value(),
      cell_info.additional_plmns[6]);

  // The F1 Setup Request can be packed.
  byte_buffer   packed;
  asn1::bit_ref bref{packed};
  ASSERT_EQ(asn1::pack(bref, f1c_gw.last_tx_pdu().pdu), asn1::OCUDUASN_SUCCESS);
}

/// Test unsuccessful F1 Setup procedure with F1 Setup Failure response with Time To Wait IE.
TEST_F(f1ap_du_test, when_f1_setup_failure_with_time_to_wait_received_then_procedure_fails_with_time_to_wait)
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

  // Status: Procedure not yet ready.
  ASSERT_FALSE(t.ready());

  // Action: F1 Setup Failure with Time To Wait IE received.
  f1ap_message f1_setup_failure =
      generate_f1_setup_failure_with_time_to_wait(f1c_gw.last_tx_pdu(), asn1::f1ap::time_to_wait_opts::v10s);
  test_logger.info("Injecting F1SetupFailure with time to wait");
  f1c_gw.clear_tx_pdus();
  f1ap->handle_message(f1_setup_failure);

  // Status: The procedure fails without a new F1 Setup Request and forwards the Time To Wait to its caller.
  ASSERT_TRUE(t.ready());
  ASSERT_FALSE(t.get().has_value());
  ASSERT_EQ(t.get().error().result, f1_setup_failure::result_code::f1_setup_failure);
  ASSERT_EQ(t.get().error().time_to_wait, std::chrono::seconds{10});
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
