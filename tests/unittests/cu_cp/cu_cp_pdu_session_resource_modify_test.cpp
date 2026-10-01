// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "cu_cp_test_environment.h"
#include "tests/test_doubles/e1ap/e1ap_cu_cp_test_messages.h"
#include "tests/test_doubles/e1ap/e1ap_test_message_validators.h"
#include "tests/test_doubles/f1ap/f1ap_test_message_validators.h"
#include "tests/test_doubles/f1ap/f1ap_test_messages.h"
#include "tests/test_doubles/ngap/ngap_test_message_validators.h"
#include "tests/test_doubles/rrc/rrc_test_message_validators.h"
#include "tests/unittests/cu_cp/test_helpers.h"
#include "tests/unittests/ngap/ngap_test_messages.h"
#include "ocudu/adt/format.h"
#include "ocudu/asn1/ngap/ngap_pdu_contents.h"
#include "ocudu/asn1/rrc_nr/dl_dcch_msg.h"
#include "ocudu/cu_cp/cu_cp_command_handler.h"
#include "ocudu/e1ap/common/e1ap_types.h"
#include "ocudu/f1ap/f1ap_message.h"
#include "ocudu/ngap/ngap_message.h"
#include "ocudu/ran/cu_types.h"
#include "ocudu/ran/rb_id.h"
#include <gtest/gtest.h>
#include <optional>
#include <sys/types.h>

using namespace ocudu;
using namespace ocucp;

static constexpr uint8_t MAX_NOF_DRBS_PER_UE = 29;

class cu_cp_pdu_session_resource_modify_test : public cu_cp_test_environment, public ::testing::Test
{
public:
  cu_cp_pdu_session_resource_modify_test() :
    cu_cp_test_environment(cu_cp_test_env_params{8, 8, 8192, MAX_NOF_DRBS_PER_UE})
  {
    // Run NG setup to completion.
    run_ng_setup();

    // Setup DU.
    std::optional<unsigned> ret = connect_new_du();
    EXPECT_TRUE(ret.has_value());
    du_idx = ret.value();
    EXPECT_TRUE(this->run_f1_setup(du_idx));

    // Setup CU-UP.
    ret = connect_new_cu_up();
    EXPECT_TRUE(ret.has_value());
    cu_up_idx = ret.value();
    EXPECT_TRUE(this->run_e1_setup(cu_up_idx));

    security_indication_t security_ind = {integrity_protection_indication_t::required,
                                          confidentiality_protection_indication_t::required};
    // Connect UE 0x4601.
    EXPECT_TRUE(attach_ue(du_idx,
                          cu_up_idx,
                          du_ue_id,
                          crnti,
                          amf_ue_id,
                          cu_up_e1ap_id,
                          psi,
                          drb_id_t::drb1,
                          qfi,
                          test_helpers::pack_ul_dcch_msg(test_helpers::create_rrc_setup_complete()),
                          make_byte_buffer("00070e00cc6fcda5").value(),
                          std::nullopt,
                          true,
                          std::nullopt,
                          security_ind));
    ue_ctx = this->find_ue_context(du_idx, du_ue_id);

    EXPECT_NE(ue_ctx, nullptr);
  }

  unsigned du_idx    = 0;
  unsigned cu_up_idx = 0;

  gnb_du_ue_f1ap_id_t    du_ue_id      = gnb_du_ue_f1ap_id_t::min;
  rnti_t                 crnti         = to_rnti(0x4601);
  amf_ue_id_t            amf_ue_id     = amf_ue_id_t::min;
  gnb_cu_up_ue_e1ap_id_t cu_up_e1ap_id = gnb_cu_up_ue_e1ap_id_t::min;

  const ue_context* ue_ctx = nullptr;

  pdu_session_id_t psi  = uint_to_pdu_session_id(1);
  pdu_session_id_t psi2 = uint_to_pdu_session_id(2);
  qos_flow_id_t    qfi  = uint_to_qos_flow_id(1);
  qos_flow_id_t    qfi2 = uint_to_qos_flow_id(2);

  ngap_message ngap_pdu;
  f1ap_message f1ap_pdu;
  e1ap_message e1ap_pdu;

