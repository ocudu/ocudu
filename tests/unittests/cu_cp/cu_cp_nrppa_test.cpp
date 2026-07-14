// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "tests/test_doubles/e1ap/e1ap_test_message_validators.h"
#include "tests/test_doubles/f1ap/f1ap_test_message_validators.h"
#include "tests/test_doubles/ngap/ngap_test_message_validators.h"
#include "tests/test_doubles/nrppa/nrppa_test_message_validators.h"
#include "tests/test_doubles/nrppa/nrppa_test_messages.h"
#include "tests/unittests/cu_cp/cu_cp_test_environment.h"
#include "ocudu/asn1/f1ap/f1ap_pdu_contents.h"
#include "ocudu/asn1/ngap/common.h"
#include "ocudu/asn1/ngap/ngap_pdu_contents.h"
#include "ocudu/asn1/nrppa/nrppa.h"
#include "ocudu/asn1/nrppa/nrppa_pdu_contents.h"
#include "ocudu/e1ap/common/e1ap_types.h"
#include "ocudu/f1ap/f1ap_message.h"
#include "ocudu/f1ap/f1ap_ue_id_types.h"
#include "ocudu/ngap/ngap_message.h"
#include "ocudu/ngap/ngap_types.h"
#include "ocudu/ran/cu_types.h"
#include "ocudu/ran/positioning/positioning_ids.h"
#include "ocudu/ran/subcarrier_spacing.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace ocucp;

class cu_cp_nrppa_test : public cu_cp_test_environment, public ::testing::Test
{
public:
  cu_cp_nrppa_test() :
    cu_cp_test_environment(cu_cp_test_env_params{/* max_nof_cu_ups */ 8,
                                                 /* max_nof_dus */ 8,
                                                 /* max_nof_ues */ 8192,
                                                 /* max_nof_drbs_per_ue */ 8,
                                                 /* amf_config */ {{default_supported_tracking_area}},
                                                 /* trigger_ho_from_measurements */ false})
  {
    // Run NG setup to completion.
    run_ng_setup();

    // Setup DU.
    std::optional<unsigned> ret = connect_new_du();
    EXPECT_TRUE(ret.has_value());
    du_idx = ret.value();
    EXPECT_TRUE(this->run_f1_setup(du_idx,
                                   int_to_gnb_du_id(0x11),
                                   {{.nci      = nr_cell_identity::create(gnb_id_t{411, 22}, 0U).value(),
                                     .pci      = 1,
                                     .tac      = 7,
                                     .band     = nr_band::n78,
                                     .nr_arfcn = 626748}}));

    // Setup second DU for realistic measurements.
    ret = connect_new_du();
    EXPECT_TRUE(ret.has_value());
    du_idx2 = ret.value();
    EXPECT_TRUE(this->run_f1_setup(du_idx2,
                                   int_to_gnb_du_id(0x22),
                                   {{.nci             = nr_cell_identity::create(gnb_id_t{411, 22}, 1U).value(),
                                     .pci             = 2,
                                     .tac             = 7,
                                     .band            = nr_band::n41,
                                     .nr_arfcn        = 519870,
                                     .meas_timing_cfg = {516030, subcarrier_spacing::kHz30}}}));

    // Setup third DU for realistic measurements.
    ret = connect_new_du();
    EXPECT_TRUE(ret.has_value());
    du_idx3 = ret.value();
    EXPECT_TRUE(this->run_f1_setup(du_idx3,
                                   int_to_gnb_du_id(0x33),
                                   {{.nci             = nr_cell_identity::create(gnb_id_t{411, 22}, 2U).value(),
                                     .pci             = 2,
                                     .tac             = 7,
                                     .band            = nr_band::n41,
                                     .nr_arfcn        = 519870,
                                     .meas_timing_cfg = {516030, subcarrier_spacing::kHz30}}}));

    // Setup CU-UP.
    ret = connect_new_cu_up();
    EXPECT_TRUE(ret.has_value());
    cu_up_idx = ret.value();
    EXPECT_TRUE(this->run_e1_setup(cu_up_idx));
  }

  static asn1::nrppa::nr_ppa_pdu_c get_nrppa_pdu(ngap_message ngap_pdu_)
  {
    asn1::nrppa::nr_ppa_pdu_c nrppa_pdu;
    if (ngap_pdu_.pdu.init_msg().value.type() ==
        asn1::ngap::ngap_elem_procs_o::init_msg_c::types_opts::options::ul_ue_associated_nrppa_transport) {
      asn1::cbit_ref bref(ngap_pdu_.pdu.init_msg().value.ul_ue_associated_nrppa_transport()->nrppa_pdu);
      report_fatal_error_if_not(nrppa_pdu.unpack(bref) == asn1::OCUDUASN_SUCCESS,
                                "Failed to unpack UL UE associated NRPPa-PDU");
    }

    if (ngap_pdu_.pdu.init_msg().value.type() ==
        asn1::ngap::ngap_elem_procs_o::init_msg_c::types_opts::options::ul_non_ue_associated_nrppa_transport) {
      asn1::cbit_ref bref(ngap_pdu_.pdu.init_msg().value.ul_non_ue_associated_nrppa_transport()->nrppa_pdu);
      report_fatal_error_if_not(nrppa_pdu.unpack(bref) == asn1::OCUDUASN_SUCCESS,
                                "Failed to unpack UL non UE associated NRPPa-PDU");
    }

    return nrppa_pdu;
  }

  static ngap_message generate_valid_dl_ue_associated_nrppa_transport_message(const ue_context* ue_ctxt,
                                                                              byte_buffer       nrppa_pdu)
  {
    ngap_message dl_ue_associated_nrppa_transport = {};
    dl_ue_associated_nrppa_transport.pdu.set_init_msg();
    dl_ue_associated_nrppa_transport.pdu.init_msg().load_info_obj(ASN1_NGAP_ID_DL_UE_ASSOCIATED_NRPPA_TRANSPORT);
    asn1::ngap::dl_ue_associated_nrppa_transport_s& nrppa_transport_msg =
        dl_ue_associated_nrppa_transport.pdu.init_msg().value.dl_ue_associated_nrppa_transport();

    nrppa_transport_msg->amf_ue_ngap_id = amf_ue_id_to_uint(ue_ctxt->amf_ue_id.value());
    nrppa_transport_msg->ran_ue_ngap_id = ran_ue_id_to_uint(ue_ctxt->ran_ue_id.value());

    nrppa_transport_msg->routing_id =
        make_byte_buffer("61666536653032392d626435662d343737362d383262342d636233353233356130663838").value();

    nrppa_transport_msg->nrppa_pdu = std::move(nrppa_pdu);

    return dl_ue_associated_nrppa_transport;
  }

  static ngap_message generate_valid_dl_non_ue_associated_nrppa_transport_message(byte_buffer nrppa_pdu)
  {
    ngap_message dl_non_ue_associated_nrppa_transport = {};
    dl_non_ue_associated_nrppa_transport.pdu.set_init_msg();
    dl_non_ue_associated_nrppa_transport.pdu.init_msg().load_info_obj(
        ASN1_NGAP_ID_DL_NON_UE_ASSOCIATED_NRPPA_TRANSPORT);
    asn1::ngap::dl_non_ue_associated_nrppa_transport_s& nrppa_transport_msg =
        dl_non_ue_associated_nrppa_transport.pdu.init_msg().value.dl_non_ue_associated_nrppa_transport();

    nrppa_transport_msg->routing_id =
        make_byte_buffer("61666536653032392d626435662d343737362d383262342d636233353233356130663838").value();

    nrppa_transport_msg->nrppa_pdu = std::move(nrppa_pdu);

    return dl_non_ue_associated_nrppa_transport;
  }

  [[nodiscard]] bool
  attach_ue(gnb_du_ue_f1ap_id_t f1ap_ue, rnti_t rnti, amf_ue_id_t amf_ue, gnb_cu_up_ue_e1ap_id_t cu_up_ue)
  {
    if (!cu_cp_test_environment::attach_ue(
            du_idx, cu_up_idx, f1ap_ue, rnti, amf_ue, cu_up_ue, psi, drb_id_t::drb1, qfi)) {
      return false;
    }

    if (this->find_ue_context(du_idx, f1ap_ue) != nullptr) {
      test_ues.emplace(f1ap_ue, find_ue_context(du_idx, f1ap_ue));
      return true;
    }

    return false;
  }

  [[nodiscard]] bool send_rrc_measurement_report(
      gnb_cu_ue_f1ap_id_t f1ap_cu_ue_id,
      gnb_du_ue_f1ap_id_t f1ap_du_ue_id,
      byte_buffer         rrc_meas_report = make_byte_buffer("000800410004015f741fe0804bf183fcaa6e9699").value())
  {
    // Inject UL RRC Message (containing RRC Measurement Report).
    get_du(du_idx).push_ul_pdu(test_helpers::generate_ul_rrc_message_transfer(
        f1ap_du_ue_id, f1ap_cu_ue_id, srb_id_t::srb1, std::move(rrc_meas_report)));
    return true;
  }

  [[nodiscard]] bool send_e_cid_measurement_initiation_request_and_await_e_cid_measurement_initiation_failure(
      const ue_context* ue_ctxt,
      byte_buffer       e_cid_meas_init_request)
  {
    report_fatal_error_if_not(not this->get_amf().try_pop_rx_pdu(ngap_pdu),
                              "there are still NGAP messages to pop from AMF");
    report_fatal_error_if_not(not this->get_du(du_idx).try_pop_dl_pdu(f1ap_pdu),
                              "there are still F1AP DL messages to pop from DU");
    report_fatal_error_if_not(not this->get_du(du_idx2).try_pop_dl_pdu(f1ap_pdu2),
                              "there are still F1AP DL messages to pop from DU2");
    report_fatal_error_if_not(not this->get_du(du_idx3).try_pop_dl_pdu(f1ap_pdu3),
                              "there are still F1AP DL messages to pop from DU3");
    report_fatal_error_if_not(not this->get_cu_up(cu_up_idx).try_pop_rx_pdu(e1ap_pdu),
                              "there are still E1AP messages to pop from CU-UP");

    // Inject E-CID Measurement Initiation Request and wait for E-CID Measurement Initiation Failure.
    get_amf().push_tx_pdu(
        generate_valid_dl_ue_associated_nrppa_transport_message(ue_ctxt, std::move(e_cid_meas_init_request)));
    report_fatal_error_if_not(this->wait_for_ngap_tx_pdu(ngap_pdu),
                              "Failed to receive UL UE Associated NRPPa Transport");
    report_fatal_error_if_not(test_helpers::is_valid_ul_ue_associated_nrppa_transport(ngap_pdu),
                              "Invalid UL UE Associated NRPPa Transport");
    report_fatal_error_if_not(test_helpers::is_valid_e_cid_meas_initiation_failure(get_nrppa_pdu(ngap_pdu)),
                              "Invalid E-CID Measurement Initiation Failure");
    return true;
  }

