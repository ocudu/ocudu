// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "cu_cp_test_environment.h"
#include "tests/test_doubles/e1ap/e1ap_test_message_validators.h"
#include "tests/test_doubles/f1ap/f1ap_test_message_validators.h"
#include "tests/test_doubles/ngap/ngap_test_message_validators.h"
#include "tests/test_doubles/rrc/rrc_test_message_validators.h"
#include "tests/test_doubles/utils/test_rng.h"
#include "tests/test_doubles/xnap/xnap_test_message_validators.h"
#include "tests/unittests/e1ap/common/e1ap_cu_cp_test_messages.h"
#include "tests/unittests/ngap/ngap_test_messages.h"
#include "tests/unittests/xnap/xnap_test_messages.h"
#include "ocudu/asn1/f1ap/f1ap_pdu_contents_ue.h"
#include "ocudu/e1ap/common/e1ap_types.h"
#include "ocudu/f1ap/f1ap_message.h"
#include "ocudu/f1ap/f1ap_ue_id_types.h"
#include "ocudu/ngap/ngap_message.h"
#include "ocudu/ngap/ngap_types.h"
#include "ocudu/ran/cu_types.h"
#include "ocudu/xnap/xnap_types.h"
#include <gtest/gtest.h>
#include <unistd.h>

using namespace ocudu;
using namespace ocucp;

class cu_cp_inter_cu_xn_handover_test : public cu_cp_test_environment, public ::testing::Test
{
public:
  cu_cp_inter_cu_xn_handover_test() :
    cu_cp_test_environment({/* max nof cu-ups */ 8,
                            /* max nof dus */ 8,
                            /* max nof ues */ 8192,
                            /* max nof drbs per ue */ 8,
                            /* amf config */ {{default_supported_tracking_area}},
                            /* trigger ho from measurements */ true,
                            /* enable rrc inactive */ false,
                            /* enable xnc peer */ true})
  {
    // Run NG setup to completion.
    run_ng_setup();

    // Wait for the XN-C gateway to be attached to the CU-CP.
    sleep(1);

    // Run XN setup to completion.
    run_xn_setup();

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
  }

  [[nodiscard]] bool attach_ue()
  {
    if (!cu_cp_test_environment::attach_ue(
            du_idx, cu_up_idx, du_ue_id, crnti, amf_ue_id, cu_up_e1ap_id, psi, drb_id_t::drb1, qfi)) {
      return false;
    }
    ue_ctx = this->find_ue_context(du_idx, du_ue_id);

    return ue_ctx != nullptr;
  }

  [[nodiscard]] bool send_handover_request_and_await_bearer_context_setup_request(local_xnap_ue_id_t local_xnap_ue_id)
  {
    report_fatal_error_if_not(not this->get_amf().try_pop_rx_pdu(ngap_pdu),
                              "there are still NGAP messages to pop from AMF");
    report_fatal_error_if_not(not this->get_du(du_idx).try_pop_dl_pdu(f1ap_pdu),
                              "there are still F1AP DL messages to pop from DU");
    report_fatal_error_if_not(not this->get_cu_up(cu_up_idx).try_pop_rx_pdu(e1ap_pdu),
                              "there are still E1AP messages to pop from CU-UP");
    report_fatal_error_if_not(not this->get_xnc_cu_cp(xnc_peer_idx).try_pop_rx_pdu(xnap_pdu),
                              "there are still XNAP messages to pop from XN-C peer CU-CP");

    // Inject Handover Request and wait for Bearer Context Setup Request.
    get_xnc_cu_cp(xnc_peer_idx).push_tx_pdu(generate_handover_request(local_xnap_ue_id));
    report_fatal_error_if_not(this->wait_for_e1ap_tx_pdu(cu_up_idx, e1ap_pdu),
                              "Failed to receive Bearer Context Setup Request");
    report_fatal_error_if_not(test_helpers::is_valid_bearer_context_setup_request(e1ap_pdu),
                              "Invalid Bearer Context Setup Request");

    cu_cp_e1ap_id =
        int_to_gnb_cu_cp_ue_e1ap_id(e1ap_pdu.pdu.init_msg().value.bearer_context_setup_request()->gnb_cu_cp_ue_e1ap_id);
    return true;
  }