  [[nodiscard]] bool is_expected_pdu_session_resource_modify_response(
      const std::vector<pdu_session_id_t>& expected_pdu_sessions_to_modify,
      const std::vector<pdu_session_id_t>& expected_pdu_sessions_failed_to_modify)
  {
    // Check failed PDU sessions
    if (expected_pdu_sessions_failed_to_modify.empty() && ngap_pdu.pdu.successful_outcome()
                                                              .value.pdu_session_res_modify_resp()
                                                              ->pdu_session_res_failed_to_modify_list_mod_res_present) {
      return false;
    }

    if (!expected_pdu_sessions_failed_to_modify.empty() &&
        !ngap_pdu.pdu.successful_outcome()
             .value.pdu_session_res_modify_resp()
             ->pdu_session_res_failed_to_modify_list_mod_res_present) {
      return false;
    }

    if (expected_pdu_sessions_failed_to_modify.size() != ngap_pdu.pdu.successful_outcome()
                                                             .value.pdu_session_res_modify_resp()
                                                             ->pdu_session_res_failed_to_modify_list_mod_res.size()) {
      return false;
    }

    for (const auto& asn1_failed_item : ngap_pdu.pdu.successful_outcome()
                                            .value.pdu_session_res_modify_resp()
                                            ->pdu_session_res_failed_to_modify_list_mod_res) {
      if (std::find(expected_pdu_sessions_failed_to_modify.begin(),
                    expected_pdu_sessions_failed_to_modify.end(),
                    uint_to_pdu_session_id(asn1_failed_item.pdu_session_id)) ==
          expected_pdu_sessions_failed_to_modify.end()) {
        return false;
      }
    }

    // Check successful PDU sessions
    if (expected_pdu_sessions_to_modify.empty() && ngap_pdu.pdu.successful_outcome()
                                                       .value.pdu_session_res_modify_resp()
                                                       ->pdu_session_res_modify_list_mod_res_present) {
      return false;
    }

    if (!expected_pdu_sessions_to_modify.empty() && !ngap_pdu.pdu.successful_outcome()
                                                         .value.pdu_session_res_modify_resp()
                                                         ->pdu_session_res_modify_list_mod_res_present) {
      return false;
    }

    if (expected_pdu_sessions_to_modify.size() != ngap_pdu.pdu.successful_outcome()
                                                      .value.pdu_session_res_modify_resp()
                                                      ->pdu_session_res_modify_list_mod_res.size()) {
      return false;
    }

    for (const auto& asn1_modify_item :
         ngap_pdu.pdu.successful_outcome().value.pdu_session_res_modify_resp()->pdu_session_res_modify_list_mod_res) {
      if (std::find(expected_pdu_sessions_to_modify.begin(),
                    expected_pdu_sessions_to_modify.end(),
                    uint_to_pdu_session_id(asn1_modify_item.pdu_session_id)) == expected_pdu_sessions_to_modify.end()) {
        return false;
      }
    }

    return true;
  }

  [[nodiscard]] bool send_pdu_session_modify_request_and_await_pdu_session_modify_response()
  {
    report_fatal_error_if_not(not this->get_amf().try_pop_rx_pdu(ngap_pdu),
                              "there are still NGAP messages to pop from AMF");
    report_fatal_error_if_not(not this->get_du(du_idx).try_pop_dl_pdu(f1ap_pdu),
                              "there are still F1AP DL messages to pop from DU");
    report_fatal_error_if_not(not this->get_cu_up(cu_up_idx).try_pop_rx_pdu(e1ap_pdu),
                              "there are still E1AP messages to pop from CU-UP");

    // Inject PDU Session Resource Modify Request and wait for PDU Session Resource Modify Response
    get_amf().push_tx_pdu(
        generate_valid_pdu_session_resource_modify_request_message(amf_ue_id, ue_ctx->ran_ue_id.value(), psi2, {qfi2}));
    report_fatal_error_if_not(this->wait_for_ngap_tx_pdu(ngap_pdu),
                              "Failed to receive PDU Session Resource Modify Response");
    report_fatal_error_if_not(test_helpers::is_valid_pdu_session_resource_modify_response(ngap_pdu),
                              "Invalid PDU Session Resource Modify Response");
    report_fatal_error_if_not(is_expected_pdu_session_resource_modify_response({}, {psi2}),
                              "Unsuccessful PDU Session Resource Modify Response");
    return true;
  }

  [[nodiscard]] bool send_pdu_session_modify_request_and_await_bearer_context_modification_request(
      pdu_session_id_t                  pdu_session_id      = uint_to_pdu_session_id(1),
      const std::vector<qos_flow_id_t>& flows_to_add_or_mod = {uint_to_qos_flow_id(2)},
      const std::vector<qos_flow_id_t>& flows_to_release    = {})
  {
    report_fatal_error_if_not(not this->get_amf().try_pop_rx_pdu(ngap_pdu),
                              "there are still NGAP messages to pop from AMF");
    report_fatal_error_if_not(not this->get_du(du_idx).try_pop_dl_pdu(f1ap_pdu),
                              "there are still F1AP DL messages to pop from DU");
    report_fatal_error_if_not(not this->get_cu_up(cu_up_idx).try_pop_rx_pdu(e1ap_pdu),
                              "there are still E1AP messages to pop from CU-UP");

    // Inject PDU Session Resource Modify Request and wait for Bearer Context Modification Request
    get_amf().push_tx_pdu(generate_valid_pdu_session_resource_modify_request_message(
        amf_ue_id, ue_ctx->ran_ue_id.value(), pdu_session_id, flows_to_add_or_mod, flows_to_release));
    report_fatal_error_if_not(this->wait_for_e1ap_tx_pdu(cu_up_idx, e1ap_pdu),
                              "Failed to receive Bearer Context Modification Request");
    report_fatal_error_if_not(test_helpers::is_valid_bearer_context_modification_request(e1ap_pdu),
                              "Invalid Bearer Context Modification Request");
    return true;
  }