  [[nodiscard]] bool send_e_cid_measurement_initiation_request_and_await_e_cid_measurement_initiation_response(
      const ue_context* ue_ctxt,
      byte_buffer       e_cid_meas_init_request)
  {
    report_fatal_error_if_not(not this->get_amf().try_pop_rx_pdu(ngap_pdu),
                              "there are still NGAP messages to pop from AMF");
    report_fatal_error_if_not(not this->get_du(du_idx).try_pop_dl_pdu(f1ap_pdu),
                              "there are still F1AP DL messages to pop from DU");
    report_fatal_error_if_not(not this->get_du(du_idx2).try_pop_dl_pdu(f1ap_pdu2),
                              "there are still F1AP DL messages to pop from DU2");
    report_fatal_error_if_not(not this->get_du(du_idx3).try_pop_dl_pdu(f1ap_pdu3),
                              "there are still F1AP DL messages to pop from DU3");
    report_fatal_error_if_not(not this->get_cu_up(cu_up_idx).try_pop_rx_pdu(e1ap_pdu),
                              "there are still E1AP messages to pop from CU-UP");

    // Inject E-CID Measurement Initiation Request and wait for E-CID Measurement Initiation Response.
    get_amf().push_tx_pdu(
        generate_valid_dl_ue_associated_nrppa_transport_message(ue_ctxt, std::move(e_cid_meas_init_request)));
    report_fatal_error_if_not(this->wait_for_ngap_tx_pdu(ngap_pdu),
                              "Failed to receive UL UE Associated NRPPa Transport");
    report_fatal_error_if_not(test_helpers::is_valid_ul_ue_associated_nrppa_transport(ngap_pdu),
                              "Invalid UL UE Associated NRPPa Transport");
    report_fatal_error_if_not(test_helpers::is_valid_e_cid_meas_initiation_response(get_nrppa_pdu(ngap_pdu)),
                              "Invalid E-CID Measurement Initiation Response");

    // Extract RAN UE Measurement ID.
    ran_ue_meas_id = uint_to_ran_ue_meas_id(
        get_nrppa_pdu(ngap_pdu).successful_outcome().value.e_c_id_meas_initiation_resp()->ran_ue_meas_id);

    return true;
  }

  [[nodiscard]] bool await_e_cid_measurement_report()
  {
    report_fatal_error_if_not(this->wait_for_ngap_tx_pdu(ngap_pdu),
                              "Failed to receive UL UE Associated NRPPa Transport");
    report_fatal_error_if_not(test_helpers::is_valid_ul_ue_associated_nrppa_transport(ngap_pdu),
                              "Invalid UL UE Associated NRPPa Transport");
    report_fatal_error_if_not(test_helpers::is_valid_e_cid_meas_report(get_nrppa_pdu(ngap_pdu)),
                              "Invalid E-CID Measurement Report");
    return true;
  }

  [[nodiscard]] bool send_e_cid_measurement_termination_command(const ue_context* ue_ctxt)
  {
    report_fatal_error_if_not(not this->get_amf().try_pop_rx_pdu(ngap_pdu),
                              "there are still NGAP messages to pop from AMF");
    report_fatal_error_if_not(not this->get_du(du_idx).try_pop_dl_pdu(f1ap_pdu),
                              "there are still F1AP DL messages to pop from DU");
    report_fatal_error_if_not(not this->get_du(du_idx2).try_pop_dl_pdu(f1ap_pdu2),
                              "there are still F1AP DL messages to pop from DU2");
    report_fatal_error_if_not(not this->get_du(du_idx3).try_pop_dl_pdu(f1ap_pdu3),
                              "there are still F1AP DL messages to pop from DU3");
    report_fatal_error_if_not(not this->get_cu_up(cu_up_idx).try_pop_rx_pdu(e1ap_pdu),
                              "there are still E1AP messages to pop from CU-UP");

    // Inject E-CID Measurement Termination Command.
    get_amf().push_tx_pdu(generate_valid_dl_ue_associated_nrppa_transport_message(
        ue_ctxt, generate_valid_nrppa_e_cid_measurement_termination_command(lmf_ue_meas_id, ran_ue_meas_id)));
    return true;
  }

  [[nodiscard]] bool send_nrppa_trp_information_request()
  {
    report_fatal_error_if_not(not this->get_amf().try_pop_rx_pdu(ngap_pdu),
                              "there are still NGAP messages to pop from AMF");
    report_fatal_error_if_not(not this->get_du(du_idx).try_pop_dl_pdu(f1ap_pdu),
                              "there are still F1AP DL messages to pop from DU");
    report_fatal_error_if_not(not this->get_du(du_idx2).try_pop_dl_pdu(f1ap_pdu2),
                              "there are still F1AP DL messages to pop from DU2");
    report_fatal_error_if_not(not this->get_du(du_idx3).try_pop_dl_pdu(f1ap_pdu3),
                              "there are still F1AP DL messages to pop from DU3");
    report_fatal_error_if_not(not this->get_cu_up(cu_up_idx).try_pop_rx_pdu(e1ap_pdu),
                              "there are still E1AP messages to pop from CU-UP");

    // Inject NRPPA TRP Information Request and wait for F1AP TRP Information Request.
    get_amf().push_tx_pdu(
        generate_valid_dl_non_ue_associated_nrppa_transport_message(generate_valid_trp_information_request()));

    return true;
  }

  [[nodiscard]] bool await_f1ap_trp_information_request(unsigned du_id, f1ap_message& pdu)
  {
    // Wait for F1AP TRP Information Request.
    report_fatal_error_if_not(this->wait_for_f1ap_tx_pdu(du_id, pdu), "Failed to receive F1AP TRP Information Request");
    report_fatal_error_if_not(test_helpers::is_valid_f1ap_trp_information_request(pdu),
                              "Invalid F1AP TRP Information Request");

    return true;
  }

  [[nodiscard]] bool send_f1ap_trp_information_response(unsigned du_id, const std::vector<trp_id_t>& trp_ids)
  {
    report_fatal_error_if_not(not this->get_amf().try_pop_rx_pdu(ngap_pdu),
                              "there are still NGAP messages to pop from AMF");
    report_fatal_error_if_not(not this->get_du(du_idx).try_pop_dl_pdu(f1ap_pdu),
                              "there are still F1AP DL messages to pop from DU");
    report_fatal_error_if_not(not this->get_du(du_idx2).try_pop_dl_pdu(f1ap_pdu2),
                              "there are still F1AP DL messages to pop from DU2");
    report_fatal_error_if_not(not this->get_du(du_idx3).try_pop_dl_pdu(f1ap_pdu3),
                              "there are still F1AP DL messages to pop from DU3");
    report_fatal_error_if_not(not this->get_cu_up(cu_up_idx).try_pop_rx_pdu(e1ap_pdu),
                              "there are still E1AP messages to pop from CU-UP");

    // Inject F1AP TRP Information Response.
    get_du(du_id).push_ul_pdu(test_helpers::generate_trp_information_response(trp_ids));

    return true;
  }

  [[nodiscard]] bool await_nrppa_trp_information_response(unsigned trp_info_list_trp_resp_size)
  {
    // Wait for NRPPa TRP Information Response.
    report_fatal_error_if_not(this->wait_for_ngap_tx_pdu(ngap_pdu),
                              "Failed to receive UL non UE Associated NRPPa Transport");
    report_fatal_error_if_not(test_helpers::is_valid_ul_non_ue_associated_nrppa_transport(ngap_pdu),
                              "Invalid UL non UE associated NRPPa Transport");

    asn1::nrppa::nr_ppa_pdu_c nrppa_pdu = get_nrppa_pdu(ngap_pdu);
    report_fatal_error_if_not(test_helpers::is_valid_nrppa_trp_information_response(nrppa_pdu),
                              "Invalid NRPPa TRP Information Response");
    report_fatal_error_if_not(nrppa_pdu.successful_outcome().value.trp_info_resp()->trp_info_list_trp_resp.size() ==
                                  trp_info_list_trp_resp_size,
                              "Invalid size of TRP Information Response List");

    return true;
  }

  [[nodiscard]] bool send_f1ap_trp_information_failure(unsigned du_id)
  {
    report_fatal_error_if_not(not this->get_amf().try_pop_rx_pdu(ngap_pdu),
                              "there are still NGAP messages to pop from AMF");
    report_fatal_error_if_not(not this->get_du(du_idx).try_pop_dl_pdu(f1ap_pdu),
                              "there are still F1AP DL messages to pop from DU");
    report_fatal_error_if_not(not this->get_du(du_idx2).try_pop_dl_pdu(f1ap_pdu2),
                              "there are still F1AP DL messages to pop from DU2");
    report_fatal_error_if_not(not this->get_du(du_idx3).try_pop_dl_pdu(f1ap_pdu3),
                              "there are still F1AP DL messages to pop from DU3");
    report_fatal_error_if_not(not this->get_cu_up(cu_up_idx).try_pop_rx_pdu(e1ap_pdu),
                              "there are still E1AP messages to pop from CU-UP");

    // Inject F1AP TRP Information Failure.
    get_du(du_id).push_ul_pdu(test_helpers::generate_trp_information_failure());

    return true;
  }

  [[nodiscard]] bool await_nrppa_trp_information_failure()
  {
    // Inject F1AP TRP Information Failures and wait for NRPPa TRP Information Failure.
    report_fatal_error_if_not(this->wait_for_ngap_tx_pdu(ngap_pdu),
                              "Failed to receive UL non UE Associated NRPPa Transport");
    report_fatal_error_if_not(test_helpers::is_valid_ul_non_ue_associated_nrppa_transport(ngap_pdu),
                              "Invalid UL non UE associated NRPPa Transport");
    report_fatal_error_if_not(test_helpers::is_valid_nrppa_trp_information_failure(get_nrppa_pdu(ngap_pdu)),
                              "Invalid NRPPa TRP Information Failure");

    return true;
  }