  [[nodiscard]] bool send_bearer_context_setup_response_and_await_ue_context_setup_request()
  {
    // Inject Bearer Context Setup Response and wait for UE Context Setup Request.
    get_cu_up(cu_up_idx).push_tx_pdu(generate_bearer_context_setup_response(cu_cp_e1ap_id, cu_up_e1ap_id));
    report_fatal_error_if_not(this->wait_for_f1ap_tx_pdu(du_idx, f1ap_pdu),
                              "Failed to receive UE Context Setup Request");
    report_fatal_error_if_not(test_helpers::is_valid_ue_context_setup_request(f1ap_pdu),
                              "Invalid UE Context Setup Request");

    report_fatal_error_if_not(f1ap_pdu.pdu.init_msg().value.ue_context_setup_request()->serving_cell_mo_present,
                              "ServingCellMO not present in UE Context Setup Request");

    ue_context_setup_req_serving_cell_mo = f1ap_pdu.pdu.init_msg().value.ue_context_setup_request()->serving_cell_mo;

    cu_ue_id = int_to_gnb_cu_ue_f1ap_id(f1ap_pdu.pdu.init_msg().value.ue_context_setup_request()->gnb_cu_ue_f1ap_id);
    return true;
  }

  [[nodiscard]] bool send_ue_context_setup_response_and_await_bearer_context_modification_request()
  {
    // Inject UE Context Setup Response and wait for Bearer Context Modification Request.
    get_du(du_idx).push_ul_pdu(test_helpers::generate_ue_context_setup_response(
        cu_ue_id,
        du_ue_id,
        crnti,
        make_byte_buffer(
            "5c04c00604b0701f00811a0f020001273b8c02692f30004d25e24040008c8040a26418d6d8d76006e08040000101000083446a48d8"
            "02692f1200000464e35b63224f8060664abff0124e9106e28dc61b8e372c6fbf56c70eb00442c0680182c4601c020521004930b2a0"
            "003fe00000000060dc2108000780594008300000020698101450a000e3890000246aac90838002081840a1839389142c60d1c3c811"
            "00000850000800b50001000850101800b50102000850202800b50203000850303800b503040c885010480504014014120580505018"
            "01416068050601c0141a0780507020314100880905204963028711d159e26f2681d2083c5df81821c00000038ffd294a5294f28160"
            "00021976000000000000000000108ad5450047001800082000e21009c400e0202108001c420138401c080441000388402708038180"
            "842000710804e18070401104000e21009c300080000008218081018201c1a0001c71000000080100020180020240088029800008c4"
            "0089c7001800")
            .value()));

    report_fatal_error_if_not(this->wait_for_e1ap_tx_pdu(cu_up_idx, e1ap_pdu),
                              "Failed to receive Bearer Context Modification Request");
    report_fatal_error_if_not(test_helpers::is_valid_bearer_context_modification_request(e1ap_pdu),
                              "Invalid Bearer Context Modification Request");
    return true;
  }

  [[nodiscard]] bool send_bearer_context_modification_response_and_await_handover_request_ack()
  {
    // Inject Bearer Context Modification Response and wait for Handover Request Ack.
    get_cu_up(cu_up_idx).push_tx_pdu(generate_bearer_context_modification_response(cu_cp_e1ap_id, cu_up_e1ap_id));
    report_fatal_error_if_not(this->wait_for_xnap_tx_pdu(xnc_peer_idx, xnap_pdu),
                              "Failed to receive Handover Request Ack");
    report_fatal_error_if_not(test_helpers::is_valid_handover_request_ack(xnap_pdu), "Invalid Handover Request Ack");
    return true;
  }

  [[nodiscard]] bool send_bearer_context_modification_response()
  {
    // Inject Bearer Context Modification Response.
    get_cu_up(cu_up_idx).push_tx_pdu(generate_bearer_context_modification_response(cu_cp_e1ap_id, cu_up_e1ap_id));
    return true;
  }

  [[nodiscard]] bool
  send_sn_status_transfer_and_await_bearer_context_modification_request(local_xnap_ue_id_t local_xnap_ue_id,
                                                                        peer_xnap_ue_id_t  peer_xnap_ue_id)
  {
    get_xnc_cu_cp(xnc_peer_idx).push_tx_pdu(generate_sn_status_transfer(local_xnap_ue_id, peer_xnap_ue_id));
    report_fatal_error_if_not(this->wait_for_e1ap_tx_pdu(cu_up_idx, e1ap_pdu),
                              "Failed to receive Bearer Context Modification Request");
    report_fatal_error_if_not(test_helpers::is_valid_bearer_context_modification_request(e1ap_pdu),
                              "Invalid Bearer Context Modification Request");
    return true;
  }