  [[nodiscard]] bool send_bearer_context_modification_failure_and_await_pdu_session_modify_response()
  {
    // Inject Bearer Context Modification Failure and wait for PDU Session Resource Setup Response
    get_cu_up(cu_up_idx).push_tx_pdu(
        generate_bearer_context_modification_failure(ue_ctx->cu_cp_e1ap_id.value(), ue_ctx->cu_up_e1ap_id.value()));
    report_fatal_error_if_not(this->wait_for_ngap_tx_pdu(ngap_pdu),
                              "Failed to receive PDU Session Resource Modify Response");
    report_fatal_error_if_not(test_helpers::is_valid_pdu_session_resource_modify_response(ngap_pdu),
                              "Invalid PDU Session Resource Modify Response");
    report_fatal_error_if_not(is_expected_pdu_session_resource_modify_response({}, {psi}),
                              "Unsuccessful PDU Session Resource Modify Response");
    return true;
  }

  [[nodiscard]] bool send_bearer_context_modification_response_and_await_ue_context_modification_request(
      pdu_session_id_t pdu_session_id = uint_to_pdu_session_id(1),
      drb_id_t         drb_id         = drb_id_t::drb2)
  {
    // Inject Bearer Context Modification Response and wait for PDU Session Resource Setup Response
    get_cu_up(cu_up_idx).push_tx_pdu(generate_bearer_context_modification_response(
        ue_ctx->cu_cp_e1ap_id.value(), ue_ctx->cu_up_e1ap_id.value(), {}, {{pdu_session_id, drb_id}}));
    report_fatal_error_if_not(this->wait_for_f1ap_tx_pdu(du_idx, f1ap_pdu),
                              "Failed to receive UE Context Modification Request");
    report_fatal_error_if_not(test_helpers::is_valid_ue_context_modification_request(f1ap_pdu),
                              "Invalid UE Context Modification Request");
    return true;
  }

  [[nodiscard]] bool send_ue_context_modification_failure_and_await_pdu_session_modify_response()
  {
    // Inject UE Context Modification Failure and wait for PDU Session Resource Setup Response
    get_du(du_idx).push_ul_pdu(
        test_helpers::generate_ue_context_modification_failure(ue_ctx->cu_ue_id.value(), ue_ctx->du_ue_id.value()));
    report_fatal_error_if_not(this->wait_for_ngap_tx_pdu(ngap_pdu),
                              "Failed to receive PDU Session Resource Modify Response");
    report_fatal_error_if_not(test_helpers::is_valid_pdu_session_resource_modify_response(ngap_pdu),
                              "Invalid PDU Session Resource Modify Response");
    report_fatal_error_if_not(is_expected_pdu_session_resource_modify_response({}, {psi}),
                              "Unsuccessful PDU Session Resource Modify Response");
    return true;
  }

  [[nodiscard]] bool send_ue_context_modification_response_and_await_bearer_context_modification_request(
      const std::vector<drb_id_t>& drbs_setup_mod_list = {drb_id_t::drb2},
      const std::vector<drb_id_t>& drbs_modified_list  = {})
  {
    // Inject UE Context Modification Response and wait for DL RRC Message (containing RRC Reconfiguration)
    get_du(du_idx).push_ul_pdu(test_helpers::generate_ue_context_modification_response(
        ue_ctx->du_ue_id.value(), ue_ctx->cu_ue_id.value(), crnti, drbs_setup_mod_list, drbs_modified_list));
    report_fatal_error_if_not(this->wait_for_e1ap_tx_pdu(cu_up_idx, e1ap_pdu),
                              "Failed to receive Bearer Context Modification Request");
    report_fatal_error_if_not(test_helpers::is_valid_bearer_context_modification_request(e1ap_pdu),
                              "Invalid Bearer Context Modification Request");
    return true;
  }