  [[nodiscard]] bool run_successful_trp_information_procedure()
  {
    std::map<unsigned, std::vector<trp_id_t>> trp_ids_per_du_idx = {
        {du_idx, {uint_to_trp_id(1), uint_to_trp_id(2), uint_to_trp_id(3)}},
        {du_idx2, {uint_to_trp_id(4), uint_to_trp_id(5), uint_to_trp_id(6)}},
        {du_idx3, {uint_to_trp_id(7), uint_to_trp_id(8), uint_to_trp_id(9)}}};

    // Inject TRP information request and await F1AP TRP information request.
    if (not send_nrppa_trp_information_request()) {
      return false;
    }
    for (const auto& [du_index, trp_ids] : trp_ids_per_du_idx) {
      // Await F1AP TRP information request on DU.
      if (not await_f1ap_trp_information_request(du_index, f1ap_pdu)) {
        return false;
      }
      // Inject F1AP TRP information response from DU.
      if (not send_f1ap_trp_information_response(du_index, trp_ids)) {
        return false;
      }
    }

    // Await NRPPa TRP information response.
    return await_nrppa_trp_information_response(9);
  }

  [[nodiscard]] bool
  send_nrppa_positioning_information_request_and_await_f1ap_positioning_information_request(const ue_context* ue_ctxt)
  {
    report_fatal_error_if_not(not this->get_amf().try_pop_rx_pdu(ngap_pdu),
                              "there are still NGAP messages to pop from AMF");
    report_fatal_error_if_not(not this->get_du(du_idx).try_pop_dl_pdu(f1ap_pdu),
                              "there are still F1AP DL messages to pop from DU");
    report_fatal_error_if_not(not this->get_du(du_idx2).try_pop_dl_pdu(f1ap_pdu2),
                              "there are still F1AP DL messages to pop from DU2");
    report_fatal_error_if_not(not this->get_du(du_idx3).try_pop_dl_pdu(f1ap_pdu3),
                              "there are still F1AP DL messages to pop from DU3");
    report_fatal_error_if_not(not this->get_cu_up(cu_up_idx).try_pop_rx_pdu(e1ap_pdu),
                              "there are still E1AP messages to pop from CU-UP");

    // Inject NRPPA Positioning Information Request and wait for F1AP Positioning Information Request.
    get_amf().push_tx_pdu(generate_valid_dl_ue_associated_nrppa_transport_message(
        ue_ctxt, generate_valid_positioning_information_request()));
    report_fatal_error_if_not(this->wait_for_f1ap_tx_pdu(du_idx, f1ap_pdu),
                              "Failed to receive F1AP Positioning Information Request");
    report_fatal_error_if_not(test_helpers::is_valid_f1ap_positioning_information_request(f1ap_pdu),
                              "Invalid F1AP Positioning Information Request");

    return true;
  }

  [[nodiscard]] bool
  send_f1ap_positioning_information_response_and_await_nrppa_positioning_information_response(const ue_context* ue_ctxt)
  {
    report_fatal_error_if_not(not this->get_amf().try_pop_rx_pdu(ngap_pdu),
                              "there are still NGAP messages to pop from AMF");
    report_fatal_error_if_not(not this->get_du(du_idx).try_pop_dl_pdu(f1ap_pdu),
                              "there are still F1AP DL messages to pop from DU");
    report_fatal_error_if_not(not this->get_du(du_idx2).try_pop_dl_pdu(f1ap_pdu2),
                              "there are still F1AP DL messages to pop from DU2");
    report_fatal_error_if_not(not this->get_du(du_idx3).try_pop_dl_pdu(f1ap_pdu3),
                              "there are still F1AP DL messages to pop from DU3");
    report_fatal_error_if_not(not this->get_cu_up(cu_up_idx).try_pop_rx_pdu(e1ap_pdu),
                              "there are still E1AP messages to pop from CU-UP");

    // Inject F1AP Positioning Information Response and wait for NRPPa Positioning Information Response.
    get_du(du_idx).push_ul_pdu(
        test_helpers::generate_positioning_information_response(ue_ctxt->du_ue_id.value(), ue_ctxt->cu_ue_id.value()));
    report_fatal_error_if_not(this->wait_for_ngap_tx_pdu(ngap_pdu),
                              "Failed to receive UL UE Associated NRPPa Transport");
    report_fatal_error_if_not(test_helpers::is_valid_ul_ue_associated_nrppa_transport(ngap_pdu),
                              "Invalid UL UE associated NRPPa Transport");
    report_fatal_error_if_not(test_helpers::is_valid_nrppa_positioning_information_response(get_nrppa_pdu(ngap_pdu)),
                              "Invalid NRPPa Positioning Information Response");

    return true;
  }

  [[nodiscard]] bool
  send_f1ap_positioning_information_response_and_await_nrppa_positioning_information_failure(const ue_context* ue_ctxt)
  {
    report_fatal_error_if_not(not this->get_amf().try_pop_rx_pdu(ngap_pdu),
                              "there are still NGAP messages to pop from AMF");
    report_fatal_error_if_not(not this->get_du(du_idx).try_pop_dl_pdu(f1ap_pdu),
                              "there are still F1AP DL messages to pop from DU");
    report_fatal_error_if_not(not this->get_du(du_idx2).try_pop_dl_pdu(f1ap_pdu2),
                              "there are still F1AP DL messages to pop from DU2");
    report_fatal_error_if_not(not this->get_du(du_idx3).try_pop_dl_pdu(f1ap_pdu3),
                              "there are still F1AP DL messages to pop from DU3");
    report_fatal_error_if_not(not this->get_cu_up(cu_up_idx).try_pop_rx_pdu(e1ap_pdu),
                              "there are still E1AP messages to pop from CU-UP");

    // Inject F1AP Positioning Information Failure and wait for NRPPa Positioning Information Failure.
    get_du(du_idx).push_ul_pdu(
        test_helpers::generate_positioning_information_failure(ue_ctxt->du_ue_id.value(), ue_ctxt->cu_ue_id.value()));
    report_fatal_error_if_not(this->wait_for_ngap_tx_pdu(ngap_pdu),
                              "Failed to receive UL UE Associated NRPPa Transport");
    report_fatal_error_if_not(test_helpers::is_valid_ul_ue_associated_nrppa_transport(ngap_pdu),
                              "Invalid UL UE associated NRPPa Transport");
    report_fatal_error_if_not(test_helpers::is_valid_nrppa_positioning_information_failure(get_nrppa_pdu(ngap_pdu)),
                              "Invalid NRPPa Positioning Information Failure");

    return true;
  }

  [[nodiscard]] bool
  send_nrppa_positioning_information_request_and_await_nrppa_positioning_information_failure(const ue_context* ue_ctxt)
  {
    report_fatal_error_if_not(not this->get_amf().try_pop_rx_pdu(ngap_pdu),
                              "there are still NGAP messages to pop from AMF");
    report_fatal_error_if_not(not this->get_du(du_idx).try_pop_dl_pdu(f1ap_pdu),
                              "there are still F1AP DL messages to pop from DU");
    report_fatal_error_if_not(not this->get_du(du_idx2).try_pop_dl_pdu(f1ap_pdu2),
                              "there are still F1AP DL messages to pop from DU2");
    report_fatal_error_if_not(not this->get_du(du_idx3).try_pop_dl_pdu(f1ap_pdu3),
                              "there are still F1AP DL messages to pop from DU3");
    report_fatal_error_if_not(not this->get_cu_up(cu_up_idx).try_pop_rx_pdu(e1ap_pdu),
                              "there are still E1AP messages to pop from CU-UP");

    // Inject NRPPA Positioning Information Request and wait for NRPPa Positioning Information Failure.
    get_amf().push_tx_pdu(generate_valid_dl_ue_associated_nrppa_transport_message(
        ue_ctxt, generate_valid_positioning_information_request()));

    report_fatal_error_if_not(this->wait_for_ngap_tx_pdu(ngap_pdu),
                              "Failed to receive UL UE Associated NRPPa Transport");
    report_fatal_error_if_not(test_helpers::is_valid_ul_ue_associated_nrppa_transport(ngap_pdu),
                              "Invalid UL UE associated NRPPa Transport");
    report_fatal_error_if_not(test_helpers::is_valid_nrppa_positioning_information_failure(get_nrppa_pdu(ngap_pdu)),
                              "Invalid NRPPa Positioning Information Failure");
    return true;
  }

  [[nodiscard]] bool
  send_nrppa_positioning_activation_request_and_await_f1ap_positioning_activation_request(const ue_context* ue_ctxt)
  {
    report_fatal_error_if_not(not this->get_amf().try_pop_rx_pdu(ngap_pdu),
                              "there are still NGAP messages to pop from AMF");
    report_fatal_error_if_not(not this->get_du(du_idx).try_pop_dl_pdu(f1ap_pdu),
                              "there are still F1AP DL messages to pop from DU");
    report_fatal_error_if_not(not this->get_du(du_idx2).try_pop_dl_pdu(f1ap_pdu2),
                              "there are still F1AP DL messages to pop from DU2");
    report_fatal_error_if_not(not this->get_du(du_idx3).try_pop_dl_pdu(f1ap_pdu3),
                              "there are still F1AP DL messages to pop from DU3");
    report_fatal_error_if_not(not this->get_cu_up(cu_up_idx).try_pop_rx_pdu(e1ap_pdu),
                              "there are still E1AP messages to pop from CU-UP");

    // Inject NRPPA Positioning Activation Request and wait for F1AP Positioning Activation Request.
    get_amf().push_tx_pdu(generate_valid_dl_ue_associated_nrppa_transport_message(
        ue_ctxt, generate_valid_positioning_activation_request()));
    report_fatal_error_if_not(this->wait_for_f1ap_tx_pdu(du_idx, f1ap_pdu),
                              "Failed to receive F1AP Positioning Activation Request");
    report_fatal_error_if_not(test_helpers::is_valid_f1ap_positioning_activation_request(f1ap_pdu),
                              "Invalid F1AP Positioning Activation Request");

    return true;
  }