  [[nodiscard]] bool send_rrc_reconfiguration_complete_and_await_path_switch_request()
  {
    // Inject UL RRC Message (containing RRC Reconfiguration Complete) and wait for Path Switch Request.
    get_du(du_idx).push_ul_pdu(test_helpers::generate_ul_rrc_message_transfer(
        du_ue_id, cu_ue_id, srb_id_t::srb1, make_byte_buffer("80000800795ae600").value()));
    report_fatal_error_if_not(this->wait_for_ngap_tx_pdu(ngap_pdu), "Failed to receive Path Switch Request");
    report_fatal_error_if_not(test_helpers::is_valid_path_switch_request(ngap_pdu), "Invalid Path Switch Request");

    return true;
  }

  [[nodiscard]] bool send_path_switch_request_ack_and_await_ue_context_modification_request()
  {
    // Inject Path Switch Request Ack and await UE Context Modification Request.
    get_amf().push_tx_pdu(generate_path_switch_request_ack(uint_to_amf_ue_id(1), ran_ue_id_t::min));
    report_fatal_error_if_not(this->wait_for_f1ap_tx_pdu(du_idx, f1ap_pdu),
                              "Failed to receive F1AP UE Context Modification Request (containing RRC Release)");
    report_fatal_error_if_not(test_helpers::is_valid_ue_context_modification_request(f1ap_pdu),
                              "Invalid UE Context Modification Request");
    return true;
  }

  [[nodiscard]] bool send_path_switch_request_ack_with_ul_tunnel_and_await_bearer_context_modification_request()
  {
    // Inject Path Switch Request Ack carrying a new UL tunnel endpoint and await Bearer Context Modification Request.
    get_amf().push_tx_pdu(
        generate_path_switch_request_ack_with_ul_tunnel(uint_to_amf_ue_id(1),
                                                        ran_ue_id_t::min,
                                                        psi,
                                                        transport_layer_address::create_from_string("127.0.1.1"),
                                                        int_to_gtpu_teid(0x00000001)));
    report_fatal_error_if_not(this->wait_for_e1ap_tx_pdu(cu_up_idx, e1ap_pdu),
                              "Failed to receive E1AP Bearer Context Modification Request (tunnel update)");
    report_fatal_error_if_not(test_helpers::is_valid_bearer_context_modification_request(e1ap_pdu),
                              "Invalid Bearer Context Modification Request");
    return true;
  }

  [[nodiscard]] bool send_rrc_measurement_report_and_await_handover_request(
      gnb_cu_ue_f1ap_id_t f1ap_cu_ue_id,
      gnb_du_ue_f1ap_id_t f1ap_du_ue_id,
      local_xnap_ue_id_t& local_xnap_ue_id,
      peer_xnap_ue_id_t&  peer_xnap_ue_id,
      byte_buffer         rrc_meas_report = make_byte_buffer("000800420004015f741fe0808bf183fc0789117e").value())
  {
    // Inject UL RRC Message (containing RRC Measurement Report) and wait for Handover Required.
    get_du(du_idx).push_ul_pdu(test_helpers::generate_ul_rrc_message_transfer(
        f1ap_du_ue_id, f1ap_cu_ue_id, srb_id_t::srb1, std::move(rrc_meas_report)));
    report_fatal_error_if_not(this->wait_for_xnap_tx_pdu(xnc_peer_idx, xnap_pdu),
                              "Failed to receive XN Handover Request");
    // report_fatal_error_if_not(test_helpers::is_valid_handover_request(xnap_pdu), "Invalid XN Handover Request");
    report_fatal_error_if_not(
        test_helpers::is_valid_rrc_handover_preparation_info(test_helpers::get_rrc_container(xnap_pdu)),
        "Invalid Handover Preparation Info");

    local_xnap_ue_id =
        uint_to_local_xnap_ue_id(xnap_pdu.pdu.init_msg().value.ho_request()->source_ng_ra_nnode_ue_xn_ap_id);
    peer_xnap_ue_id =
        uint_to_peer_xnap_ue_id(xnap_pdu.pdu.init_msg().value.ho_request()->source_ng_ra_nnode_ue_xn_ap_id);

    return true;
  }

  [[nodiscard]] bool send_handover_preparation_failure(peer_xnap_ue_id_t peer_xnap_ue_id)
  {
    // Inject Handover Preparation Failure.
    get_xnc_cu_cp(xnc_peer_idx).push_tx_pdu(generate_handover_preparation_failure(peer_xnap_ue_id));
    return true;
  }

