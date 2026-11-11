// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "cu_cp_test_environment.h"
#include "tests/ocudu_test_requirements.h"
#include "tests/test_doubles/e1ap/e1ap_cu_cp_test_messages.h"
#include "tests/test_doubles/f1ap/f1ap_test_message_validators.h"
#include "tests/test_doubles/f1ap/f1ap_test_messages.h"
#include "tests/test_doubles/rrc/rrc_test_messages.h"
#include "tests/unittests/cu_cp/test_helpers.h"
#include "tests/unittests/ngap/ngap_test_messages.h"
#include "ocudu/asn1/f1ap/f1ap_pdu_contents_ue.h"
#include "ocudu/asn1/rrc_nr/dl_dcch_msg.h"
#include "ocudu/cu_cp/cu_cp_command_handler.h"
#include "ocudu/e1ap/common/e1ap_message.h"
#include "ocudu/f1ap/f1ap_message.h"
#include "ocudu/ngap/ngap_message.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace ocucp;

/// Covers the measurement gap request for the location measurements of a UE, TS 38.331 section 5.5.6.
class cu_cp_location_measurement_indication_test : public cu_cp_test_environment, public ::testing::Test
{
public:
  cu_cp_location_measurement_indication_test() : cu_cp_test_environment(cu_cp_test_env_params{})
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
                          make_byte_buffer("00070e00cc6fcda5").value()));
    ue_ctx = this->find_ue_context(du_idx, du_ue_id);
    EXPECT_NE(ue_ctx, nullptr);
  }

  /// The packed MeasGapConfig the DU returns: an FR2 gap of 6 ms every 40 ms, or the release of the FR2 gap.
  static byte_buffer make_packed_meas_gap_cfg(bool release = false)
  {
    asn1::rrc_nr::meas_gap_cfg_s meas_gap_cfg;
    meas_gap_cfg.gap_fr2_present = true;
    if (release) {
      meas_gap_cfg.gap_fr2.set_release();
    } else {
      meas_gap_cfg.gap_fr2.set_setup();
      meas_gap_cfg.gap_fr2.setup().gap_offset = 7;
      meas_gap_cfg.gap_fr2.setup().mgl.value  = asn1::rrc_nr::gap_cfg_s::mgl_opts::ms6;
      meas_gap_cfg.gap_fr2.setup().mgrp.value = asn1::rrc_nr::gap_cfg_s::mgrp_opts::ms40;
      meas_gap_cfg.gap_fr2.setup().mgta.value = asn1::rrc_nr::gap_cfg_s::mgta_opts::ms0;
    }

    byte_buffer   pdu;
    asn1::bit_ref bref{pdu};
    EXPECT_EQ(asn1::pack(bref, meas_gap_cfg), asn1::OCUDUASN_SUCCESS);
    return pdu;
  }

  /// The UE sends a LocationMeasurementIndication. It indicates a start with \c info, or a stop without it.
  void send_location_measurement_indication(const std::optional<asn1::rrc_nr::location_meas_info_c>& info)
  {
    get_du(du_idx).push_ul_pdu(test_helpers::generate_ul_rrc_message_transfer(
        ue_ctx->du_ue_id.value(),
        ue_ctx->cu_ue_id.value(),
        srb_id_t::srb1,
        generate_protected_ul_dcch_pdu(test_helpers::create_location_meas_ind(info), ul_count++)));
  }

  /// \brief Waits for the UE Context Modification Request and checks its CU to DU RRC Information.
  ///
  /// The CU to DU RRC Information always carries the MeasConfig of the UE. It carries the LocationMeasurementInfo of
  /// the UE only when \c with_location_meas_info is true.
  [[nodiscard]] bool await_ue_context_modification_request(bool with_location_meas_info)
  {
    if (!this->wait_for_f1ap_tx_pdu(du_idx, f1ap_pdu)) {
      test_logger.error("Failed to receive UE Context Modification Request");
      return false;
    }
    if (!test_helpers::is_valid_ue_context_modification_request(f1ap_pdu)) {
      test_logger.error("Invalid UE Context Modification Request");
      return false;
    }

    const auto& req = f1ap_pdu.pdu.init_msg().value.ue_context_mod_request();
    if (!req->cu_to_du_rrc_info_present || req->cu_to_du_rrc_info.meas_cfg.empty()) {
      test_logger.error("UE Context Modification Request without MeasConfig");
      return false;
    }

    const bool has_location_meas_info =
        req->cu_to_du_rrc_info.ie_exts_present && req->cu_to_du_rrc_info.ie_exts.location_meas_info_present;
    if (!with_location_meas_info) {
      return !has_location_meas_info;
    }
    if (!has_location_meas_info) {
      test_logger.error("UE Context Modification Request without Location Measurement Information");
      return false;
    }

    // The DU receives the LocationMeasurementInfo of the UE unchanged.
    byte_buffer   expected_info;
    asn1::bit_ref bref{expected_info};
    if (asn1::pack(bref, test_helpers::create_nr_prs_location_meas_info()) != asn1::OCUDUASN_SUCCESS) {
      return false;
    }
    return req->cu_to_du_rrc_info.ie_exts.location_meas_info == expected_info;
  }

  /// The DU answers the UE Context Modification Request with \c meas_gap_cfg.
  void send_ue_context_modification_response(byte_buffer meas_gap_cfg)
  {
    f1ap_message resp = test_helpers::generate_ue_context_modification_response(
        ue_ctx->du_ue_id.value(), ue_ctx->cu_ue_id.value(), crnti, {}, {});
    resp.pdu.successful_outcome().value.ue_context_mod_resp()->du_to_cu_rrc_info.meas_gap_cfg = std::move(meas_gap_cfg);
    get_du(du_idx).push_ul_pdu(resp);
  }

  /// Waits for the RRC Reconfiguration that carries the measurement gap config, and returns it.
  [[nodiscard]] std::optional<asn1::rrc_nr::rrc_recfg_s> await_rrc_reconfiguration_with_meas_gap_cfg()
  {
    if (!this->wait_for_f1ap_tx_pdu(du_idx, f1ap_pdu) || !test_helpers::is_valid_dl_rrc_message_transfer(f1ap_pdu)) {
      test_logger.error("Failed to receive DL RRC Message Transfer");
      return std::nullopt;
    }
    byte_buffer dl_dcch_pdu = test_helpers::extract_dl_dcch_msg(test_helpers::get_rrc_container(f1ap_pdu));
    asn1::rrc_nr::dl_dcch_msg_s dl_dcch_msg;
    asn1::cbit_ref              bref{dl_dcch_pdu};
    if (asn1::unpack(dl_dcch_msg, bref) != asn1::OCUDUASN_SUCCESS ||
        dl_dcch_msg.msg.c1().type().value != asn1::rrc_nr::dl_dcch_msg_type_c::c1_c_::types_opts::rrc_recfg) {
      test_logger.error("DL RRC Message Transfer without RRC Reconfiguration");
      return std::nullopt;
    }
    const asn1::rrc_nr::rrc_recfg_ies_s& ies = dl_dcch_msg.msg.c1().rrc_recfg().crit_exts.rrc_recfg();
    if (!ies.meas_cfg_present || !ies.meas_cfg.meas_gap_cfg_present) {
      test_logger.error("RRC Reconfiguration without MeasGapConfig");
      return std::nullopt;
    }
    return dl_dcch_msg.msg.c1().rrc_recfg();
  }

  /// The UE completes the RRC Reconfiguration with \c transaction_id.
  void send_rrc_reconfiguration_complete(uint8_t transaction_id)
  {
    get_du(du_idx).push_ul_pdu(test_helpers::generate_ul_rrc_message_transfer(
        ue_ctx->du_ue_id.value(),
        ue_ctx->cu_ue_id.value(),
        srb_id_t::srb1,
        generate_rrc_reconfiguration_complete_pdu(transaction_id, ul_count++)));
  }

  /// The UE starts the location measurements, and the DU configures a measurement gap for them.
  [[nodiscard]] bool start_location_measurements()
  {
    send_location_measurement_indication(test_helpers::create_nr_prs_location_meas_info());
    if (!await_ue_context_modification_request(/* with_location_meas_info */ true)) {
      return false;
    }
    send_ue_context_modification_response(make_packed_meas_gap_cfg());
    std::optional<asn1::rrc_nr::rrc_recfg_s> rrc_recfg = await_rrc_reconfiguration_with_meas_gap_cfg();
    if (!rrc_recfg.has_value()) {
      return false;
    }
    send_rrc_reconfiguration_complete(rrc_recfg->rrc_transaction_id);
    return true;
  }

  unsigned du_idx    = 0;
  unsigned cu_up_idx = 0;

  gnb_du_ue_f1ap_id_t    du_ue_id      = gnb_du_ue_f1ap_id_t::min;
  rnti_t                 crnti         = to_rnti(0x4601);
  amf_ue_id_t            amf_ue_id     = amf_ue_id_t::min;
  gnb_cu_up_ue_e1ap_id_t cu_up_e1ap_id = gnb_cu_up_ue_e1ap_id_t::min;
  pdu_session_id_t       psi           = uint_to_pdu_session_id(1);
  qos_flow_id_t          qfi           = uint_to_qos_flow_id(1);

  const ue_context* ue_ctx = nullptr;

  /// COUNT of the next UL PDCP PDU on SRB1. The attach uses the ones before.
  uint8_t ul_count = 8;

  f1ap_message f1ap_pdu;
};

