// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "../common/test_helpers.h"
#include "e1ap_cu_up_test_helpers.h"
#include "lib/e1ap/common/e1ap_asn1_utils.h"
#include "ocudu/asn1/e1ap/e1ap_pdu_contents.h"
#include "ocudu/ran/bcd_helper.h"
#include "ocudu/support/async/async_test_utils.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace ocuup;
using namespace asn1::e1ap;

//////////////////////////////////////////////////////////////////////////////////////
/* Handling of unsupported messages                                                 */
//////////////////////////////////////////////////////////////////////////////////////

TEST_F(e1ap_cu_up_test, when_unsupported_init_msg_received_then_message_ignored)
{
  // Set last message of PDU notifier to successful outcome.
  e1ap_gw.last_tx_e1ap_pdu.pdu.set_init_msg();

  // Generate unupported E1AP PDU.
  e1ap_message unsupported_msg = {};
  unsupported_msg.pdu.set_init_msg();

  e1ap->handle_message(unsupported_msg);

  // Check that PDU has not been forwarded.
  EXPECT_EQ(e1ap_gw.last_tx_e1ap_pdu.pdu.type(), asn1::e1ap::e1ap_pdu_c::types_opts::options::init_msg);
}

TEST_F(e1ap_cu_up_test, when_unsupported_successful_outcome_received_then_message_ignored)
{
  // Set last message of PDU notifier to init_msg.
  e1ap_gw.last_tx_e1ap_pdu.pdu.set_init_msg();

  // Generate unupported E1AP PDU.
  e1ap_message unsupported_msg = {};
  unsupported_msg.pdu.set_successful_outcome();

  e1ap->handle_message(unsupported_msg);

  // Check that PDU has not been forwarded.
  EXPECT_EQ(e1ap_gw.last_tx_e1ap_pdu.pdu.type(), asn1::e1ap::e1ap_pdu_c::types_opts::options::init_msg);
}

TEST_F(e1ap_cu_up_test, when_unsupported_unsuccessful_outcome_received_then_message_ignored)
{
  // Set last message of PDU notifier to init_msg.
  e1ap_gw.last_tx_e1ap_pdu.pdu.set_init_msg();

  // Generate unupported E1AP PDU.
  e1ap_message unsupported_msg = {};
  unsupported_msg.pdu.set_unsuccessful_outcome();

  e1ap->handle_message(unsupported_msg);

  // Check that PDU has not been forwarded.
  EXPECT_EQ(e1ap_gw.last_tx_e1ap_pdu.pdu.type(), asn1::e1ap::e1ap_pdu_c::types_opts::options::init_msg);
}

//////////////////////////////////////////////////////////////////////////////////////
/* Handling of E1 Setup                                                             */
//////////////////////////////////////////////////////////////////////////////////////

/// Test the successful CU-UP initiated E1 setup
TEST_F(e1ap_cu_up_test, when_cu_up_started_then_e1_setup_request_sent)
{
  run_e1_setup_procedure();

  // Check the generated PDU is indeed the E1 Setup Request.
  ASSERT_EQ(e1ap_gw.last_tx_e1ap_pdu.pdu.type(), asn1::e1ap::e1ap_pdu_c::types_opts::options::init_msg);
  ASSERT_EQ(e1ap_gw.last_tx_e1ap_pdu.pdu.init_msg().value.type(),
            asn1::e1ap::e1ap_elem_procs_o::init_msg_c::types_opts::options::gnb_cu_up_e1_setup_request);
}