  [[nodiscard]] bool
  send_f1ap_positioning_activation_response_and_await_nrppa_positioning_activation_response(const ue_context* ue_ctxt)
  {
    report_fatal_error_if_not(not this->get_amf().try_pop_rx_pdu(ngap_pdu),
                              "there are still NGAP messages to pop from AMF");
    report_fatal_error_if_not(not this->get_du(du_idx).try_pop_dl_pdu(f1ap_pdu),
                              "there are still F1AP DL messages to pop from DU");
    report_fatal_error_if_not(not this->get_du(du_idx2).try_pop_dl_pdu(f1ap_pdu2),
                              "there are still F1AP DL messages to pop from DU2");
    report_fatal_error_if_not(not this->get_du(du_idx3).try_pop_dl_pdu(f1ap_pdu3),
                              "there are still F1AP DL messages to pop from DU3");
    report_fatal_error_if_not(not this->get_cu_up(cu_up_idx).try_pop_rx_pdu(e1ap_pdu),
                              "there are still E1AP messages to pop from CU-UP");

    // Inject F1AP Positioning Activation Response and wait for NRPPa Positioning Activation Response.
    get_du(du_idx).push_ul_pdu(
        test_helpers::generate_positioning_activation_response(ue_ctxt->du_ue_id.value(), ue_ctxt->cu_ue_id.value()));
    report_fatal_error_if_not(this->wait_for_ngap_tx_pdu(ngap_pdu),
                              "Failed to receive UL UE Associated NRPPa Transport");
    report_fatal_error_if_not(test_helpers::is_valid_ul_ue_associated_nrppa_transport(ngap_pdu),
                              "Invalid UL UE associated NRPPa Transport");
    report_fatal_error_if_not(test_helpers::is_valid_nrppa_positioning_activation_response(get_nrppa_pdu(ngap_pdu)),
                              "Invalid NRPPa Positioning Activation Response");

    return true;
  }

  [[nodiscard]] bool
  send_f1ap_positioning_activation_failure_and_await_nrppa_positioning_activation_failure(const ue_context* ue_ctxt)
  {
    report_fatal_error_if_not(not this->get_amf().try_pop_rx_pdu(ngap_pdu),
                              "there are still NGAP messages to pop from AMF");
    report_fatal_error_if_not(not this->get_du(du_idx).try_pop_dl_pdu(f1ap_pdu),
                              "there are still F1AP DL messages to pop from DU");
    report_fatal_error_if_not(not this->get_du(du_idx2).try_pop_dl_pdu(f1ap_pdu2),
                              "there are still F1AP DL messages to pop from DU2");
    report_fatal_error_if_not(not this->get_du(du_idx3).try_pop_dl_pdu(f1ap_pdu3),
                              "there are still F1AP DL messages to pop from DU3");
    report_fatal_error_if_not(not this->get_cu_up(cu_up_idx).try_pop_rx_pdu(e1ap_pdu),
                              "there are still E1AP messages to pop from CU-UP");

    // Inject F1AP Positioning Activation Failure and wait for NRPPa Positioning Activation Failure.
    get_du(du_idx).push_ul_pdu(
        test_helpers::generate_positioning_activation_failure(ue_ctxt->du_ue_id.value(), ue_ctxt->cu_ue_id.value()));
    report_fatal_error_if_not(this->wait_for_ngap_tx_pdu(ngap_pdu),
                              "Failed to receive UL UE Associated NRPPa Transport");
    report_fatal_error_if_not(test_helpers::is_valid_ul_ue_associated_nrppa_transport(ngap_pdu),
                              "Invalid UL UE associated NRPPa Transport");
    report_fatal_error_if_not(test_helpers::is_valid_nrppa_positioning_activation_failure(get_nrppa_pdu(ngap_pdu)),
                              "Invalid NRPPa Positioning Activation Failure");

    return true;
  }

  [[nodiscard]] bool
  send_nrppa_positioning_activation_request_and_await_nrppa_positioning_activation_failure(const ue_context* ue_ctxt)
  {
    report_fatal_error_if_not(not this->get_amf().try_pop_rx_pdu(ngap_pdu),
                              "there are still NGAP messages to pop from AMF");
    report_fatal_error_if_not(not this->get_du(du_idx).try_pop_dl_pdu(f1ap_pdu),
                              "there are still F1AP DL messages to pop from DU");
    report_fatal_error_if_not(not this->get_du(du_idx2).try_pop_dl_pdu(f1ap_pdu2),
                              "there are still F1AP DL messages to pop from DU2");
    report_fatal_error_if_not(not this->get_du(du_idx3).try_pop_dl_pdu(f1ap_pdu3),
                              "there are still F1AP DL messages to pop from DU3");
    report_fatal_error_if_not(not this->get_cu_up(cu_up_idx).try_pop_rx_pdu(e1ap_pdu),
                              "there are still E1AP messages to pop from CU-UP");

    // Inject NRPPA Positioning Activation Request and wait for NRPPa Positioning Activation Failure.
    get_amf().push_tx_pdu(generate_valid_dl_ue_associated_nrppa_transport_message(
        ue_ctxt, generate_valid_positioning_activation_request()));

    report_fatal_error_if_not(this->wait_for_ngap_tx_pdu(ngap_pdu),
                              "Failed to receive UL UE Associated NRPPa Transport");
    report_fatal_error_if_not(test_helpers::is_valid_ul_ue_associated_nrppa_transport(ngap_pdu),
                              "Invalid UL UE associated NRPPa Transport");
    report_fatal_error_if_not(test_helpers::is_valid_nrppa_positioning_activation_failure(get_nrppa_pdu(ngap_pdu)),
                              "Invalid NRPPa Positioning Activation Failure");
    return true;
  }

  [[nodiscard]] bool send_nrppa_measurement_request(
      lmf_meas_id_t                                       meas_id,
      const std::vector<trp_id_t>&                        trp_ids             = {uint_to_trp_id(1),
                                                                                 uint_to_trp_id(2),
                                                                                 uint_to_trp_id(3),
                                                                                 uint_to_trp_id(4),
                                                                                 uint_to_trp_id(5),
                                                                                 uint_to_trp_id(6),
                                                                                 uint_to_trp_id(7),
                                                                                 uint_to_trp_id(8),
                                                                                 uint_to_trp_id(9)},
      const std::vector<trp_meas_quantities_list_item_t>& trp_meas_quantities = {{trp_meas_quantities_item_t::ul_rtoa}})
  {
    report_fatal_error_if_not(not this->get_amf().try_pop_rx_pdu(ngap_pdu),
                              "there are still NGAP messages to pop from AMF");
    report_fatal_error_if_not(not this->get_du(du_idx).try_pop_dl_pdu(f1ap_pdu),
                              "there are still F1AP DL messages to pop from DU");
    report_fatal_error_if_not(not this->get_du(du_idx).try_pop_dl_pdu(f1ap_pdu2),
                              "there are still F1AP DL messages to pop from DU2");
    report_fatal_error_if_not(not this->get_du(du_idx3).try_pop_dl_pdu(f1ap_pdu3),
                              "there are still F1AP DL messages to pop from DU3");
    report_fatal_error_if_not(not this->get_cu_up(cu_up_idx).try_pop_rx_pdu(e1ap_pdu),
                              "there are still E1AP messages to pop from CU-UP");

    std::vector<trp_meas_request_item_t> trp_meas_request_list;
    trp_meas_request_list.reserve(trp_ids.size());
    for (const auto& trp_id : trp_ids) {
      trp_meas_request_list.push_back(trp_meas_request_item_t{trp_id});
    }

    // Inject NRPPa Measurement Request.
    get_amf().push_tx_pdu(generate_valid_dl_non_ue_associated_nrppa_transport_message(
        generate_valid_nrppa_measurement_request(meas_id, trp_meas_request_list, trp_meas_quantities)));

    return true;
  }

  [[nodiscard]] bool await_f1ap_positioning_measurement_request(unsigned du_id, f1ap_message& pdu)
  {
    // Wait for F1AP Positioning Measurement Request.
    report_fatal_error_if_not(this->wait_for_f1ap_tx_pdu(du_id, pdu),
                              "Failed to receive F1AP Positioning Measurement Request");
    report_fatal_error_if_not(test_helpers::is_valid_f1ap_positioning_measurement_request(pdu),
                              "Invalid F1AP Positioning Measurement Request");

    // Extract RAN Measurement ID.
    ran_meas_id = uint_to_ran_meas_id(pdu.pdu.init_msg().value.positioning_meas_request()->ran_meas_id);

    return true;
  }

  [[nodiscard]] bool send_f1ap_positioning_measurement_response(unsigned                     du_id,
                                                                lmf_meas_id_t                lmf_meas,
                                                                ran_meas_id_t                ran_meas,
                                                                const std::vector<trp_id_t>& trp_ids,
                                                                unsigned                     transaction_id = 1)
  {
    report_fatal_error_if_not(not this->get_amf().try_pop_rx_pdu(ngap_pdu),
                              "there are still NGAP messages to pop from AMF");
    report_fatal_error_if_not(not this->get_du(du_idx).try_pop_dl_pdu(f1ap_pdu),
                              "there are still F1AP DL messages to pop from DU");
    report_fatal_error_if_not(not this->get_du(du_idx2).try_pop_dl_pdu(f1ap_pdu2),
                              "there are still F1AP DL messages to pop from DU2");
    report_fatal_error_if_not(not this->get_du(du_idx3).try_pop_dl_pdu(f1ap_pdu3),
                              "there are still F1AP DL messages to pop from DU3");
    report_fatal_error_if_not(not this->get_cu_up(cu_up_idx).try_pop_rx_pdu(e1ap_pdu),
                              "there are still E1AP messages to pop from CU-UP");

    // Inject F1AP Positioning Measurement Response.
    get_du(du_id).push_ul_pdu(
        test_helpers::generate_positioning_measurement_response(lmf_meas, ran_meas, trp_ids, transaction_id));

    return true;
  }

  [[nodiscard]] bool send_f1ap_positioning_measurement_response_with_aoa(unsigned                     du_id,
                                                                         lmf_meas_id_t                lmf_meas,
                                                                         ran_meas_id_t                ran_meas,
                                                                         const std::vector<trp_id_t>& trp_ids,
                                                                         uint16_t                     azimuth_ao_a,
                                                                         unsigned transaction_id = 1)
  {
    report_fatal_error_if_not(not this->get_amf().try_pop_rx_pdu(ngap_pdu),
                              "there are still NGAP messages to pop from AMF");
    report_fatal_error_if_not(not this->get_du(du_idx).try_pop_dl_pdu(f1ap_pdu),
                              "there are still F1AP DL messages to pop from DU");
    report_fatal_error_if_not(not this->get_du(du_idx2).try_pop_dl_pdu(f1ap_pdu2),
                              "there are still F1AP DL messages to pop from DU2");
    report_fatal_error_if_not(not this->get_du(du_idx3).try_pop_dl_pdu(f1ap_pdu3),
                              "there are still F1AP DL messages to pop from DU3");
    report_fatal_error_if_not(not this->get_cu_up(cu_up_idx).try_pop_rx_pdu(e1ap_pdu),
                              "there are still E1AP messages to pop from CU-UP");

    // Inject F1AP Positioning Measurement Response containing a UL Angle of Arrival measurement result.
    get_du(du_id).push_ul_pdu(test_helpers::generate_positioning_measurement_response_with_aoa(
        lmf_meas, ran_meas, trp_ids, azimuth_ao_a, transaction_id));

    return true;
  }