TEST_F(cu_cp_location_measurement_indication_test,
       when_du_configures_a_measurement_gap_then_rrc_reconfiguration_with_the_gap_is_sent_to_ue)
{
  OCUDU_TEST_REQUIREMENTS("MVP-FUNC-POS-16-3-b");

  send_location_measurement_indication(test_helpers::create_nr_prs_location_meas_info());
  ASSERT_TRUE(await_ue_context_modification_request(/* with_location_meas_info */ true));

  send_ue_context_modification_response(make_packed_meas_gap_cfg());

  // The CU-CP sends the gap to the UE in an RRC Reconfiguration.
  std::optional<asn1::rrc_nr::rrc_recfg_s> rrc_recfg = await_rrc_reconfiguration_with_meas_gap_cfg();
  ASSERT_TRUE(rrc_recfg.has_value());
  const asn1::rrc_nr::rrc_recfg_ies_s& ies = rrc_recfg->crit_exts.rrc_recfg();
  ASSERT_TRUE(ies.meas_cfg.meas_gap_cfg.gap_fr2_present);
  EXPECT_EQ(ies.meas_cfg.meas_gap_cfg.gap_fr2.setup().gap_offset, 7);
  // The RRC Reconfiguration also carries the cell group config the DU returned.
  ASSERT_TRUE(ies.non_crit_ext_present);
  EXPECT_FALSE(ies.non_crit_ext.master_cell_group.empty());

  // The UE completes the reconfiguration, and the procedure ends without further messages to the DU.
  send_rrc_reconfiguration_complete(rrc_recfg->rrc_transaction_id);
  ASSERT_FALSE(this->wait_for_f1ap_tx_pdu(du_idx, f1ap_pdu, std::chrono::milliseconds{100}));
  EXPECT_NE(this->find_ue_context(du_idx, du_ue_id), nullptr);
}