/// Test that a single PLMN in the setup request is present in the transmitted E1 Setup Request PDU
TEST_F(e1ap_cu_up_test, when_single_plmn_configured_then_present_in_e1_setup_request)
{
  const std::string      plmn = "00202";
  cu_up_e1_setup_request req  = generate_cu_up_e1_setup_request();
  req.supported_plmns.push_back({plmn, {}, {}, {}});

  bool ret = e1ap->connect_to_cu_cp();
  ocudu_assert(ret, "Failed to connect to CU-CP");
  async_task<cu_up_e1_setup_response>         t = e1ap->handle_cu_up_e1_setup_request(req);
  lazy_task_launcher<cu_up_e1_setup_response> t_launcher(t);
  unsigned                                    transaction_id = get_transaction_id(e1ap_gw.last_tx_e1ap_pdu.pdu).value();
  e1ap_message                                e1_setup_response = generate_cu_up_e1_setup_response(transaction_id);
  e1ap->handle_message(e1_setup_response);

  auto& setup_req = e1ap_gw.last_tx_e1ap_pdu.pdu.init_msg().value.gnb_cu_up_e1_setup_request();
  ASSERT_EQ(setup_req->supported_plmns.size(), 1U);
  EXPECT_EQ(setup_req->supported_plmns[0].plmn_id.to_number(), bcd_helper::plmn_string_to_bcd(plmn));
}

/// Test that multiple PLMNs in the setup request are all present in the transmitted E1 Setup Request PDU
TEST_F(e1ap_cu_up_test, when_multiple_plmns_configured_then_all_present_in_e1_setup_request)
{
  // Build a setup request with two PLMNs.
  cu_up_e1_setup_request request_msg = generate_cu_up_e1_setup_request();
  const std::string      plmn1       = "00101";
  const std::string      plmn2       = "00202";
  request_msg.supported_plmns.push_back({plmn1, {}, {}, {}});
  request_msg.supported_plmns.push_back({plmn2, {}, {}, {}});

  // Connect and send the setup request.
  bool ret = e1ap->connect_to_cu_cp();
  ocudu_assert(ret, "Failed to connect to CU-CP");
  async_task<cu_up_e1_setup_response>         t = e1ap->handle_cu_up_e1_setup_request(request_msg);
  lazy_task_launcher<cu_up_e1_setup_response> t_launcher(t);
  unsigned                                    transaction_id = get_transaction_id(e1ap_gw.last_tx_e1ap_pdu.pdu).value();
  e1ap_message                                e1_setup_response = generate_cu_up_e1_setup_response(transaction_id);
  e1ap->handle_message(e1_setup_response);

  // Inspect the transmitted ASN.1 PDU.
  auto& setup_req = e1ap_gw.last_tx_e1ap_pdu.pdu.init_msg().value.gnb_cu_up_e1_setup_request();
  ASSERT_EQ(setup_req->supported_plmns.size(), 2U);
  EXPECT_EQ(setup_req->supported_plmns[0].plmn_id.to_number(), bcd_helper::plmn_string_to_bcd(plmn1));
  EXPECT_EQ(setup_req->supported_plmns[1].plmn_id.to_number(), bcd_helper::plmn_string_to_bcd(plmn2));
}

//////////////////////////////////////////////////////////////////////////////////////
/* Handling of Bearer Context Messages                                              */
//////////////////////////////////////////////////////////////////////////////////////

TEST_F(e1ap_cu_up_test, when_valid_bearer_context_setup_received_then_bearer_context_setup_response_is_sent)
{
  run_e1_setup_procedure();

  test_logger.info("TEST: Receive BearerContextSetupRequest message...");

  // Receive BearerContextSetupRequest message.
  e1ap_message bearer_context_setup = generate_bearer_context_setup_request(9);
  e1ap->handle_message(bearer_context_setup);

  // Check if BearerContextSetupRequest was forwarded to UE manager.
  ASSERT_EQ(cu_up_notifier.last_bearer_context_setup_request.serving_plmn, "20899"); // this is the human readable PLMN.

  // Check the generated PDU is indeed the Bearer Context Setup response.
  ASSERT_EQ(asn1::e1ap::e1ap_pdu_c::types_opts::options::successful_outcome, e1ap_gw.last_tx_e1ap_pdu.pdu.type());
  ASSERT_EQ(asn1::e1ap::e1ap_elem_procs_o::successful_outcome_c::types_opts::options::bearer_context_setup_resp,
            e1ap_gw.last_tx_e1ap_pdu.pdu.successful_outcome().value.type());

  ASSERT_EQ(e1ap_gw.last_tx_e1ap_pdu.pdu.successful_outcome()
                .value.bearer_context_setup_resp()
                ->sys_bearer_context_setup_resp.ng_ran_bearer_context_setup_resp()
                .pdu_session_res_setup_list[0]
                .drb_setup_list_ng_ran.size(),
            1);
}