  [[nodiscard]] expected<byte_buffer> send_bearer_context_modification_response_and_await_rrc_reconfiguration(
      pdu_session_id_t pdu_session_id = uint_to_pdu_session_id(1),
      drb_id_t         drb_to_modify  = drb_id_t::drb2,
      bool             is_drb_release = false)
  {
    // Inject Bearer Context Modification Response and wait for PDU Session Resource Setup Response
    get_cu_up(cu_up_idx).push_tx_pdu(generate_bearer_context_modification_response(
        ue_ctx->cu_cp_e1ap_id.value(), ue_ctx->cu_up_e1ap_id.value(), {}, {{pdu_session_id, drb_to_modify}}));
    report_fatal_error_if_not(this->wait_for_f1ap_tx_pdu(du_idx, f1ap_pdu),
                              "Failed to receive F1AP DL RRC Message (containing RRC Reconfiguration)");
    report_fatal_error_if_not(test_helpers::is_valid_dl_rrc_message_transfer(f1ap_pdu),
                              "Invalid DL RRC Message Transfer");

    const byte_buffer& rrc_container = test_helpers::get_rrc_container(f1ap_pdu);
    byte_buffer        rrc_msg       = test_helpers::extract_dl_dcch_msg(rrc_container);

    if (is_drb_release) {
      report_fatal_error_if_not(
          test_helpers::is_valid_rrc_reconfiguration(
              rrc_msg, false, std::vector<srb_id_t>{}, std::vector<drb_id_t>{}, std::vector<drb_id_t>{drb_to_modify}),
          "Invalid RRC Reconfiguration");
    } else {
      report_fatal_error_if_not(test_helpers::is_valid_rrc_reconfiguration(
                                    rrc_msg, false, std::vector<srb_id_t>{}, std::vector<drb_id_t>{drb_to_modify}),
                                "Invalid RRC Reconfiguration");
    }
    return rrc_msg;
  }

  [[nodiscard]] bool timeout_rrc_reconfiguration_and_await_pdu_session_modify_response()
  {
    // Fail RRC Reconfiguration (UE doesn't respond) and wait for PDU Session Resource Setup Response
    if (tick_until(
            rrc_test_timer_values.t310 + rrc_test_timer_values.t311 +
                this->get_cu_cp_cfg().rrc.rrc_procedure_guard_time_ms,
            [&]() { return false; },
            false)) {
      return false;
    }
    report_fatal_error_if_not(this->wait_for_ngap_tx_pdu(ngap_pdu),
                              "Failed to receive PDU Session Resource Modify Response");
    report_fatal_error_if_not(test_helpers::is_valid_pdu_session_resource_modify_response(ngap_pdu),
                              "Invalid PDU Session Resource Modify Response");
    report_fatal_error_if_not(is_expected_pdu_session_resource_modify_response({}, {psi}),
                              "Unsuccessful PDU Session Resource Modify Response");
    return true;
  }

  bool send_rrc_reconfiguration_complete_and_await_pdu_session_modify_response(
      byte_buffer rrc_reconfiguration_complete = make_byte_buffer("00080800e6847bbd").value())
  {
    // Inject UL RRC Message (containing RRC Reconfiguration Complete) and wait for PDU Session Resource Modify Response
    get_du(du_idx).push_ul_pdu(test_helpers::generate_ul_rrc_message_transfer(
        du_ue_id, ue_ctx->cu_ue_id.value(), srb_id_t::srb1, std::move(rrc_reconfiguration_complete)));
    report_fatal_error_if_not(this->wait_for_ngap_tx_pdu(ngap_pdu),
                              "Failed to receive PDU Session Resource Modify Response");
    report_fatal_error_if_not(test_helpers::is_valid_pdu_session_resource_modify_response(ngap_pdu),
                              "Invalid PDU Session Resource Modify Response");
    report_fatal_error_if_not(is_expected_pdu_session_resource_modify_response({psi}, {}),
                              "Unsuccessful PDU Session Resource Modify Response");

    return true;
  }

  [[nodiscard]] bool modify_pdu_session_and_add_qos_flow(
      pdu_session_id_t pdu_session_id,
      drb_id_t         drb_id,
      qos_flow_id_t    qos_flow_id,
      byte_buffer      rrc_reconfiguration_complete = make_byte_buffer("00080800e6847bbd").value())
  {
    report_fatal_error_if_not(not this->get_amf().try_pop_rx_pdu(ngap_pdu),
                              "there are still NGAP messages to pop from AMF");
    report_fatal_error_if_not(not this->get_du(du_idx).try_pop_dl_pdu(f1ap_pdu),
                              "there are still F1AP DL messages to pop from DU");
    report_fatal_error_if_not(not this->get_cu_up(cu_up_idx).try_pop_rx_pdu(e1ap_pdu),
                              "there are still E1AP messages to pop from CU-UP");

    // Inject PDU Session Resource Modify Request and await Bearer Context Modification Request
    if (!send_pdu_session_modify_request_and_await_bearer_context_modification_request(
            pdu_session_id, {qos_flow_id}, {})) {
      return false;
    }

    // Inject Bearer Context Modification Response and await UE Context Modification Request
    if (!send_bearer_context_modification_response_and_await_ue_context_modification_request(pdu_session_id, drb_id)) {
      return false;
    }

    // Inject UE Context Modification Response and await Bearer Context Modification Request
    if (!send_ue_context_modification_response_and_await_bearer_context_modification_request()) {
      return false;
    }

    // Inject Bearer Context Modification Response and await RRC Reconfiguration
    if (!send_bearer_context_modification_response_and_await_rrc_reconfiguration(pdu_session_id, drb_id)) {
      return false;
    }

    // Inject RRC Reconfiguration Complete and await successful PDU Session Resource Modify Response
    if (!send_rrc_reconfiguration_complete_and_await_pdu_session_modify_response(
            std::move(rrc_reconfiguration_complete))) {
      return false;
    }
    return true;
  }
};