TEST_F(cu_cp_location_measurement_indication_test,
       when_ue_stops_location_measurements_then_du_decides_the_gaps_from_the_meas_config_only)
{
  OCUDU_TEST_REQUIREMENTS("MVP-FUNC-POS-16-3-b");

  ASSERT_TRUE(start_location_measurements());

  // The UE stops the location measurements. The DU receives the MeasConfig of the UE without the
  // LocationMeasurementInfo, and releases the measurement gap.
  send_location_measurement_indication(std::nullopt);
  ASSERT_TRUE(await_ue_context_modification_request(/* with_location_meas_info */ false));
  send_ue_context_modification_response(make_packed_meas_gap_cfg(/* release */ true));

  // The CU-CP sends the release of the gap to the UE.
  std::optional<asn1::rrc_nr::rrc_recfg_s> rrc_recfg = await_rrc_reconfiguration_with_meas_gap_cfg();
  ASSERT_TRUE(rrc_recfg.has_value());
  const asn1::rrc_nr::meas_gap_cfg_s& meas_gap_cfg = rrc_recfg->crit_exts.rrc_recfg().meas_cfg.meas_gap_cfg;
  ASSERT_TRUE(meas_gap_cfg.gap_fr2_present);
  EXPECT_EQ(meas_gap_cfg.gap_fr2.type().value, asn1::setup_release_opts::release);

  send_rrc_reconfiguration_complete(rrc_recfg->rrc_transaction_id);
  ASSERT_FALSE(this->wait_for_f1ap_tx_pdu(du_idx, f1ap_pdu, std::chrono::milliseconds{100}));
  EXPECT_NE(this->find_ue_context(du_idx, du_ue_id), nullptr);
}