  [[nodiscard]] bool timeout_handover_command_and_await_handover_cancel()
  {
    // Fail Handover Preparation (XN-C peer CU-CP doesn't respond) and await Handover Cancel.
    if (tick_until(std::chrono::milliseconds(1000), [&]() { return false; })) {
      return false;
    }
    report_fatal_error_if_not(this->wait_for_xnap_tx_pdu(xnc_peer_idx, xnap_pdu), "Failed to receive Handover Cancel");
    report_fatal_error_if_not(test_helpers::is_valid_handover_cancel(xnap_pdu), "Invalid Handover Cancel");
    return true;
  }

  [[nodiscard]] bool
  send_handover_request_ack_and_await_ue_context_modification_request(local_xnap_ue_id_t local_xnap_ue_id,
                                                                      peer_xnap_ue_id_t  peer_xnap_ue_id)
  {
    // Inject Handover Request Ack and wait for UE Context Modification Request (containing RRC Reconfiguration).
    get_xnc_cu_cp(xnc_peer_idx).push_tx_pdu(generate_handover_request_ack(local_xnap_ue_id, peer_xnap_ue_id));

    report_fatal_error_if_not(
        this->wait_for_f1ap_tx_pdu(du_idx, f1ap_pdu),
        "Failed to receive F1AP UE Context Modification Request (containing RRC Reconfiguration)");
    report_fatal_error_if_not(test_helpers::is_valid_ue_context_modification_request(f1ap_pdu),
                              "Invalid UE Context Modification Request");

    // Make sure RRC Reconfiguration is valid
    const byte_buffer& rrc_container = test_helpers::get_rrc_container(f1ap_pdu);
    report_fatal_error_if_not(
        test_helpers::is_valid_rrc_reconfiguration(test_helpers::extract_dl_dcch_msg(rrc_container),
                                                   false,
                                                   std::vector<srb_id_t>{srb_id_t::srb1, srb_id_t::srb2},
                                                   std::vector<drb_id_t>{drb_id_t::drb1},
                                                   {},
                                                   ue_context_setup_req_serving_cell_mo),
        "Invalid RRC Reconfiguration");

    return true;
  }

  [[nodiscard]] bool
  send_ue_context_modification_response_and_await_bearer_context_modification_request(gnb_cu_ue_f1ap_id_t f1ap_cu_ue_id,
                                                                                      gnb_du_ue_f1ap_id_t f1ap_du_ue_id)
  {
    get_du(du_idx).push_ul_pdu(
        test_helpers::generate_ue_context_modification_response(f1ap_du_ue_id, f1ap_cu_ue_id, crnti));

    report_fatal_error_if_not(
        this->wait_for_e1ap_tx_pdu(cu_up_idx, e1ap_pdu),
        "Failed to receive E1AP Bearer Context Modification Request (containing PDCP SN status query)");
    report_fatal_error_if_not(test_helpers::is_valid_bearer_context_modification_request(e1ap_pdu),
                              "Invalid Bearer Context Modification Request");
    return true;
  }

  [[nodiscard]] bool
  send_bearer_context_modification_response_and_await_sn_status_transfer(gnb_cu_cp_ue_e1ap_id_t cu_cp_ue_e1ap_id,
                                                                         gnb_cu_up_ue_e1ap_id_t cu_up_ue_e1ap_id)
  {
    get_cu_up(cu_up_idx).push_tx_pdu(
        generate_bearer_context_modification_response_with_pdcp_status(cu_cp_ue_e1ap_id, cu_up_ue_e1ap_id));

    report_fatal_error_if_not(this->wait_for_xnap_tx_pdu(xnc_peer_idx, xnap_pdu),
                              "Failed to transmist XNAP SN Status transfer to XN-C peer CU-CP");
    report_fatal_error_if_not(test_helpers::is_valid_sn_status_transfer(xnap_pdu), "Invalid XNAP SN Status Transfer");

    return true;
  }

  [[nodiscard]] bool send_ue_context_modification_response_empty(gnb_cu_ue_f1ap_id_t f1ap_cu_ue_id,
                                                                 gnb_du_ue_f1ap_id_t f1ap_du_ue_id)
  {
    // Send an UE Context Modification Response without any DRBs.
    get_du(du_idx).push_ul_pdu(
        test_helpers::generate_ue_context_modification_response(f1ap_du_ue_id, f1ap_cu_ue_id, crnti, {}));
    return true;
  }