TEST_F(e1ap_cu_up_test, when_invalid_bearer_context_setup_received_then_bearer_context_setup_failure_is_sent)
{
  run_e1_setup_procedure();

  test_logger.info("TEST: Receive BearerContextSetupRequest message...");

  // Receive BearerContextSetupRequest message.
  e1ap_message bearer_context_setup = generate_invalid_bearer_context_setup_request(9);
  e1ap->handle_message(bearer_context_setup);

  // Check the generated PDU is indeed the Bearer Context Setup Failure.
  ASSERT_EQ(asn1::e1ap::e1ap_pdu_c::types_opts::options::unsuccessful_outcome, e1ap_gw.last_tx_e1ap_pdu.pdu.type());
  ASSERT_EQ(asn1::e1ap::e1ap_elem_procs_o::unsuccessful_outcome_c::types_opts::options::bearer_context_setup_fail,
            e1ap_gw.last_tx_e1ap_pdu.pdu.unsuccessful_outcome().value.type());
}

TEST_F(e1ap_cu_up_test,
       when_invalid_bearer_context_setup_inactivity_timer_received_then_bearer_context_setup_failure_is_sent)
{
  run_e1_setup_procedure();

  test_logger.info("TEST: Receive BearerContextSetupRequest message...");

  // Receive BearerContextSetupRequest message.
  e1ap_message bearer_context_setup = generate_invalid_bearer_context_setup_request_inactivity_timer(9);
  e1ap->handle_message(bearer_context_setup);

  // Check the generated PDU is indeed the Bearer Context Setup Failure.
  ASSERT_EQ(asn1::e1ap::e1ap_pdu_c::types_opts::options::unsuccessful_outcome, e1ap_gw.last_tx_e1ap_pdu.pdu.type());
  ASSERT_EQ(asn1::e1ap::e1ap_elem_procs_o::unsuccessful_outcome_c::types_opts::options::bearer_context_setup_fail,
            e1ap_gw.last_tx_e1ap_pdu.pdu.unsuccessful_outcome().value.type());
}

TEST_F(e1ap_cu_up_test,
       when_valid_bearer_context_modification_received_then_bearer_context_modification_response_is_sent)
{
  run_e1_setup_procedure();

  // Setup Bearer Context.
  this->setup_bearer(9);

  test_logger.info("TEST: Receive BearerContextModificationRequest message...");

  // Receive BearerContextModificationRequest.
  e1ap_message bearer_context_modification = generate_bearer_context_modification_request(9, 0);
  e1ap->handle_message(bearer_context_modification);

  // Check the generated PDU is indeed the Bearer Context Modification Response.
  ASSERT_EQ(asn1::e1ap::e1ap_pdu_c::types_opts::options::successful_outcome, e1ap_gw.last_tx_e1ap_pdu.pdu.type());
  ASSERT_EQ(asn1::e1ap::e1ap_elem_procs_o::successful_outcome_c::types_opts::options::bearer_context_mod_resp,
            e1ap_gw.last_tx_e1ap_pdu.pdu.successful_outcome().value.type());
}