TEST_F(cu_cp_location_measurement_indication_test,
       when_pdu_session_is_set_up_during_location_measurements_then_du_receives_the_location_meas_info_again)
{
  OCUDU_TEST_REQUIREMENTS("MVP-FUNC-POS-16-3-b");

  ASSERT_TRUE(start_location_measurements());

  // The AMF sets up a second PDU session for the UE.
  const pdu_session_id_t psi2 = uint_to_pdu_session_id(2);
  const qos_flow_id_t    qfi2 = uint_to_qos_flow_id(2);
  ASSERT_TRUE(send_pdu_session_resource_setup_request_and_await_bearer_context_modification_request(
      generate_valid_pdu_session_resource_setup_request_message(
          ue_ctx->amf_ue_id.value(), ue_ctx->ran_ue_id.value(), {{psi2, {pdu_session_type_t::ipv4, {{qfi2, 9}}}}}),
      cu_up_idx));
  get_cu_up(cu_up_idx).push_tx_pdu(
      generate_bearer_context_modification_response(ue_ctx->cu_cp_e1ap_id.value(),
                                                    ue_ctx->cu_up_e1ap_id.value(),
                                                    {{psi2, {drb_test_params{drb_id_t::drb2, qfi2}}}},
                                                    {}));

  // The UE Context Modification Request for the new DRB keeps the location measurements of the UE.
  ASSERT_TRUE(await_ue_context_modification_request(/* with_location_meas_info */ true));
}

TEST_F(cu_cp_location_measurement_indication_test,
       when_pdu_session_is_modified_during_location_measurements_then_du_receives_the_location_meas_info_again)
{
  OCUDU_TEST_REQUIREMENTS("MVP-FUNC-POS-16-3-b");

  ASSERT_TRUE(start_location_measurements());

  // The AMF adds a QoS flow to the PDU session of the UE.
  get_amf().push_tx_pdu(generate_valid_pdu_session_resource_modify_request_message(
      ue_ctx->amf_ue_id.value(), ue_ctx->ran_ue_id.value(), psi, {uint_to_qos_flow_id(2)}, {}));
  e1ap_message e1ap_pdu;
  ASSERT_TRUE(this->wait_for_e1ap_tx_pdu(cu_up_idx, e1ap_pdu));
  get_cu_up(cu_up_idx).push_tx_pdu(generate_bearer_context_modification_response(
      ue_ctx->cu_cp_e1ap_id.value(), ue_ctx->cu_up_e1ap_id.value(), {}, {{psi, drb_id_t::drb2}}));

  // The UE Context Modification Request for the new DRB keeps the location measurements of the UE.
  ASSERT_TRUE(await_ue_context_modification_request(/* with_location_meas_info */ true));
}

TEST_F(cu_cp_location_measurement_indication_test,
       when_ue_hands_over_during_location_measurements_then_target_du_receives_the_location_meas_info)
{
  OCUDU_TEST_REQUIREMENTS("MVP-FUNC-POS-16-3-b");

  ASSERT_TRUE(start_location_measurements());

  // The CU-CP hands the UE over to a new UE context on the same cell.
  get_cu_cp().get_command_handler().get_mobility_command_handler().trigger_handover(
      pci_t{0}, crnti, pci_t{0}, plmn_identity::test_value(), 7);

  // The UE Context Setup Request for the target UE context carries the location measurements of the UE.
  ASSERT_TRUE(this->wait_for_f1ap_tx_pdu(du_idx, f1ap_pdu));
  ASSERT_TRUE(test_helpers::is_valid_ue_context_setup_request(f1ap_pdu));
  const auto& req = f1ap_pdu.pdu.init_msg().value.ue_context_setup_request();
  ASSERT_TRUE(req->cu_to_du_rrc_info.ie_exts_present);
  ASSERT_TRUE(req->cu_to_du_rrc_info.ie_exts.location_meas_info_present);
  byte_buffer   expected_info;
  asn1::bit_ref bref{expected_info};
  ASSERT_EQ(asn1::pack(bref, test_helpers::create_nr_prs_location_meas_info()), asn1::OCUDUASN_SUCCESS);
  EXPECT_EQ(req->cu_to_du_rrc_info.ie_exts.location_meas_info, expected_info);
}