  [[nodiscard]] bool await_nrppa_measurement_response(unsigned trp_meas_resp_list_size)
  {
    // Wait for NRPPa Measurement Response.
    report_fatal_error_if_not(this->wait_for_ngap_tx_pdu(ngap_pdu),
                              "Failed to receive UL non UE Associated NRPPa Transport");
    report_fatal_error_if_not(test_helpers::is_valid_ul_non_ue_associated_nrppa_transport(ngap_pdu),
                              "Invalid UL non UE associated NRPPa Transport");
    asn1::nrppa::nr_ppa_pdu_c nrppa_pdu = get_nrppa_pdu(ngap_pdu);
    report_fatal_error_if_not(test_helpers::is_valid_nrppa_measurement_response(nrppa_pdu),
                              "Invalid NRPPa Measurement Response");
    report_fatal_error_if_not(nrppa_pdu.successful_outcome().value.meas_resp()->trp_meas_resp_list.size() ==
                                  trp_meas_resp_list_size,
                              "Invalid size of TRP Measurement Response List");

    return true;
  }

  [[nodiscard]] bool send_f1ap_positioning_measurement_failure(unsigned du_id)
  {
    report_fatal_error_if_not(not this->get_amf().try_pop_rx_pdu(ngap_pdu),
                              "there are still NGAP messages to pop from AMF");
    report_fatal_error_if_not(not this->get_du(du_idx).try_pop_dl_pdu(f1ap_pdu),
                              "there are still F1AP DL messages to pop from DU");
    report_fatal_error_if_not(not this->get_du(du_idx2).try_pop_dl_pdu(f1ap_pdu2),
                              "there are still F1AP DL messages to pop from DU2");
    report_fatal_error_if_not(not this->get_du(du_idx3).try_pop_dl_pdu(f1ap_pdu3),
                              "there are still F1AP DL messages to pop from DU3");
    report_fatal_error_if_not(not this->get_cu_up(cu_up_idx).try_pop_rx_pdu(e1ap_pdu),
                              "there are still E1AP messages to pop from CU-UP");

    // Inject F1AP Positioning Measurement Failure.
    get_du(du_id).push_ul_pdu(test_helpers::generate_positioning_measurement_failure(lmf_meas_id, ran_meas_id));

    return true;
  }

  [[nodiscard]] bool await_nrppa_measurement_failure()
  {
    // Wait for NRPPa Measurement Failure.
    report_fatal_error_if_not(this->wait_for_ngap_tx_pdu(ngap_pdu),
                              "Failed to receive UL non UE Associated NRPPa Transport");
    report_fatal_error_if_not(test_helpers::is_valid_ul_non_ue_associated_nrppa_transport(ngap_pdu),
                              "Invalid UL non UE associated NRPPa Transport");
    report_fatal_error_if_not(test_helpers::is_valid_nrppa_measurement_failure(get_nrppa_pdu(ngap_pdu)),
                              "Invalid NRPPa Measurement Failure");

    return true;
  }

  [[nodiscard]] bool send_measurement_request_and_await_measurement_failure()
  {
    report_fatal_error_if_not(not this->get_amf().try_pop_rx_pdu(ngap_pdu),
                              "there are still NGAP messages to pop from AMF");
    report_fatal_error_if_not(not this->get_du(du_idx).try_pop_dl_pdu(f1ap_pdu),
                              "there are still F1AP DL messages to pop from DU");
    report_fatal_error_if_not(not this->get_du(du_idx2).try_pop_dl_pdu(f1ap_pdu2),
                              "there are still F1AP DL messages to pop from DU2");
    report_fatal_error_if_not(not this->get_du(du_idx3).try_pop_dl_pdu(f1ap_pdu3),
                              "there are still F1AP DL messages to pop from DU3");
    report_fatal_error_if_not(not this->get_cu_up(cu_up_idx).try_pop_rx_pdu(e1ap_pdu),
                              "there are still E1AP messages to pop from CU-UP");

    // Inject measurement request and wait for measurement failure.
    get_amf().push_tx_pdu(generate_valid_dl_non_ue_associated_nrppa_transport_message(
        generate_valid_nrppa_measurement_request(lmf_meas_id)));
    report_fatal_error_if_not(this->wait_for_ngap_tx_pdu(ngap_pdu),
                              "Failed to receive UL non UE Associated NRPPa Transport");
    report_fatal_error_if_not(test_helpers::is_valid_ul_non_ue_associated_nrppa_transport(ngap_pdu),
                              "Invalid UL non UE associated NRPPa Transport");
    report_fatal_error_if_not(test_helpers::is_valid_nrppa_measurement_failure(get_nrppa_pdu(ngap_pdu)),
                              "Invalid Measurement Failure");

    return true;
  }

  [[nodiscard]] bool run_successful_positioning_procedure(const ue_context* ue_ctxt,
                                                          lmf_meas_id_t     lmf_meas,
                                                          ran_meas_id_t     ran_meas,
                                                          unsigned          f1ap_transaction_id)
  {
    // Inject positioning information request and await F1AP positioning information request.
    if (!send_nrppa_positioning_information_request_and_await_f1ap_positioning_information_request(ue_ctxt)) {
      return false;
    }

    // Inject F1AP positioning information response and await NRPPa positioning information response.
    if (!send_f1ap_positioning_information_response_and_await_nrppa_positioning_information_response(ue_ctxt)) {
      return false;
    }

    // Inject positioning activation request and await F1AP positioning activation request.
    if (!send_nrppa_positioning_activation_request_and_await_f1ap_positioning_activation_request(ue_ctxt)) {
      return false;
    }

    // Inject F1AP positioning activation response and await NRPPa positioning activation response.
    if (!send_f1ap_positioning_activation_response_and_await_nrppa_positioning_activation_response(ue_ctxt)) {
      return false;
    }

    // Inject measurement request.
    if (!send_nrppa_measurement_request(lmf_meas)) {
      return false;
    }

    // Await F1AP positioning measurement request.
    if (!await_f1ap_positioning_measurement_request(du_idx, f1ap_pdu)) {
      return false;
    }

    // Inject F1AP positioning measurement response.
    if (!send_f1ap_positioning_measurement_response(du_idx,
                                                    lmf_meas,
                                                    ran_meas,
                                                    {uint_to_trp_id(1), uint_to_trp_id(2), uint_to_trp_id(3)},
                                                    f1ap_transaction_id)) {
      return false;
    }

    // Await F1AP positioning measurement request.
    if (!await_f1ap_positioning_measurement_request(du_idx2, f1ap_pdu2)) {
      return false;
    }

    // Inject F1AP positioning measurement response.
    if (!send_f1ap_positioning_measurement_response(du_idx2,
                                                    lmf_meas,
                                                    ran_meas,
                                                    {uint_to_trp_id(4), uint_to_trp_id(5), uint_to_trp_id(6)},
                                                    f1ap_transaction_id)) {
      return false;
    }

    // Await F1AP positioning measurement request.
    if (!await_f1ap_positioning_measurement_request(du_idx3, f1ap_pdu3)) {
      return false;
    }

    // Inject F1AP positioning measurement response.
    if (!send_f1ap_positioning_measurement_response(du_idx3,
                                                    lmf_meas,
                                                    ran_meas,
                                                    {uint_to_trp_id(7), uint_to_trp_id(8), uint_to_trp_id(9)},
                                                    f1ap_transaction_id)) {
      return false;
    }

    // Await NRPPa measurement response.
    if (!await_nrppa_measurement_response(9)) {
      return false;
    }

    return true;
  }

  unsigned du_idx    = 0;
  unsigned du_idx2   = 0;
  unsigned du_idx3   = 0;
  unsigned cu_up_idx = 0;

  // UE 1.
  lmf_meas_id_t lmf_meas_id = lmf_meas_id_t::min;
  ran_meas_id_t ran_meas_id = ran_meas_id_t::min;

  gnb_du_ue_f1ap_id_t    du_ue_id      = gnb_du_ue_f1ap_id_t::min;
  rnti_t                 crnti         = to_rnti(0x4601);
  amf_ue_id_t            amf_ue_id     = amf_ue_id_t::min;
  gnb_cu_up_ue_e1ap_id_t cu_up_e1ap_id = gnb_cu_up_ue_e1ap_id_t::min;

  lmf_ue_meas_id_t lmf_ue_meas_id = lmf_ue_meas_id_t::min;
  ran_ue_meas_id_t ran_ue_meas_id = ran_ue_meas_id_t::min;

  // UE 2.
  lmf_meas_id_t lmf_meas_id_2 = uint_to_lmf_meas_id(2);
  ran_meas_id_t ran_meas_id_2 = uint_to_ran_meas_id(2);

  gnb_du_ue_f1ap_id_t    du_ue_id_2      = int_to_gnb_du_ue_f1ap_id(1);
  rnti_t                 crnti_2         = to_rnti(0x4602);
  amf_ue_id_t            amf_ue_id_2     = uint_to_amf_ue_id(1);
  gnb_cu_up_ue_e1ap_id_t cu_up_e1ap_id_2 = int_to_gnb_cu_up_ue_e1ap_id(1);

  lmf_ue_meas_id_t lmf_ue_meas_id_2 = uint_to_lmf_ue_meas_id(2);
  ran_ue_meas_id_t ran_ue_meas_id_2 = uint_to_ran_ue_meas_id(2);

  std::map<gnb_du_ue_f1ap_id_t, const ue_context*> test_ues;

  pdu_session_id_t psi = uint_to_pdu_session_id(1);
  qos_flow_id_t    qfi = uint_to_qos_flow_id(1);

  ngap_message ngap_pdu;
  f1ap_message f1ap_pdu;
  f1ap_message f1ap_pdu2;
  f1ap_message f1ap_pdu3;
  e1ap_message e1ap_pdu;
};

//----------------------------------------------------------------------------------//
// E-CID measurement initiation test                                                //
//----------------------------------------------------------------------------------//