TEST_F(e1ap_cu_up_test,
       when_invalid_bearer_context_modification_received_then_bearer_context_modification_failure_is_sent)
{
  run_e1_setup_procedure();

  // Setup Bearer Context.
  this->setup_bearer(9);

  test_logger.info("TEST: Receive BearerContextModificationRequest message...");

  // Receive BearerContextModificationRequest message
  e1ap_message bearer_context_modification = generate_invalid_bearer_context_modification_request(9, 0);
  e1ap->handle_message(bearer_context_modification);

  // Check the generated PDU is indeed the Bearer Context Modification Failure
  ASSERT_EQ(asn1::e1ap::e1ap_pdu_c::types_opts::options::unsuccessful_outcome, e1ap_gw.last_tx_e1ap_pdu.pdu.type());
  ASSERT_EQ(asn1::e1ap::e1ap_elem_procs_o::unsuccessful_outcome_c::types_opts::options::bearer_context_mod_fail,
            e1ap_gw.last_tx_e1ap_pdu.pdu.unsuccessful_outcome().value.type());
}

/// Verify that ng_ul_up_tnl_info in a pdu_session_res_to_modify_item is correctly decoded and forwarded to the
/// CU-UP notifier. This covers the E1AP ASN1 deserialization path added for the Xn path switch N3 tunnel update.
TEST_F(e1ap_cu_up_test,
       when_bearer_context_modification_with_ng_ul_up_tnl_info_received_then_info_is_decoded_and_forwarded)
{
  run_e1_setup_procedure();
  this->setup_bearer(9);

  constexpr uint32_t    upf_teid = 0xdeadbeef;
  constexpr const char* upf_addr = "192.168.1.100";

  e1ap_message bearer_context_modification =
      generate_bearer_context_modification_request_with_ng_ul_tnl(9, upf_teid, upf_addr);
  e1ap->handle_message(bearer_context_modification);

  // The modification response must indicate success.
  ASSERT_EQ(asn1::e1ap::e1ap_pdu_c::types_opts::options::successful_outcome, e1ap_gw.last_tx_e1ap_pdu.pdu.type());
  ASSERT_EQ(asn1::e1ap::e1ap_elem_procs_o::successful_outcome_c::types_opts::options::bearer_context_mod_resp,
            e1ap_gw.last_tx_e1ap_pdu.pdu.successful_outcome().value.type());

  // Verify the decoded ng_ul_up_tnl_info was forwarded to the CU-UP notifier.
  const auto& mod_req = cu_up_notifier.last_bearer_context_modification_request;
  ASSERT_TRUE(mod_req.ng_ran_bearer_context_mod_request.has_value());
  const auto& pdu_session_res_to_mod_list =
      mod_req.ng_ran_bearer_context_mod_request.value().pdu_session_res_to_modify_list;
  ASSERT_EQ(pdu_session_res_to_mod_list.size(), 1U);

  const auto& session_item = *pdu_session_res_to_mod_list.begin();
  ASSERT_TRUE(session_item.ng_ul_up_tnl_info.has_value());
  EXPECT_EQ(session_item.ng_ul_up_tnl_info.value().gtp_teid.value(), upf_teid);
  EXPECT_EQ(session_item.ng_ul_up_tnl_info.value().tp_address.to_string(), upf_addr);
}

/// Negative: when ng_ul_up_tnl_info is absent in a pdu_session_res_to_modify_item, the field must not be set in the
/// decoded output (no phantom population of the optional).
TEST_F(e1ap_cu_up_test,
       when_bearer_context_modification_without_ng_ul_up_tnl_info_received_then_field_is_absent_in_decoded_request)
{
  run_e1_setup_procedure();
  this->setup_bearer(9);

  // The standard modification request does NOT set ng_ul_up_tnl_info_present.
  e1ap_message bearer_context_modification = generate_bearer_context_modification_request(9, 0);
  e1ap->handle_message(bearer_context_modification);

  ASSERT_EQ(asn1::e1ap::e1ap_pdu_c::types_opts::options::successful_outcome, e1ap_gw.last_tx_e1ap_pdu.pdu.type());

  const auto& mod_req = cu_up_notifier.last_bearer_context_modification_request;
  ASSERT_TRUE(mod_req.ng_ran_bearer_context_mod_request.has_value());
  const auto& pdu_session_res_to_mod_list =
      mod_req.ng_ran_bearer_context_mod_request.value().pdu_session_res_to_modify_list;
  ASSERT_EQ(pdu_session_res_to_mod_list.size(), 1U);

  const auto& session_item = *pdu_session_res_to_mod_list.begin();
  EXPECT_FALSE(session_item.ng_ul_up_tnl_info.has_value());
}