TEST_F(cu_cp_location_measurement_indication_test,
       when_ue_reestablishes_during_location_measurements_then_du_configures_the_measurement_gap_again)
{
  OCUDU_TEST_REQUIREMENTS("MVP-FUNC-POS-16-3-b");

  ASSERT_TRUE(start_location_measurements());

  // The UE reestablishes the connection in the same cell.
  const gnb_du_ue_f1ap_id_t new_du_ue_id = int_to_gnb_du_ue_f1ap_id(1);
  const rnti_t              new_crnti    = to_rnti(0x4602);
  get_du(du_idx).push_ul_pdu(test_helpers::generate_init_ul_rrc_message_transfer(
      new_du_ue_id,
      new_crnti,
      plmn_identity::test_value(),
      {},
      test_helpers::pack_ul_ccch_msg(test_helpers::create_rrc_reestablishment_request(crnti, 0, "1111010001000010"))));
  ASSERT_TRUE(this->wait_for_f1ap_tx_pdu(du_idx, f1ap_pdu));
  ASSERT_TRUE(test_helpers::is_valid_dl_rrc_message_transfer_with_msg4(f1ap_pdu));
  ASSERT_EQ(f1ap_pdu.pdu.init_msg().value.dl_rrc_msg_transfer()->srb_id, 1) << "CU-CP did not accept reestablishment";
  const gnb_cu_ue_f1ap_id_t new_cu_ue_id =
      int_to_gnb_cu_ue_f1ap_id(f1ap_pdu.pdu.init_msg().value.dl_rrc_msg_transfer()->gnb_cu_ue_f1ap_id);

  // The UE completes the reestablishment. The PDCP header and MAC-I fit the keys of the reestablished UE.
  byte_buffer reest_complete = test_helpers::pack_ul_dcch_msg(test_helpers::create_rrc_reestablishment_complete());
  ASSERT_TRUE(reest_complete.prepend(std::array<uint8_t, 2>{0x00U, 0x00U}));
  ASSERT_TRUE(reest_complete.append(std::array<uint8_t, 4>{0x85, 0xc1, 0x04, 0xf1}));
  get_du(du_idx).push_ul_pdu(test_helpers::generate_ul_rrc_message_transfer(
      new_du_ue_id, new_cu_ue_id, srb_id_t::srb1, std::move(reest_complete)));

  // The CU-UP gives the new UL tunnels.
  e1ap_message e1ap_pdu;
  ASSERT_TRUE(this->wait_for_e1ap_tx_pdu(cu_up_idx, e1ap_pdu));
  const auto& bearer_mod_req = e1ap_pdu.pdu.init_msg().value.bearer_context_mod_request();
  const auto  cu_cp_e1ap_id  = int_to_gnb_cu_cp_ue_e1ap_id(bearer_mod_req->gnb_cu_cp_ue_e1ap_id);
  const auto  new_cu_up_id   = int_to_gnb_cu_up_ue_e1ap_id(bearer_mod_req->gnb_cu_up_ue_e1ap_id);
  get_cu_up(cu_up_idx).push_tx_pdu(generate_bearer_context_modification_response(cu_cp_e1ap_id, new_cu_up_id));

  // The UE Context Modification Request carries the location measurements of the UE.
  ASSERT_TRUE(this->wait_for_f1ap_tx_pdu(du_idx, f1ap_pdu));
  ASSERT_TRUE(test_helpers::is_valid_ue_context_modification_request(f1ap_pdu));
  const auto& req = f1ap_pdu.pdu.init_msg().value.ue_context_mod_request();
  ASSERT_TRUE(req->cu_to_du_rrc_info_present);
  ASSERT_TRUE(req->cu_to_du_rrc_info.ie_exts_present);
  ASSERT_TRUE(req->cu_to_du_rrc_info.ie_exts.location_meas_info_present);
  byte_buffer   expected_info;
  asn1::bit_ref bref{expected_info};
  ASSERT_EQ(asn1::pack(bref, test_helpers::create_nr_prs_location_meas_info()), asn1::OCUDUASN_SUCCESS);
  EXPECT_EQ(req->cu_to_du_rrc_info.ie_exts.location_meas_info, expected_info);

  // The DU configures the measurement gap again.
  f1ap_message resp = test_helpers::generate_ue_context_modification_response(new_du_ue_id, new_cu_ue_id, new_crnti);
  resp.pdu.successful_outcome().value.ue_context_mod_resp()->du_to_cu_rrc_info.meas_gap_cfg =
      make_packed_meas_gap_cfg();
  get_du(du_idx).push_ul_pdu(resp);
  ASSERT_TRUE(this->wait_for_e1ap_tx_pdu(cu_up_idx, e1ap_pdu));
  get_cu_up(cu_up_idx).push_tx_pdu(generate_bearer_context_modification_response(cu_cp_e1ap_id, new_cu_up_id));

  // The RRC Reconfiguration after the reestablishment carries the measurement gap.
  std::optional<asn1::rrc_nr::rrc_recfg_s> rrc_recfg = await_rrc_reconfiguration_with_meas_gap_cfg();
  ASSERT_TRUE(rrc_recfg.has_value());
  const asn1::rrc_nr::meas_gap_cfg_s& meas_gap_cfg = rrc_recfg->crit_exts.rrc_recfg().meas_cfg.meas_gap_cfg;
  ASSERT_TRUE(meas_gap_cfg.gap_fr2_present);
  EXPECT_EQ(meas_gap_cfg.gap_fr2.setup().gap_offset, 7);
}