TEST_F(
    cu_cp_nrppa_test,
    when_valid_e_cid_measurement_initiation_request_is_received_but_no_measurements_are_available_then_e_cid_measurement_initiation_failure_is_send)
{
  // Attach UE.
  ASSERT_TRUE(attach_ue(du_ue_id, crnti, amf_ue_id, cu_up_e1ap_id));

  // Inject E-CID measurement initiation request and await E-CID measurement initiation failure.
  ASSERT_TRUE(send_e_cid_measurement_initiation_request_and_await_e_cid_measurement_initiation_failure(
      test_ues.at(du_ue_id), generate_valid_nrppa_e_cid_measurement_initiation_request(lmf_ue_meas_id)));
}

TEST_F(
    cu_cp_nrppa_test,
    when_valid_e_cid_measurement_initiation_request_with_only_unsupported_measurement_quantities_is_received_then_e_cid_measurement_initiation_failure_is_send)
{
  // Attach UE.
  ASSERT_TRUE(attach_ue(du_ue_id, crnti, amf_ue_id, cu_up_e1ap_id));

  // Inject E-CID measurement initiation request and await E-CID measurement initiation failure.
  ASSERT_TRUE(send_e_cid_measurement_initiation_request_and_await_e_cid_measurement_initiation_failure(
      test_ues.at(du_ue_id),
      generate_valid_nrppa_e_cid_measurement_initiation_request(
          lmf_ue_meas_id,
          {{nrppa_meas_quantities_item{nrppa_meas_quantities_value::angle_of_arrival},
            nrppa_meas_quantities_item{nrppa_meas_quantities_value::timing_advance_type1}}})));
}

TEST_F(cu_cp_nrppa_test,
       when_valid_e_cid_measurement_initiation_request_is_received_then_e_cid_measurement_initiation_response_is_send)
{
  // Attach UE.
  ASSERT_TRUE(attach_ue(du_ue_id, crnti, amf_ue_id, cu_up_e1ap_id));

  // Inject measurement report.
  ASSERT_TRUE(
      send_rrc_measurement_report(test_ues.at(du_ue_id)->cu_ue_id.value(), test_ues.at(du_ue_id)->du_ue_id.value()));

  // Inject E-CID measurement initiation request and await E-CID measurement initiation response.
  ASSERT_TRUE(send_e_cid_measurement_initiation_request_and_await_e_cid_measurement_initiation_response(
      test_ues.at(du_ue_id), generate_valid_nrppa_e_cid_measurement_initiation_request(lmf_ue_meas_id)));
}

TEST_F(
    cu_cp_nrppa_test,
    when_multiple_valid_e_cid_measurement_initiation_requests_are_received_then_e_cid_measurement_initiation_responses_are_send)
{
  // Attach UE.
  ASSERT_TRUE(attach_ue(du_ue_id, crnti, amf_ue_id, cu_up_e1ap_id));

  // Inject measurement report.
  ASSERT_TRUE(
      send_rrc_measurement_report(test_ues.at(du_ue_id)->cu_ue_id.value(), test_ues.at(du_ue_id)->du_ue_id.value()));

  // Inject E-CID measurement initiation request and await E-CID measurement initiation response.
  ASSERT_TRUE(send_e_cid_measurement_initiation_request_and_await_e_cid_measurement_initiation_response(
      test_ues.at(du_ue_id), generate_valid_nrppa_e_cid_measurement_initiation_request(lmf_ue_meas_id_t::min)));

  // Inject second E-CID measurement initiation request and await E-CID measurement initiation response.
  ASSERT_TRUE(send_e_cid_measurement_initiation_request_and_await_e_cid_measurement_initiation_response(
      test_ues.at(du_ue_id), generate_valid_nrppa_e_cid_measurement_initiation_request(lmf_ue_meas_id_t::min)));
}

TEST_F(
    cu_cp_nrppa_test,
    when_periodic_e_cid_measurement_is_requested_but_no_measurements_are_available_then_e_cid_measurement_reports_are_not_send)
{
  // Attach UE.
  ASSERT_TRUE(attach_ue(du_ue_id, crnti, amf_ue_id, cu_up_e1ap_id));

  // Inject E-CID measurement initiation request and await E-CID measurement initiation response.
  ASSERT_TRUE(send_e_cid_measurement_initiation_request_and_await_e_cid_measurement_initiation_response(
      test_ues.at(du_ue_id),
      generate_valid_nrppa_e_cid_measurement_initiation_request_with_periodic_reports(lmf_ue_meas_id)));

  // Await periodic measurement report.
  ASSERT_FALSE(tick_until(
      std::chrono::milliseconds{500}, []() { return false; }, false))
      << "Periodic E-CID Measurement Report was not received";

  ASSERT_FALSE(this->wait_for_ngap_tx_pdu(ngap_pdu));
}

TEST_F(
    cu_cp_nrppa_test,
    when_periodic_e_cid_measurement_is_requested_and_measurements_are_available_then_e_cid_measurement_reports_are_send)
{
  // Attach UE.
  ASSERT_TRUE(attach_ue(du_ue_id, crnti, amf_ue_id, cu_up_e1ap_id));

  // Inject measurement report.
  ASSERT_TRUE(
      send_rrc_measurement_report(test_ues.at(du_ue_id)->cu_ue_id.value(), test_ues.at(du_ue_id)->du_ue_id.value()));

  // Inject E-CID measurement initiation request and await E-CID measurement initiation response.
  ASSERT_TRUE(send_e_cid_measurement_initiation_request_and_await_e_cid_measurement_initiation_response(
      test_ues.at(du_ue_id),
      generate_valid_nrppa_e_cid_measurement_initiation_request_with_periodic_reports(lmf_ue_meas_id)));

  // Await periodic measurement report.
  ASSERT_TRUE(tick_until(
      std::chrono::milliseconds{500}, [this]() { return await_e_cid_measurement_report(); }, false))
      << "Periodic E-CID Measurement Report was not received";
}

TEST_F(cu_cp_nrppa_test, when_e_cid_termination_command_is_received_then_periodic_e_cid_measurement_reports_are_stopped)
{
  // Attach UE.
  ASSERT_TRUE(attach_ue(du_ue_id, crnti, amf_ue_id, cu_up_e1ap_id));

  // Inject measurement report.
  ASSERT_TRUE(
      send_rrc_measurement_report(test_ues.at(du_ue_id)->cu_ue_id.value(), test_ues.at(du_ue_id)->du_ue_id.value()));

  // Inject E-CID measurement initiation request and await E-CID measurement initiation response.
  ASSERT_TRUE(send_e_cid_measurement_initiation_request_and_await_e_cid_measurement_initiation_response(
      test_ues.at(du_ue_id),
      generate_valid_nrppa_e_cid_measurement_initiation_request_with_periodic_reports(lmf_ue_meas_id)));

  // Await periodic measurement report.
  ASSERT_TRUE(tick_until(
      std::chrono::milliseconds{500}, [this]() { return await_e_cid_measurement_report(); }, false))
      << "Periodic E-CID Measurement Report was not received";

  // Inject E-CID termination command.
  ASSERT_TRUE(send_e_cid_measurement_termination_command(test_ues.at(du_ue_id)));

  // No periodic measurement report should be sent.
  ASSERT_FALSE(tick_until(
      std::chrono::milliseconds{500}, []() { return false; }, false))
      << "Periodic E-CID Measurement Report was not received";
  ASSERT_FALSE(this->wait_for_ngap_tx_pdu(ngap_pdu));
}

TEST_F(cu_cp_nrppa_test, when_e_cid_termination_command_for_unknown_ue_is_received_then_command_is_ignored)
{
  // Attach UE.
  ASSERT_TRUE(attach_ue(du_ue_id, crnti, amf_ue_id, cu_up_e1ap_id));

  // Inject measurement report.
  ASSERT_TRUE(
      send_rrc_measurement_report(test_ues.at(du_ue_id)->cu_ue_id.value(), test_ues.at(du_ue_id)->du_ue_id.value()));

  // Inject E-CID termination command.
  ASSERT_TRUE(send_e_cid_measurement_termination_command(test_ues.at(du_ue_id)));

  // No periodic measurement report should be sent.
  ASSERT_FALSE(tick_until(
      std::chrono::milliseconds{500}, []() { return false; }, false))
      << "Periodic E-CID Measurement Report was not received";
  ASSERT_FALSE(this->wait_for_ngap_tx_pdu(ngap_pdu));
}

//----------------------------------------------------------------------------------//
// TRP Information Exchange test                                                    //
//----------------------------------------------------------------------------------//

TEST_F(cu_cp_nrppa_test,
       when_valid_trp_information_responses_are_received_from_the_du_then_response_is_forwarded_to_lmf)
{
  // Inject TRP information request and await F1AP TRP information request.
  ASSERT_TRUE(send_nrppa_trp_information_request());
  // Await F1AP TRP information request on DU 1.
  ASSERT_TRUE(await_f1ap_trp_information_request(du_idx, f1ap_pdu));
  // Inject F1AP TRP information response from DU 1.
  ASSERT_TRUE(send_f1ap_trp_information_response(du_idx, {uint_to_trp_id(1)}));
  // Await F1AP TRP information request on DU 2.
  ASSERT_TRUE(await_f1ap_trp_information_request(du_idx2, f1ap_pdu2));
  // Inject F1AP TRP information response from DU 2.
  ASSERT_TRUE(send_f1ap_trp_information_response(du_idx2, {uint_to_trp_id(2)}));
  // Await F1AP TRP information request on DU 3.
  ASSERT_TRUE(await_f1ap_trp_information_request(du_idx3, f1ap_pdu3));
  // Inject F1AP TRP information response from DU 3.
  ASSERT_TRUE(send_f1ap_trp_information_response(du_idx3, {uint_to_trp_id(3)}));

  // Await NRPPa TRP information response.
  ASSERT_TRUE(await_nrppa_trp_information_response(3));
}

TEST_F(cu_cp_nrppa_test, when_valid_trp_information_failures_are_received_from_the_dus_then_failure_is_forwarded_to_lmf)
{
  // Inject TRP information request and await F1AP TRP information request.
  ASSERT_TRUE(send_nrppa_trp_information_request());
  // Await F1AP TRP information request on DU 1.
  ASSERT_TRUE(await_f1ap_trp_information_request(du_idx, f1ap_pdu));
  // Inject F1AP TRP information failure from DU 1.
  ASSERT_TRUE(send_f1ap_trp_information_failure(du_idx));
  // Await F1AP TRP information request on DU 2.
  ASSERT_TRUE(await_f1ap_trp_information_request(du_idx2, f1ap_pdu2));
  // Inject F1AP TRP information failure from DU 2.
  ASSERT_TRUE(send_f1ap_trp_information_failure(du_idx2));
  // Await F1AP TRP information request on DU 3.
  ASSERT_TRUE(await_f1ap_trp_information_request(du_idx3, f1ap_pdu3));
  // Inject F1AP TRP information failure from DU 3.
  ASSERT_TRUE(send_f1ap_trp_information_failure(du_idx3));

  // Await NRPPa TRP information failure.
  ASSERT_TRUE(await_nrppa_trp_information_failure());
}