  [[nodiscard]] bool
  send_xnap_ue_context_release_and_await_bearer_context_release_command(local_xnap_ue_id_t local_xnap_ue_id,
                                                                        peer_xnap_ue_id_t  peer_xnap_ue_id)
  {
    report_fatal_error_if_not(not this->get_amf().try_pop_rx_pdu(ngap_pdu),
                              "there are still NGAP messages to pop from AMF");
    report_fatal_error_if_not(not this->get_du(du_idx).try_pop_dl_pdu(f1ap_pdu),
                              "there are still F1AP DL messages to pop from DU");
    report_fatal_error_if_not(not this->get_cu_up(cu_up_idx).try_pop_rx_pdu(e1ap_pdu),
                              "there are still E1AP messages to pop from CU-UP");
    report_fatal_error_if_not(not this->get_xnc_cu_cp(xnc_peer_idx).try_pop_rx_pdu(xnap_pdu),
                              "there are still XNAP messages to pop from XN-C peer CU-CP");

    // Inject XNAP UE Context Release and wait for Bearer Context Release Command.
    get_xnc_cu_cp(xnc_peer_idx).push_tx_pdu(generate_ue_context_release(local_xnap_ue_id, peer_xnap_ue_id));
    report_fatal_error_if_not(this->wait_for_e1ap_tx_pdu(cu_up_idx, e1ap_pdu),
                              "Failed to receive Bearer Context Release Command");
    report_fatal_error_if_not(test_helpers::is_valid_bearer_context_release_command(e1ap_pdu),
                              "Invalid Bearer Context Release Command");
    return true;
  }

  [[nodiscard]] bool send_bearer_context_release_complete_and_await_f1ap_ue_context_release_command(
      gnb_cu_cp_ue_e1ap_id_t e1ap_cu_cp_ue_id,
      gnb_cu_up_ue_e1ap_id_t e1ap_cu_up_ue_id)
  {
    // Inject Bearer Context Release Complete and wait for F1AP UE Context Release Command.
    get_cu_up(cu_up_idx).push_tx_pdu(generate_bearer_context_release_complete(e1ap_cu_cp_ue_id, e1ap_cu_up_ue_id));
    report_fatal_error_if_not(this->wait_for_f1ap_tx_pdu(du_idx, f1ap_pdu),
                              "Failed to receive UE Context Release Command");
    report_fatal_error_if_not(test_helpers::is_valid_ue_context_release_command(f1ap_pdu),
                              "Invalid UE Context Release Command");
    return true;
  }

  [[nodiscard]] bool send_f1ap_ue_context_release_complete(gnb_cu_ue_f1ap_id_t f1ap_cu_ue_id,
                                                           gnb_du_ue_f1ap_id_t f1ap_du_ue_id)
  {
    // Inject F1AP UE Context Release Complete and wait for N1AP UE Context Release Command.
    get_du(du_idx).push_ul_pdu(test_helpers::generate_ue_context_release_complete(f1ap_cu_ue_id, f1ap_du_ue_id));
    return true;
  }

  unsigned du_idx       = 0;
  unsigned cu_up_idx    = 0;
  unsigned xnc_peer_idx = 0;

  gnb_du_ue_f1ap_id_t du_ue_id = gnb_du_ue_f1ap_id_t::min;
  gnb_cu_ue_f1ap_id_t cu_ue_id;
  rnti_t              crnti     = to_rnti(0x4601);
  amf_ue_id_t         amf_ue_id = uint_to_amf_ue_id(
      test_rng::uniform_int<uint64_t>(amf_ue_id_to_uint(amf_ue_id_t::min), amf_ue_id_to_uint(amf_ue_id_t::max)));
  gnb_cu_up_ue_e1ap_id_t cu_up_e1ap_id = gnb_cu_up_ue_e1ap_id_t::min;
  gnb_cu_cp_ue_e1ap_id_t cu_cp_e1ap_id;

  local_xnap_ue_id_t source_local_xnap_ue_id = local_xnap_ue_id_t::min;
  peer_xnap_ue_id_t  source_peer_xnap_ue_id  = peer_xnap_ue_id_t::min;

  local_xnap_ue_id_t target_local_xnap_ue_id = local_xnap_ue_id_t::min;
  peer_xnap_ue_id_t  target_peer_xnap_ue_id  = peer_xnap_ue_id_t::min;

  std::optional<uint8_t> ue_context_setup_req_serving_cell_mo = std::nullopt;

  const ue_context* ue_ctx = nullptr;

  pdu_session_id_t psi = uint_to_pdu_session_id(1);
  qos_flow_id_t    qfi = uint_to_qos_flow_id(1);

  ngap_message ngap_pdu;
  xnap_message xnap_pdu;
  f1ap_message f1ap_pdu;
  e1ap_message e1ap_pdu;
};