TEST_F(cu_cp_location_measurement_indication_test,
       when_du_configures_no_measurement_gap_then_no_rrc_reconfiguration_is_sent_to_ue)
{
  OCUDU_TEST_REQUIREMENTS("MVP-FUNC-POS-16-3-b");

  send_location_measurement_indication(test_helpers::create_nr_prs_location_meas_info());
  ASSERT_TRUE(await_ue_context_modification_request(/* with_location_meas_info */ true));

  send_ue_context_modification_response(byte_buffer{});

  ASSERT_FALSE(this->wait_for_f1ap_tx_pdu(du_idx, f1ap_pdu, std::chrono::milliseconds{100}));
  EXPECT_NE(this->find_ue_context(du_idx, du_ue_id), nullptr);
}

TEST_F(cu_cp_location_measurement_indication_test,
       when_du_rejects_the_ue_context_modification_then_no_rrc_reconfiguration_is_sent_and_ue_stays_connected)
{
  OCUDU_TEST_REQUIREMENTS("MVP-FUNC-POS-16-3-b");

  send_location_measurement_indication(test_helpers::create_nr_prs_location_meas_info());
  ASSERT_TRUE(await_ue_context_modification_request(/* with_location_meas_info */ true));

  get_du(du_idx).push_ul_pdu(
      test_helpers::generate_ue_context_modification_failure(ue_ctx->cu_ue_id.value(), ue_ctx->du_ue_id.value()));

  ASSERT_FALSE(this->wait_for_f1ap_tx_pdu(du_idx, f1ap_pdu, std::chrono::milliseconds{100}));
  EXPECT_NE(this->find_ue_context(du_idx, du_ue_id), nullptr);
}

TEST_F(cu_cp_location_measurement_indication_test,
       when_ue_without_location_measurements_stops_them_then_nothing_is_sent_to_du)
{
  OCUDU_TEST_REQUIREMENTS("MVP-FUNC-POS-16-3-b");

  send_location_measurement_indication(std::nullopt);

  ASSERT_FALSE(this->wait_for_f1ap_tx_pdu(du_idx, f1ap_pdu, std::chrono::milliseconds{100}));
  EXPECT_NE(this->find_ue_context(du_idx, du_ue_id), nullptr);
}