TEST_F(cu_cp_pdu_session_resource_modify_test,
       when_modify_request_with_inactive_pdu_session_arrives_then_modification_fails)
{
  // Inject PDU Session Resource Modify Request and await PDU Session Resource Modify Response
  ASSERT_TRUE(send_pdu_session_modify_request_and_await_pdu_session_modify_response());
}

TEST_F(cu_cp_pdu_session_resource_modify_test, when_bearer_ctxt_modification_fails_then_pdu_session_modify_fails)
{
  // Inject PDU Session Resource Modify Request and await Bearer Context Modification Request
  ASSERT_TRUE(send_pdu_session_modify_request_and_await_bearer_context_modification_request());

  // Inject Bearer Context Modification Failure and await PDU Session Resource Modify Response
  ASSERT_TRUE(send_bearer_context_modification_failure_and_await_pdu_session_modify_response());
}

TEST_F(cu_cp_pdu_session_resource_modify_test, when_ue_ctxt_modification_fails_then_pdu_session_modify_fails)
{
  // Inject PDU Session Resource Modify Request and await Bearer Context Modification Request
  ASSERT_TRUE(send_pdu_session_modify_request_and_await_bearer_context_modification_request());

  // Inject Bearer Context Modification Response and await UE Context Modification Request
  ASSERT_TRUE(send_bearer_context_modification_response_and_await_ue_context_modification_request());

  // Inject UE Context Modification Failure and await PDU Session Resource Modify Response
  ASSERT_TRUE(send_ue_context_modification_failure_and_await_pdu_session_modify_response());
}

TEST_F(cu_cp_pdu_session_resource_modify_test, when_second_bearer_ctxt_modification_fails_then_pdu_session_modify_fails)
{
  // Inject PDU Session Resource Modify Request and await Bearer Context Modification Request
  ASSERT_TRUE(send_pdu_session_modify_request_and_await_bearer_context_modification_request());

  // Inject Bearer Context Modification Response and await UE Context Modification Request
  ASSERT_TRUE(send_bearer_context_modification_response_and_await_ue_context_modification_request());

  // Inject UE Context Modification Response and await Bearer Context Modification Request
  ASSERT_TRUE(send_ue_context_modification_response_and_await_bearer_context_modification_request());

  // Inject Bearer Context Modification Failure and await PDU Session Resource Modify Response
  ASSERT_TRUE(send_bearer_context_modification_failure_and_await_pdu_session_modify_response());
}

TEST_F(cu_cp_pdu_session_resource_modify_test, when_rrc_reconfiguration_fails_then_pdu_session_modify_fails)
{
  // Inject PDU Session Resource Modify Request and await Bearer Context Modification Request
  ASSERT_TRUE(send_pdu_session_modify_request_and_await_bearer_context_modification_request());

  // Inject Bearer Context Modification Response and await UE Context Modification Request
  ASSERT_TRUE(send_bearer_context_modification_response_and_await_ue_context_modification_request());

  // Inject UE Context Modification Response and await Bearer Context Modification Request
  ASSERT_TRUE(send_ue_context_modification_response_and_await_bearer_context_modification_request());

  // Inject Bearer Context Modification Response and await RRC Reconfiguration
  ASSERT_TRUE(send_bearer_context_modification_response_and_await_rrc_reconfiguration());

  // Let the RRC Reconfiguration timeout and await PDU Session Resource Modify Response
  ASSERT_TRUE(timeout_rrc_reconfiguration_and_await_pdu_session_modify_response());
}

TEST_F(cu_cp_pdu_session_resource_modify_test, when_rrc_reconfiguration_succeeds_then_pdu_session_modify_succeeds)
{
  // Inject PDU Session Resource Modify Request and await Bearer Context Modification Request
  ASSERT_TRUE(send_pdu_session_modify_request_and_await_bearer_context_modification_request());

  // Inject Bearer Context Modification Response and await UE Context Modification Request
  ASSERT_TRUE(send_bearer_context_modification_response_and_await_ue_context_modification_request());

  // Inject UE Context Modification Response and await Bearer Context Modification Request
  ASSERT_TRUE(send_ue_context_modification_response_and_await_bearer_context_modification_request());

  // Inject Bearer Context Modification Response and await RRC Reconfiguration
  ASSERT_TRUE(send_bearer_context_modification_response_and_await_rrc_reconfiguration());

  // Inject RRC Reconfiguration Complete and await successful PDU Session Resource Modify Response
  ASSERT_TRUE(send_rrc_reconfiguration_complete_and_await_pdu_session_modify_response());
}