TEST_F(cu_cp_nrppa_test,
       when_at_least_one_valid_trp_information_response_is_received_from_the_dus_then_response_is_forwarded_to_lmf)
{
  // Inject TRP information request and await F1AP TRP information request.
  ASSERT_TRUE(send_nrppa_trp_information_request());
  // Await F1AP TRP information request on DU 1.
  ASSERT_TRUE(await_f1ap_trp_information_request(du_idx, f1ap_pdu));
  // Inject F1AP TRP information response from DU 1.
  ASSERT_TRUE(send_f1ap_trp_information_response(du_idx, {uint_to_trp_id(1)}));
  // Await F1AP TRP information request on DU 2.
  ASSERT_TRUE(await_f1ap_trp_information_request(du_idx2, f1ap_pdu2));
  // Inject F1AP TRP information failure from DU 2.
  ASSERT_TRUE(send_f1ap_trp_information_failure(du_idx2));
  // Await F1AP TRP information request on DU 3.
  ASSERT_TRUE(await_f1ap_trp_information_request(du_idx3, f1ap_pdu3));
  // Inject F1AP TRP information failure from DU 3.
  ASSERT_TRUE(send_f1ap_trp_information_failure(du_idx3));

  // Await NRPPa TRP information response.
  ASSERT_TRUE(await_nrppa_trp_information_response(1));
}

//----------------------------------------------------------------------------------//
// Positioning Information Exchange test                                            //
//----------------------------------------------------------------------------------//

TEST_F(cu_cp_nrppa_test,
       when_valid_positioning_information_response_is_received_from_the_du_then_response_is_forwarded_to_lmf)
{
  // Handle TRP information procedure.
  ASSERT_TRUE(run_successful_trp_information_procedure());

  // Attach UE.
  ASSERT_TRUE(attach_ue(du_ue_id, crnti, amf_ue_id, cu_up_e1ap_id));

  // Inject positioning information request and await F1AP positioning information request.
  ASSERT_TRUE(
      send_nrppa_positioning_information_request_and_await_f1ap_positioning_information_request(test_ues.at(du_ue_id)));

  // Inject F1AP positioning information response and await NRPPa positioning information response.
  ASSERT_TRUE(send_f1ap_positioning_information_response_and_await_nrppa_positioning_information_response(
      test_ues.at(du_ue_id)));
}

TEST_F(cu_cp_nrppa_test,
       when_valid_positioning_information_failure_is_received_from_the_du_then_failure_is_forwarded_to_lmf)
{
  // Handle TRP information procedure.
  ASSERT_TRUE(run_successful_trp_information_procedure());

  // Attach UE.
  ASSERT_TRUE(attach_ue(du_ue_id, crnti, amf_ue_id, cu_up_e1ap_id));

  // Inject positioning information request and await F1AP positioning information request.
  ASSERT_TRUE(
      send_nrppa_positioning_information_request_and_await_f1ap_positioning_information_request(test_ues.at(du_ue_id)));

  // Inject F1AP positioning information failures and await NRPPa positioning information failure.
  ASSERT_TRUE(send_f1ap_positioning_information_response_and_await_nrppa_positioning_information_failure(
      test_ues.at(du_ue_id)));
}

TEST_F(cu_cp_nrppa_test, when_trp_information_is_not_available_then_positioning_information_request_is_rejected)
{
  // Attach UE.
  ASSERT_TRUE(attach_ue(du_ue_id, crnti, amf_ue_id, cu_up_e1ap_id));

  // Inject positioning information request and await NRPPA positioning information failure.
  ASSERT_TRUE(send_nrppa_positioning_information_request_and_await_nrppa_positioning_information_failure(
      test_ues.at(du_ue_id)));
}

//----------------------------------------------------------------------------------//
// Positioning Activation test                                                      //
//----------------------------------------------------------------------------------//

TEST_F(cu_cp_nrppa_test,
       when_valid_positioning_activation_response_is_received_from_the_du_then_response_is_forwarded_to_lmf)
{
  // Handle TRP information procedure.
  ASSERT_TRUE(run_successful_trp_information_procedure());

  // Attach UE.
  ASSERT_TRUE(attach_ue(du_ue_id, crnti, amf_ue_id, cu_up_e1ap_id));

  // Inject positioning information request and await F1AP positioning information request.
  ASSERT_TRUE(
      send_nrppa_positioning_information_request_and_await_f1ap_positioning_information_request(test_ues.at(du_ue_id)));

  // Inject F1AP positioning information response and await NRPPa positioning information response.
  ASSERT_TRUE(send_f1ap_positioning_information_response_and_await_nrppa_positioning_information_response(
      test_ues.at(du_ue_id)));

  // Inject positioning activation request and await F1AP positioning activation request.
  ASSERT_TRUE(
      send_nrppa_positioning_activation_request_and_await_f1ap_positioning_activation_request(test_ues.at(du_ue_id)));

  // Inject F1AP positioning activation response and await NRPPa positioning activation response.
  ASSERT_TRUE(
      send_f1ap_positioning_activation_response_and_await_nrppa_positioning_activation_response(test_ues.at(du_ue_id)));
}

TEST_F(cu_cp_nrppa_test,
       when_valid_positioning_activation_failure_is_received_from_the_du_then_failure_is_forwarded_to_lmf)
{
  // Handle TRP information procedure.
  ASSERT_TRUE(run_successful_trp_information_procedure());

  // Attach UE.
  ASSERT_TRUE(attach_ue(du_ue_id, crnti, amf_ue_id, cu_up_e1ap_id));

  // Inject positioning information request and await F1AP positioning information request.
  ASSERT_TRUE(
      send_nrppa_positioning_information_request_and_await_f1ap_positioning_information_request(test_ues.at(du_ue_id)));

  // Inject F1AP positioning information response and await NRPPa positioning information response.
  ASSERT_TRUE(send_f1ap_positioning_information_response_and_await_nrppa_positioning_information_response(
      test_ues.at(du_ue_id)));

  // Inject positioning activation request and await F1AP positioning activation request.
  ASSERT_TRUE(
      send_nrppa_positioning_activation_request_and_await_f1ap_positioning_activation_request(test_ues.at(du_ue_id)));

  // Inject F1AP positioning activation failures and await NRPPa positioning activation failure.
  ASSERT_TRUE(
      send_f1ap_positioning_activation_failure_and_await_nrppa_positioning_activation_failure(test_ues.at(du_ue_id)));
}

TEST_F(cu_cp_nrppa_test, when_trp_information_is_not_available_then_positioning_activation_request_is_rejected)
{
  // Attach UE.
  ASSERT_TRUE(attach_ue(du_ue_id, crnti, amf_ue_id, cu_up_e1ap_id));

  // Inject positioning activation request and await NRPPA positioning activation failure.
  ASSERT_TRUE(
      send_nrppa_positioning_activation_request_and_await_nrppa_positioning_activation_failure(test_ues.at(du_ue_id)));
}

//----------------------------------------------------------------------------------//
// Measurement information test                                                     //
//----------------------------------------------------------------------------------//

TEST_F(cu_cp_nrppa_test, when_valid_measurement_responses_are_received_from_the_dus_then_response_is_forwarded_to_lmf)
{
  // Handle TRP information procedure.
  ASSERT_TRUE(run_successful_trp_information_procedure());

  // Attach UE.
  ASSERT_TRUE(attach_ue(du_ue_id, crnti, amf_ue_id, cu_up_e1ap_id));

  // Inject positioning information request and await F1AP positioning information request.
  ASSERT_TRUE(
      send_nrppa_positioning_information_request_and_await_f1ap_positioning_information_request(test_ues.at(du_ue_id)));

  // Inject F1AP positioning information response and await NRPPa positioning information response.
  ASSERT_TRUE(send_f1ap_positioning_information_response_and_await_nrppa_positioning_information_response(
      test_ues.at(du_ue_id)));

  // Inject positioning activation request and await F1AP positioning activation request.
  ASSERT_TRUE(
      send_nrppa_positioning_activation_request_and_await_f1ap_positioning_activation_request(test_ues.at(du_ue_id)));

  // Inject F1AP positioning activation response and await NRPPa positioning activation response.
  ASSERT_TRUE(
      send_f1ap_positioning_activation_response_and_await_nrppa_positioning_activation_response(test_ues.at(du_ue_id)));

  // Inject measurement request.
  ASSERT_TRUE(send_nrppa_measurement_request(lmf_meas_id));

  // Await F1AP positioning measurement request.
  ASSERT_TRUE(await_f1ap_positioning_measurement_request(du_idx, f1ap_pdu));

  // Inject F1AP positioning measurement response.
  ASSERT_TRUE(send_f1ap_positioning_measurement_response(
      du_idx, lmf_meas_id, ran_meas_id, {uint_to_trp_id(1), uint_to_trp_id(2), uint_to_trp_id(3)}));

  // Await F1AP positioning measurement request.
  ASSERT_TRUE(await_f1ap_positioning_measurement_request(du_idx2, f1ap_pdu2));

  // Inject F1AP positioning measurement response.
  ASSERT_TRUE(send_f1ap_positioning_measurement_response(
      du_idx2, lmf_meas_id, ran_meas_id, {uint_to_trp_id(4), uint_to_trp_id(5), uint_to_trp_id(6)}));

  // Await F1AP positioning measurement request.
  ASSERT_TRUE(await_f1ap_positioning_measurement_request(du_idx3, f1ap_pdu3));

  // Inject F1AP positioning measurement response.
  ASSERT_TRUE(send_f1ap_positioning_measurement_response(
      du_idx3, lmf_meas_id, ran_meas_id, {uint_to_trp_id(7), uint_to_trp_id(8), uint_to_trp_id(9)}));

  // Await NRPPa measurement response.
  ASSERT_TRUE(await_nrppa_measurement_response(9));
}