///////////////////////////////////////////////////////////////////////////////
//                             Source CU-CP
///////////////////////////////////////////////////////////////////////////////
TEST_F(cu_cp_inter_cu_xn_handover_test, when_handover_preparation_failure_is_received_then_handover_fails)
{
  // Attach UE.
  ASSERT_TRUE(attach_ue());

  // Inject RRC Measurement Report and await Handover Request.
  ASSERT_TRUE(send_rrc_measurement_report_and_await_handover_request(
      ue_ctx->cu_ue_id.value(), ue_ctx->du_ue_id.value(), source_local_xnap_ue_id, target_peer_xnap_ue_id));

  // Inject Handover Preparation Failure.
  ASSERT_TRUE(send_handover_preparation_failure(target_peer_xnap_ue_id));

  // STATUS: Handover Preparation failed and no further messages are sent to the AMF.
  report_fatal_error_if_not(not this->get_amf().try_pop_rx_pdu(ngap_pdu),
                            "there are still NGAP messages to pop from AMF");
}

TEST_F(cu_cp_inter_cu_xn_handover_test, when_handover_command_times_out_then_handover_cancel_is_sent)
{
  // Attach UE.
  ASSERT_TRUE(attach_ue());

  // Inject RRC Measurement Report and await Handover Request.
  ASSERT_TRUE(send_rrc_measurement_report_and_await_handover_request(
      ue_ctx->cu_ue_id.value(), ue_ctx->du_ue_id.value(), source_local_xnap_ue_id, target_peer_xnap_ue_id));

  // Timeout Handover Command and await Handover Cancel.
  ASSERT_TRUE(timeout_handover_command_and_await_handover_cancel());
}

TEST_F(cu_cp_inter_cu_xn_handover_test, when_handover_succeeds_then_amf_releases_ue)
{
  // Attach UE.
  ASSERT_TRUE(attach_ue());

  // Inject RRC Measurement Report and await Handover Request.
  ASSERT_TRUE(send_rrc_measurement_report_and_await_handover_request(
      ue_ctx->cu_ue_id.value(), ue_ctx->du_ue_id.value(), source_local_xnap_ue_id, target_peer_xnap_ue_id));

  // Inject Handover Request Ack and await UE Context Modification Request (with RRC Reconfiguration).
  ASSERT_TRUE(send_handover_request_ack_and_await_ue_context_modification_request(target_local_xnap_ue_id,
                                                                                  target_peer_xnap_ue_id));

  // Inject Handover UE Context Modification Response and await for Bearer Context Modification Request (to query PDCP
  // state).
  ASSERT_TRUE(send_ue_context_modification_response_and_await_bearer_context_modification_request(
      ue_ctx->cu_ue_id.value(), ue_ctx->du_ue_id.value()));

  ASSERT_TRUE(send_bearer_context_modification_response_and_await_sn_status_transfer(ue_ctx->cu_cp_e1ap_id.value(),
                                                                                     ue_ctx->cu_up_e1ap_id.value()));

  // Inject XNAP UE Context Release and await Bearer Context Release Command.
  ASSERT_TRUE(send_xnap_ue_context_release_and_await_bearer_context_release_command(target_local_xnap_ue_id,
                                                                                    target_peer_xnap_ue_id));

  // Inject Bearer Context Release Complete and await F1AP UE Context Release Command.
  ASSERT_TRUE(send_bearer_context_release_complete_and_await_f1ap_ue_context_release_command(
      ue_ctx->cu_cp_e1ap_id.value(), ue_ctx->cu_up_e1ap_id.value()));

  // Inject F1AP UE Context Release Complete.
  ASSERT_TRUE(send_f1ap_ue_context_release_complete(ue_ctx->cu_ue_id.value(), ue_ctx->du_ue_id.value()));

  // STATUS: UE should be removed at this stage
  auto report = this->get_cu_cp().get_metrics_handler().request_metrics_report();
  ASSERT_EQ(report.ues.size(), 0) << "UE should be removed";
}