TEST_F(cu_cp_pdu_session_resource_modify_test,
       when_valid_modification_arrives_and_qos_flow_can_be_removed_then_pdu_session_modification_succeeds)
{
  // Run PDU session modification and add second QoS flow
  ASSERT_TRUE(modify_pdu_session_and_add_qos_flow(psi, drb_id_t::drb2, qfi2));

  // Inject PDU Session Resource Modify Request and await Bearer Context Modification Request
  ASSERT_TRUE(send_pdu_session_modify_request_and_await_bearer_context_modification_request(psi, {}, {qfi2}));

  // Inject Bearer Context Modification Response and await UE Context Modification Request
  ASSERT_TRUE(send_bearer_context_modification_response_and_await_ue_context_modification_request());

  // Inject UE Context Modification Response and await Bearer Context Modification Request
  ASSERT_TRUE(
      send_ue_context_modification_response_and_await_bearer_context_modification_request({}, {drb_id_t::drb2}));

  // Inject Bearer Context Modification Response and await RRC Reconfiguration
  ASSERT_TRUE(send_bearer_context_modification_response_and_await_rrc_reconfiguration(psi, drb_id_t::drb2, true));

  // Inject RRC Reconfiguration Complete and await successful PDU Session Resource Modify Response
  ASSERT_TRUE(send_rrc_reconfiguration_complete_and_await_pdu_session_modify_response(
      generate_rrc_reconfiguration_complete_pdu(1, 9)));
}

TEST_F(cu_cp_pdu_session_resource_modify_test, when_many_qos_flows_are_added_pdu_session_modification_succeeds)
{
  // Add QoS flows until maximum number of DRBs is reached
  unsigned transaction_id = 0;
  unsigned count          = 8;
  for (unsigned i = 2; i <= MAX_NOF_DRBS_PER_UE; ++i) {
    ASSERT_TRUE(modify_pdu_session_and_add_qos_flow(psi,
                                                    uint_to_drb_id(i),
                                                    uint_to_qos_flow_id(i),
                                                    generate_rrc_reconfiguration_complete_pdu(transaction_id, count)));
    count++;
    transaction_id++;
    if (transaction_id == 4) {
      transaction_id = 0;
    }
  }
}

TEST_F(cu_cp_pdu_session_resource_modify_test, when_one_to_many_qos_flows_are_added_last_pdu_session_modification_fails)
{
  // Add QoS flows until maximum number of DRBs is reached
  unsigned transaction_id = 0;
  unsigned count          = 8;
  for (unsigned i = 2; i <= MAX_NOF_DRBS_PER_UE; ++i) {
    ASSERT_TRUE(modify_pdu_session_and_add_qos_flow(psi,
                                                    uint_to_drb_id(i),
                                                    uint_to_qos_flow_id(i),
                                                    generate_rrc_reconfiguration_complete_pdu(transaction_id, count)));
    count++;
    transaction_id++;
    if (transaction_id == 4) {
      transaction_id = 0;
    }
  }

  // Try to add one more QoS flow and expect failure
  // Inject PDU Session Resource Modify Request and wait for PDU Session Resource Modify Response
  get_amf().push_tx_pdu(
      generate_valid_pdu_session_resource_modify_request_message(amf_ue_id, ue_ctx->ran_ue_id.value(), psi2, {qfi2}));
  ASSERT_TRUE(this->wait_for_ngap_tx_pdu(ngap_pdu));
  ASSERT_TRUE(test_helpers::is_valid_pdu_session_resource_modify_response(ngap_pdu));
  ASSERT_TRUE(is_expected_pdu_session_resource_modify_response({}, {psi2}));
}

TEST_F(cu_cp_pdu_session_resource_modify_test, when_valid_modification_is_received_twice_then_second_modification_fails)
{
  // Run PDU session modification and add second QoS flow
  ASSERT_TRUE(modify_pdu_session_and_add_qos_flow(psi, drb_id_t::drb2, qfi2));

  // Run PDU session modification again and try to add same QoS flow
  // Inject PDU Session Resource Modify Request and wait for PDU Session Resource Modify Response
  get_amf().push_tx_pdu(
      generate_valid_pdu_session_resource_modify_request_message(amf_ue_id, ue_ctx->ran_ue_id.value(), psi2, {qfi2}));
  ASSERT_TRUE(this->wait_for_ngap_tx_pdu(ngap_pdu));
  ASSERT_TRUE(test_helpers::is_valid_pdu_session_resource_modify_response(ngap_pdu));
  ASSERT_TRUE(is_expected_pdu_session_resource_modify_response({}, {psi2}));
}