TEST_F(cu_cp_nrppa_test, when_valid_measurement_failures_are_received_from_the_dus_then_failure_is_forwarded_to_lmf)
{
  // Handle TRP information procedure.
  ASSERT_TRUE(run_successful_trp_information_procedure());

  // Attach UE.
  ASSERT_TRUE(attach_ue(du_ue_id, crnti, amf_ue_id, cu_up_e1ap_id));

  // Inject positioning information request and await F1AP positioning information request.
  ASSERT_TRUE(
      send_nrppa_positioning_information_request_and_await_f1ap_positioning_information_request(test_ues.at(du_ue_id)));

  // Inject F1AP positioning information response and await NRPPa positioning information response.
  ASSERT_TRUE(send_f1ap_positioning_information_response_and_await_nrppa_positioning_information_response(
      test_ues.at(du_ue_id)));

  // Inject positioning activation request and await F1AP positioning activation request.
  ASSERT_TRUE(
      send_nrppa_positioning_activation_request_and_await_f1ap_positioning_activation_request(test_ues.at(du_ue_id)));

  // Inject F1AP positioning activation failures and await NRPPa positioning activation failure.
  ASSERT_TRUE(
      send_f1ap_positioning_activation_failure_and_await_nrppa_positioning_activation_failure(test_ues.at(du_ue_id)));

  // Inject measurement request.
  ASSERT_TRUE(send_nrppa_measurement_request(lmf_meas_id));

  // Await F1AP positioning measurement request.
  ASSERT_TRUE(await_f1ap_positioning_measurement_request(du_idx, f1ap_pdu));

  // Inject F1AP positioning measurement failure.
  ASSERT_TRUE(send_f1ap_positioning_measurement_failure(du_idx));

  // Await F1AP positioning measurement request.
  ASSERT_TRUE(await_f1ap_positioning_measurement_request(du_idx2, f1ap_pdu2));

  // Inject F1AP positioning measurement failure.
  ASSERT_TRUE(send_f1ap_positioning_measurement_failure(du_idx2));

  // Await F1AP positioning measurement request.
  ASSERT_TRUE(await_f1ap_positioning_measurement_request(du_idx3, f1ap_pdu3));

  // Inject F1AP positioning measurement failure.
  ASSERT_TRUE(send_f1ap_positioning_measurement_failure(du_idx3));

  // Await NRPPa measurement failure.
  ASSERT_TRUE(await_nrppa_measurement_failure());
}

TEST_F(
    cu_cp_nrppa_test,
    when_at_least_one_valid_positioning_measurement_response_is_received_from_the_dus_then_response_is_forwarded_to_lmf)
{
  // Inject TRP information request and await F1AP TRP information request.
  ASSERT_TRUE(send_nrppa_trp_information_request());
  // Await F1AP TRP information request on DU 1.
  ASSERT_TRUE(await_f1ap_trp_information_request(du_idx, f1ap_pdu));
  // Inject F1AP TRP information response from DU 1.
  ASSERT_TRUE(send_f1ap_trp_information_response(du_idx, {uint_to_trp_id(1), uint_to_trp_id(2), uint_to_trp_id(3)}));
  // Await F1AP TRP information request on DU 2.
  ASSERT_TRUE(await_f1ap_trp_information_request(du_idx2, f1ap_pdu2));
  // Inject F1AP TRP information failure from DU 2.
  ASSERT_TRUE(send_f1ap_trp_information_failure(du_idx2));
  // Await F1AP TRP information request on DU 3.
  ASSERT_TRUE(await_f1ap_trp_information_request(du_idx3, f1ap_pdu3));
  // Inject F1AP TRP information failure from DU 3.
  ASSERT_TRUE(send_f1ap_trp_information_failure(du_idx3));
  // Await NRPPa TRP information response.
  ASSERT_TRUE(await_nrppa_trp_information_response(3));

  // Attach UE.
  ASSERT_TRUE(attach_ue(du_ue_id, crnti, amf_ue_id, cu_up_e1ap_id));

  // Inject positioning information request and await F1AP positioning information request.
  ASSERT_TRUE(
      send_nrppa_positioning_information_request_and_await_f1ap_positioning_information_request(test_ues.at(du_ue_id)));

  // Inject F1AP positioning information response and await NRPPa positioning information response.
  ASSERT_TRUE(send_f1ap_positioning_information_response_and_await_nrppa_positioning_information_response(
      test_ues.at(du_ue_id)));

  // Inject positioning activation request and await F1AP positioning activation request.
  ASSERT_TRUE(
      send_nrppa_positioning_activation_request_and_await_f1ap_positioning_activation_request(test_ues.at(du_ue_id)));

  // Inject F1AP positioning activation response and await NRPPa positioning activation response.
  ASSERT_TRUE(
      send_f1ap_positioning_activation_response_and_await_nrppa_positioning_activation_response(test_ues.at(du_ue_id)));

  // Inject measurement request.
  ASSERT_TRUE(send_nrppa_measurement_request(lmf_meas_id, {uint_to_trp_id(1), uint_to_trp_id(2), uint_to_trp_id(3)}));

  // Await F1AP positioning measurement request.
  ASSERT_TRUE(await_f1ap_positioning_measurement_request(du_idx, f1ap_pdu));

  // Inject F1AP positioning measurement response.
  ASSERT_TRUE(send_f1ap_positioning_measurement_response(
      du_idx, lmf_meas_id, ran_meas_id, {uint_to_trp_id(1), uint_to_trp_id(2), uint_to_trp_id(3)}));

  // Await NRPPa measurement response.
  ASSERT_TRUE(await_nrppa_measurement_response(3));
}

TEST_F(cu_cp_nrppa_test,
       when_valid_ul_angle_of_arrival_measurement_response_is_received_from_the_du_then_response_is_forwarded_to_lmf)
{
  // Handle TRP information procedure.
  ASSERT_TRUE(run_successful_trp_information_procedure());

  // Attach UE.
  ASSERT_TRUE(attach_ue(du_ue_id, crnti, amf_ue_id, cu_up_e1ap_id));

  // Inject positioning information request and await F1AP positioning information request.
  ASSERT_TRUE(
      send_nrppa_positioning_information_request_and_await_f1ap_positioning_information_request(test_ues.at(du_ue_id)));

  // Inject F1AP positioning information response and await NRPPa positioning information response.
  ASSERT_TRUE(send_f1ap_positioning_information_response_and_await_nrppa_positioning_information_response(
      test_ues.at(du_ue_id)));

  // Inject positioning activation request and await F1AP positioning activation request.
  ASSERT_TRUE(
      send_nrppa_positioning_activation_request_and_await_f1ap_positioning_activation_request(test_ues.at(du_ue_id)));

  // Inject F1AP positioning activation response and await NRPPa positioning activation response.
  ASSERT_TRUE(
      send_f1ap_positioning_activation_response_and_await_nrppa_positioning_activation_response(test_ues.at(du_ue_id)));

  // Inject measurement request, requesting UL Angle of Arrival for the TRPs hosted by DU 1.
  ASSERT_TRUE(send_nrppa_measurement_request(
      lmf_meas_id, {uint_to_trp_id(1), uint_to_trp_id(2), uint_to_trp_id(3)}, {{trp_meas_quantities_item_t::ul_aoa}}));

  // Await F1AP positioning measurement request.
  ASSERT_TRUE(await_f1ap_positioning_measurement_request(du_idx, f1ap_pdu));

  // Inject F1AP positioning measurement response with a UL Angle of Arrival measurement result.
  constexpr uint16_t azimuth_ao_a = 1800;
  ASSERT_TRUE(send_f1ap_positioning_measurement_response_with_aoa(
      du_idx, lmf_meas_id, ran_meas_id, {uint_to_trp_id(1), uint_to_trp_id(2), uint_to_trp_id(3)}, azimuth_ao_a));

  // Await NRPPa measurement response.
  ASSERT_TRUE(await_nrppa_measurement_response(3));

  // Verify that the UL Angle of Arrival measurement result was correctly forwarded to the LMF.
  asn1::nrppa::nr_ppa_pdu_c nrppa_pdu          = get_nrppa_pdu(ngap_pdu);
  const auto&               trp_meas_resp_list = nrppa_pdu.successful_outcome().value.meas_resp()->trp_meas_resp_list;
  ASSERT_EQ(trp_meas_resp_list.size(), 3);
  for (const auto& trp_meas_resp_item : trp_meas_resp_list) {
    ASSERT_EQ(trp_meas_resp_item.meas_result.size(), 1);
    const auto& meas_result = trp_meas_resp_item.meas_result[0];
    ASSERT_EQ(meas_result.measured_results_value.type(),
              asn1::nrppa::trp_measured_results_value_c::types_opts::ul_angle_of_arrival);
    ASSERT_EQ(meas_result.measured_results_value.ul_angle_of_arrival().azimuth_ao_a, azimuth_ao_a);
  }
}

TEST_F(cu_cp_nrppa_test, when_trp_information_is_not_available_then_measurement_failure_is_sent)
{
  // Inject measurement request and await measurement failure.
  ASSERT_TRUE(send_measurement_request_and_await_measurement_failure());
}

//----------------------------------------------------------------------------------//
// Multi UE Measurement information test                                            //
//----------------------------------------------------------------------------------//

TEST_F(cu_cp_nrppa_test, when_measurement_requests_for_multiple_ues_are_received_then_responses_are_forwarded_to_lmf)
{
  // Handle TRP information procedure.
  ASSERT_TRUE(run_successful_trp_information_procedure());

  // Attach UE.
  ASSERT_TRUE(attach_ue(du_ue_id, crnti, amf_ue_id, cu_up_e1ap_id));

  // Attach UE 2.
  ASSERT_TRUE(attach_ue(du_ue_id_2, crnti_2, amf_ue_id_2, cu_up_e1ap_id_2));

  // Run positioning for UE 1.
  ASSERT_TRUE(run_successful_positioning_procedure(test_ues.at(du_ue_id), lmf_meas_id, ran_meas_id, 1));

  // Run positioning for UE 2.
  ASSERT_TRUE(run_successful_positioning_procedure(test_ues.at(du_ue_id_2), lmf_meas_id_2, ran_meas_id_2, 2));

  // Run positioning for UE 1.
  ASSERT_TRUE(run_successful_positioning_procedure(test_ues.at(du_ue_id), lmf_meas_id, ran_meas_id, 3));

  // Run positioning for UE 2.
  ASSERT_TRUE(run_successful_positioning_procedure(test_ues.at(du_ue_id_2), lmf_meas_id_2, ran_meas_id_2, 4));
}