///////////////////////////////////////////////////////////////////////////////
//                             Target CU-CP
///////////////////////////////////////////////////////////////////////////////
TEST_F(cu_cp_inter_cu_xn_handover_test, when_handover_request_received_then_path_switch_request_is_sent)
{
  // Inject Handover Request and await Bearer Context Setup Request.
  ASSERT_TRUE(send_handover_request_and_await_bearer_context_setup_request(source_local_xnap_ue_id));

  // Inject Bearer Context Setup Response and await UE Context Setup Request.
  ASSERT_TRUE(send_bearer_context_setup_response_and_await_ue_context_setup_request());

  // Inject UE Context Setup Response and await Bearer Context Modification Request.
  ASSERT_TRUE(send_ue_context_setup_response_and_await_bearer_context_modification_request());

  // Inject Bearer Context Modification Response and await Handover Request Ack.
  ASSERT_TRUE(send_bearer_context_modification_response_and_await_handover_request_ack());

  // Inject XNAP SN RAN Status Transfer and Bearer Context Modification Response.
  ASSERT_TRUE(send_sn_status_transfer_and_await_bearer_context_modification_request(source_local_xnap_ue_id,
                                                                                    source_peer_xnap_ue_id));

  // Inject Bearer Context Modification Response and ACK the PDCP state modification.
  ASSERT_TRUE(send_bearer_context_modification_response());

  // Inject RRC Reconfiguration Complete and await Path Switch Request.
  ASSERT_TRUE(send_rrc_reconfiguration_complete_and_await_path_switch_request());

  // Inject Path Switch Request Ack and await UE Context Modification Request.
  ASSERT_TRUE(send_path_switch_request_ack_and_await_ue_context_modification_request());

  // Inject UE Context Modification Response to ACK the RRC reconfiguration complete indicator.
  ASSERT_TRUE(send_ue_context_modification_response_empty(cu_ue_id, du_ue_id));
}

TEST_F(cu_cp_inter_cu_xn_handover_test,
       when_path_switch_ack_contains_new_ul_tunnel_then_bearer_context_modification_is_sent)
{
  // Bring handover up to the point where the Path Switch Request has been sent.
  ASSERT_TRUE(send_handover_request_and_await_bearer_context_setup_request(source_local_xnap_ue_id));
  ASSERT_TRUE(send_bearer_context_setup_response_and_await_ue_context_setup_request());
  ASSERT_TRUE(send_ue_context_setup_response_and_await_bearer_context_modification_request());
  ASSERT_TRUE(send_bearer_context_modification_response_and_await_handover_request_ack());
  ASSERT_TRUE(send_sn_status_transfer_and_await_bearer_context_modification_request(source_local_xnap_ue_id,
                                                                                    source_peer_xnap_ue_id));
  ASSERT_TRUE(send_bearer_context_modification_response());
  ASSERT_TRUE(send_rrc_reconfiguration_complete_and_await_path_switch_request());

  // Inject Path Switch Request Ack with a new UL tunnel and await E1AP Bearer Context Modification carrying it.
  ASSERT_TRUE(send_path_switch_request_ack_with_ul_tunnel_and_await_bearer_context_modification_request());

  // Inject Bearer Context Modification Response; expect UE Context Modification Request (RRC release indicator).
  ASSERT_TRUE(send_bearer_context_modification_response());
  report_fatal_error_if_not(this->wait_for_f1ap_tx_pdu(du_idx, f1ap_pdu),
                            "Failed to receive F1AP UE Context Modification Request after tunnel update");
  report_fatal_error_if_not(test_helpers::is_valid_ue_context_modification_request(f1ap_pdu),
                            "Invalid UE Context Modification Request");
  ASSERT_TRUE(send_ue_context_modification_response_empty(cu_ue_id, du_ue_id));
}

TEST_F(cu_cp_inter_cu_xn_handover_test, when_path_switch_request_is_rejected_by_amf_then_routine_fails)
{
  // Bring handover up to the point where the Path Switch Request has been sent.
  ASSERT_TRUE(send_handover_request_and_await_bearer_context_setup_request(source_local_xnap_ue_id));
  ASSERT_TRUE(send_bearer_context_setup_response_and_await_ue_context_setup_request());
  ASSERT_TRUE(send_ue_context_setup_response_and_await_bearer_context_modification_request());
  ASSERT_TRUE(send_bearer_context_modification_response_and_await_handover_request_ack());
  ASSERT_TRUE(send_sn_status_transfer_and_await_bearer_context_modification_request(source_local_xnap_ue_id,
                                                                                    source_peer_xnap_ue_id));
  ASSERT_TRUE(send_bearer_context_modification_response());
  ASSERT_TRUE(send_rrc_reconfiguration_complete_and_await_path_switch_request());

  // Inject Path Switch Request Failure; the routine should terminate without sending further messages.
  get_amf().push_tx_pdu(generate_path_switch_request_failure(
      uint_to_amf_ue_id(1), ran_ue_id_t::min, ngap_cause_t{ngap_cause_radio_network_t::ho_target_not_allowed}));

  report_fatal_error_if_not(not this->get_xnc_cu_cp(xnc_peer_idx).try_pop_rx_pdu(xnap_pdu),
                            "Unexpected XNAP message after path switch failure");
  report_fatal_error_if_not(not this->get_du(du_idx).try_pop_dl_pdu(f1ap_pdu),
                            "Unexpected F1AP message after path switch failure");
  report_fatal_error_if_not(not this->get_cu_up(cu_up_idx).try_pop_rx_pdu(e1ap_pdu),
                            "Unexpected E1AP message after path switch failure");
}