TEST_F(cu_cp_pdu_session_resource_modify_test,
       when_valid_modification_arrives_and_qos_flow_can_be_modified_then_pdu_session_modification_succeeds)
{
  // Inject PDU Session Resource Modify Request and await Bearer Context Modification Request
  ASSERT_TRUE(send_pdu_session_modify_request_and_await_bearer_context_modification_request(psi, {qfi}, {}));

  // Inject Bearer Context Modification Response and await UE Context Modification Request
  ASSERT_TRUE(send_bearer_context_modification_response_and_await_ue_context_modification_request());

  // Inject UE Context Modification Response and await Bearer Context Modification Request
  ASSERT_TRUE(
      send_ue_context_modification_response_and_await_bearer_context_modification_request({}, {drb_id_t::drb1}));

  // Inject Bearer Context Modification Response and await RRC Reconfiguration
  ASSERT_TRUE(send_bearer_context_modification_response_and_await_rrc_reconfiguration(psi, drb_id_t::drb1, false));

  // Inject RRC Reconfiguration Complete and await successful PDU Session Resource Modify Response
  ASSERT_TRUE(send_rrc_reconfiguration_complete_and_await_pdu_session_modify_response());
}

TEST_F(cu_cp_pdu_session_resource_modify_test,
       when_new_drb_is_created_pdcp_config_keeps_integrity_and_confidentiality_settings)
{
  // Add QoS flows until maximum number of DRBs is reached
  drb_id_t      drb_id{2};
  qos_flow_id_t qos_flow_id{2};
  unsigned      count          = 8;
  unsigned      transaction_id = 0;

  // Inject PDU Session Resource Modify Request and await Bearer Context Modification Request
  ASSERT_TRUE(send_pdu_session_modify_request_and_await_bearer_context_modification_request(psi, {qos_flow_id}, {}));

  // Inject Bearer Context Modification Response and await UE Context Modification Request
  ASSERT_TRUE(send_bearer_context_modification_response_and_await_ue_context_modification_request(psi, drb_id));

  // Inject UE Context Modification Response and await Bearer Context Modification Request
  ASSERT_TRUE(send_ue_context_modification_response_and_await_bearer_context_modification_request());

  // Inject Bearer Context Modification Response and await RRC Reconfiguration
  expected<byte_buffer> rrc_reconf =
      send_bearer_context_modification_response_and_await_rrc_reconfiguration(psi, drb_id);
  ASSERT_TRUE(rrc_reconf);
  ASSERT_TRUE(test_helpers::is_valid_pdcp_security(rrc_reconf.value(), drb_id, true, true));

  // Inject RRC Reconfiguration Complete and await successful PDU Session Resource Modify Response
  ASSERT_TRUE(send_rrc_reconfiguration_complete_and_await_pdu_session_modify_response(
      generate_rrc_reconfiguration_complete_pdu(transaction_id, count)));
}