TEST_F(e1ap_cu_up_test, when_valid_bearer_context_release_command_received_then_bearer_context_release_complete_is_sent)
{
  run_e1_setup_procedure();

  // Setup Bearer Context.
  this->setup_bearer(9);

  test_logger.info("TEST: Receive BearerContextReleaseCommand message...");

  // Receive BearerContextReleaseCommand.
  e1ap_message bearer_context_release_cmd = generate_bearer_context_release_command(9, 0);
  e1ap->handle_message(bearer_context_release_cmd);

  // Check the generated PDU is indeed the Bearer Context Modification Response.
  ASSERT_EQ(asn1::e1ap::e1ap_pdu_c::types_opts::options::successful_outcome, e1ap_gw.last_tx_e1ap_pdu.pdu.type());
  ASSERT_EQ(asn1::e1ap::e1ap_elem_procs_o::successful_outcome_c::types_opts::options::bearer_context_release_complete,
            e1ap_gw.last_tx_e1ap_pdu.pdu.successful_outcome().value.type());
}

TEST_F(e1ap_cu_up_test, when_valid_full_e1_reset_received_then_reset_ack_sent)
{
  run_e1_setup_procedure();

  // Setup Bearer Context.
  this->setup_bearer(9);

  // Receive E1 Reset message.
  e1ap_message e1_reset = generate_e1_reset({});
  e1ap->handle_message(e1_reset);

  // Check the generated PDU is indeed the E1 Reset ACK.
  ASSERT_EQ(asn1::e1ap::e1ap_pdu_c::types_opts::options::successful_outcome, e1ap_gw.last_tx_e1ap_pdu.pdu.type());
  ASSERT_EQ(asn1::e1ap::e1ap_elem_procs_o::successful_outcome_c::types_opts::options::reset_ack,
            e1ap_gw.last_tx_e1ap_pdu.pdu.successful_outcome().value.type());
}

TEST_F(e1ap_cu_up_test, when_valid_part_of_e1_reset_received_then_reset_ack_sent)
{
  run_e1_setup_procedure();

  // Setup Bearer Context.
  this->setup_bearer(9);

  // Receive E1 Reset message.
  e1ap_message e1_reset = generate_e1_reset({{gnb_cu_cp_ue_e1ap_id_t{0}, gnb_cu_up_ue_e1ap_id_t{0}}});
  e1ap->handle_message(e1_reset);

  // Check the generated PDU is indeed the E1 Reset ACK.
  ASSERT_EQ(asn1::e1ap::e1ap_pdu_c::types_opts::options::successful_outcome, e1ap_gw.last_tx_e1ap_pdu.pdu.type());
  ASSERT_EQ(asn1::e1ap::e1ap_elem_procs_o::successful_outcome_c::types_opts::options::reset_ack,
            e1ap_gw.last_tx_e1ap_pdu.pdu.successful_outcome().value.type());
}

TEST_F(e1ap_cu_up_test, when_bearer_context_valid_and_resume_requested_dl_status_notification_sent)
{
  run_e1_setup_procedure();

  // Setup Bearer Context.
  this->setup_bearer(9);

  // Receive E1 Reset message.
  cu_up_ue_index_t ue_index{0};
  e1ap->handle_dl_data_notification_required(ue_index);

  // Check the generated PDU is indeed the DL Data Notification.
  ASSERT_EQ(asn1::e1ap::e1ap_pdu_c::types_opts::options::init_msg, e1ap_gw.last_tx_e1ap_pdu.pdu.type());
  ASSERT_EQ(asn1::e1ap::e1ap_elem_procs_o::init_msg_c::types_opts::options::dl_data_notif,
            e1ap_gw.last_tx_e1ap_pdu.pdu.init_msg().value.type());
}

int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