///////////////////////////////////////////////////////////////////////////////
//                             ZigZag Handover
///////////////////////////////////////////////////////////////////////////////
TEST_F(cu_cp_inter_cu_xn_handover_test, when_zigzag_handover_is_performed_then_handovers_are_successful)
{
  // This CU-CP is the target...

  // Inject Handover Request and await Bearer Context Setup Request
  ASSERT_TRUE(send_handover_request_and_await_bearer_context_setup_request(source_local_xnap_ue_id));

  // Inject Bearer Context Setup Response and await UE Context Setup Request
  ASSERT_TRUE(send_bearer_context_setup_response_and_await_ue_context_setup_request());

  // Inject UE Context Setup Response and await Bearer Context Modification Request
  ASSERT_TRUE(send_ue_context_setup_response_and_await_bearer_context_modification_request());

  // Inject Bearer Context Modification Response and await Handover Request Ack
  ASSERT_TRUE(send_bearer_context_modification_response_and_await_handover_request_ack());

  // Inject XNAP SN Status Transfer and Bearer Context Modification Response.
  ASSERT_TRUE(send_sn_status_transfer_and_await_bearer_context_modification_request(source_local_xnap_ue_id,
                                                                                    source_peer_xnap_ue_id));

  // Inject Bearer Context Modification Response and ACK the PDCP state modification.
  ASSERT_TRUE(send_bearer_context_modification_response());

  // Inject RRC Reconfiguration Complete and await Path Switch Request.
  ASSERT_TRUE(send_rrc_reconfiguration_complete_and_await_path_switch_request());

  // Inject Path Switch Request Ack and await UE Context Modification Request.
  ASSERT_TRUE(send_path_switch_request_ack_and_await_ue_context_modification_request());

  // Inject UE Context Modification Response to ACK the RRC reconfiguration complete indicator.
  ASSERT_TRUE(send_ue_context_modification_response_empty(cu_ue_id, du_ue_id));

  // ... and now this CU-CP is the source.

  // Pop messages from XN-C peer CU-CP before injecting the RRC Measurement Report.
  this->get_xnc_cu_cp(xnc_peer_idx).try_pop_rx_pdu(xnap_pdu);

  // Inject RRC Measurement Report and await Handover Request.
  ASSERT_TRUE(send_rrc_measurement_report_and_await_handover_request(
      cu_ue_id,
      du_ue_id,
      source_local_xnap_ue_id,
      target_peer_xnap_ue_id,
      make_byte_buffer("000100420004015f741fe0808bf183fce4fc8052").value()));

  // Inject Handover Request Ack and await UE Context Modification Request (with RRC Reconfiguration).
  ASSERT_TRUE(send_handover_request_ack_and_await_ue_context_modification_request(target_local_xnap_ue_id,
                                                                                  target_peer_xnap_ue_id));

  // Inject Handover UE Context Modification Response and await for Bearer Context Modification Request (to query PDCP
  // state).
  ASSERT_TRUE(send_ue_context_modification_response_and_await_bearer_context_modification_request(cu_ue_id, du_ue_id));

  // Inject Bearer Context Modification Response and await SN Status Transfer.
  ASSERT_TRUE(send_bearer_context_modification_response_and_await_sn_status_transfer(cu_cp_e1ap_id, cu_up_e1ap_id));

  // Inject XNAP UE Context Release and await Bearer Context Release Command.
  ASSERT_TRUE(send_xnap_ue_context_release_and_await_bearer_context_release_command(target_local_xnap_ue_id,
                                                                                    target_peer_xnap_ue_id));

  // Inject Bearer Context Release Complete and await F1AP UE Context Release Command.
  ASSERT_TRUE(
      send_bearer_context_release_complete_and_await_f1ap_ue_context_release_command(cu_cp_e1ap_id, cu_up_e1ap_id));

  // Inject F1AP UE Context Release Complete.
  ASSERT_TRUE(send_f1ap_ue_context_release_complete(cu_ue_id, du_ue_id));

  // STATUS: UE should be removed at this stage
  auto report = this->get_cu_cp().get_metrics_handler().request_metrics_report();
  ASSERT_EQ(report.ues.size(), 0) << "UE should be removed";
}