TEST_F(cu_cp_pdu_session_resource_modify_test,
       when_mobility_config_shrinks_then_modification_reconfiguration_carries_removals)
{
  cu_cp_mobility_config_handler& mob_cfg = get_cu_cp().get_command_handler().get_mobility_config_handler();

  const nr_cell_identity serving_nci = nr_cell_identity::create(gnb_id_t{411, 22}, 0U).value();
  const nr_cell_identity ext_nci     = nr_cell_identity::create(gnb_id_t{0x100, 22}, 0U).value();

  // Add an external neighbor cell and a relation from the serving cell at runtime, referencing the A3
  // report config the test environment configures under id 2.
  serving_cell_meas_config ext_cell;
  ext_cell.gnb_id_bit_length   = 22;
  ext_cell.nci                 = ext_nci;
  ext_cell.pci                 = 3;
  ext_cell.band.emplace()      = nr_band::n78;
  ext_cell.ssb_arfcn.emplace() = 632128;
  ext_cell.ssb_scs.emplace()   = subcarrier_spacing::kHz30;
  {
    rrc_ssb_mtc ssb_mtc;
    ssb_mtc.dur                                = 1;
    ssb_mtc.periodicity_and_offset.periodicity = rrc_periodicity_and_offset::periodicity_t::sf5;
    ssb_mtc.periodicity_and_offset.offset      = 0;
    ext_cell.ssb_mtc.emplace()                 = ssb_mtc;
  }
  ASSERT_TRUE(mob_cfg.update_mobility_cell(ext_cell));
  ASSERT_TRUE(mob_cfg.update_neighbor(serving_nci, ext_nci, {uint_to_report_cfg_id(2)}));

  // First modification: the RRC Reconfiguration carries the measConfig additions for the new neighbor and
  // becomes the UE's stored measurement config.
  ASSERT_TRUE(
      send_pdu_session_modify_request_and_await_bearer_context_modification_request(psi, {uint_to_qos_flow_id(2)}, {}));
  ASSERT_TRUE(send_bearer_context_modification_response_and_await_ue_context_modification_request(psi, drb_id_t::drb2));
  ASSERT_TRUE(send_ue_context_modification_response_and_await_bearer_context_modification_request());
  expected<byte_buffer> rrc_msg =
      send_bearer_context_modification_response_and_await_rrc_reconfiguration(psi, drb_id_t::drb2);
  ASSERT_TRUE(rrc_msg.has_value());
  {
    asn1::cbit_ref              bref{rrc_msg.value()};
    asn1::rrc_nr::dl_dcch_msg_s dcch;
    ASSERT_EQ(dcch.unpack(bref), asn1::OCUDUASN_SUCCESS);
    const auto& reconfig_ies = dcch.msg.c1().rrc_recfg().crit_exts.rrc_recfg();
    ASSERT_TRUE(reconfig_ies.meas_cfg_present);
    ASSERT_GT(reconfig_ies.meas_cfg.meas_obj_to_add_mod_list.size(), 0U);
  }
  ASSERT_TRUE(send_rrc_reconfiguration_complete_and_await_pdu_session_modify_response(
      generate_rrc_reconfiguration_complete_pdu(0, 8)));

  // Remove every relation of the serving cell (the one added above plus the two the test environment
  // configures) and its periodic report: nothing is left to measure.
  ASSERT_TRUE(mob_cfg.remove_neighbor(serving_nci, ext_nci));
  ASSERT_TRUE(mob_cfg.remove_neighbor(serving_nci, nr_cell_identity::create(gnb_id_t{411, 22}, 1U).value()));
  ASSERT_TRUE(mob_cfg.remove_neighbor(serving_nci, nr_cell_identity::create(gnb_id_t{412, 22}, 0U).value()));
  ASSERT_TRUE(mob_cfg.set_periodic_report(serving_nci, std::nullopt));

  // Second modification (new QoS flow on a new DRB): the RRC Reconfiguration carries only the removals of
  // the previous measurement config.
  ASSERT_TRUE(
      send_pdu_session_modify_request_and_await_bearer_context_modification_request(psi, {uint_to_qos_flow_id(3)}, {}));
  ASSERT_TRUE(send_bearer_context_modification_response_and_await_ue_context_modification_request(psi, drb_id_t::drb3));
  ASSERT_TRUE(send_ue_context_modification_response_and_await_bearer_context_modification_request({drb_id_t::drb3}));
  rrc_msg = send_bearer_context_modification_response_and_await_rrc_reconfiguration(psi, drb_id_t::drb3);
  ASSERT_TRUE(rrc_msg.has_value());
  {
    asn1::cbit_ref              bref{rrc_msg.value()};
    asn1::rrc_nr::dl_dcch_msg_s dcch;
    ASSERT_EQ(dcch.unpack(bref), asn1::OCUDUASN_SUCCESS);
    const auto& reconfig_ies = dcch.msg.c1().rrc_recfg().crit_exts.rrc_recfg();
    ASSERT_TRUE(reconfig_ies.meas_cfg_present);
    EXPECT_EQ(reconfig_ies.meas_cfg.meas_obj_to_add_mod_list.size(), 0U);
    EXPECT_GT(reconfig_ies.meas_cfg.meas_obj_to_rem_list.size(), 0U);
    EXPECT_GT(reconfig_ies.meas_cfg.meas_id_to_rem_list.size(), 0U);
  }
  ASSERT_TRUE(send_rrc_reconfiguration_complete_and_await_pdu_session_modify_response(
      generate_rrc_reconfiguration_complete_pdu(1, 9)));
}

TEST_F(cu_cp_pdu_session_resource_modify_test,
       when_invalid_mobility_config_commands_are_dispatched_then_they_are_rejected)
{
  cu_cp_mobility_config_handler& mob_cfg = get_cu_cp().get_command_handler().get_mobility_config_handler();

  const nr_cell_identity serving_nci = nr_cell_identity::create(gnb_id_t{411, 22}, 0U).value();
  const nr_cell_identity unknown_nci = nr_cell_identity::create(gnb_id_t{411, 22}, 9U).value();

  // Refusals cross the dispatch boundary as synchronous false results.
  EXPECT_FALSE(mob_cfg.update_neighbor(unknown_nci, serving_nci, {uint_to_report_cfg_id(2)}));
  EXPECT_FALSE(mob_cfg.remove_neighbor(serving_nci, unknown_nci));
  EXPECT_FALSE(mob_cfg.remove_mobility_cell(unknown_nci));
  // Report config 2 is referenced by the test environment's neighbor relations.
  EXPECT_FALSE(mob_cfg.remove_report_config(uint_to_report_cfg_id(2)));
  // A serving-cell periodic report must reference a periodical config; id 2 is event-triggered.
  EXPECT_FALSE(mob_cfg.set_periodic_report(serving_nci, uint_to_report_cfg_id(2)));
}
