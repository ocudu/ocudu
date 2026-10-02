// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "cu_cp_test_environment.h"
#include "tests/ocudu_test_requirements.h"
#include "tests/test_doubles/f1ap/f1ap_test_message_validators.h"
#include "tests/test_doubles/f1ap/f1ap_test_messages.h"
#include "tests/test_doubles/rrc/rrc_test_messages.h"
#include "tests/unittests/cu_cp/test_helpers.h"
#include "ocudu/asn1/f1ap/f1ap_pdu_contents_ue.h"
#include "ocudu/asn1/rrc_nr/dl_dcch_msg.h"
#include "ocudu/asn1/rrc_nr/ul_dcch_msg.h"
#include "ocudu/f1ap/f1ap_message.h"
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

  /// The LocationMeasurementInfo with one NR PRS measurement.
  static asn1::rrc_nr::location_meas_info_c make_nr_prs_location_meas_info()
  {
    asn1::rrc_nr::nr_prs_meas_info_r16_s prs_info;
    prs_info.dl_prs_point_a_r16                               = 643296;
    prs_info.nr_meas_prs_repeat_and_offset_r16.set_ms40_r16() = 7;
    prs_info.nr_meas_prs_len_r16.value = asn1::rrc_nr::nr_prs_meas_info_r16_s::nr_meas_prs_len_r16_opts::ms6;

    asn1::rrc_nr::location_meas_info_c info;
    info.set_nr_prs_meas_r16().push_back(prs_info);
    return info;
  }

  /// The packed MeasGapConfig the DU returns: an FR2 gap of 6 ms every 40 ms.
  static byte_buffer make_packed_meas_gap_cfg()
  {
    asn1::rrc_nr::meas_gap_cfg_s meas_gap_cfg;
    meas_gap_cfg.gap_fr2_present = true;
    meas_gap_cfg.gap_fr2.set_setup();
    meas_gap_cfg.gap_fr2.setup().gap_offset = 7;
    meas_gap_cfg.gap_fr2.setup().mgl.value  = asn1::rrc_nr::gap_cfg_s::mgl_opts::ms6;
    meas_gap_cfg.gap_fr2.setup().mgrp.value = asn1::rrc_nr::gap_cfg_s::mgrp_opts::ms40;
    meas_gap_cfg.gap_fr2.setup().mgta.value = asn1::rrc_nr::gap_cfg_s::mgta_opts::ms0;

    byte_buffer   pdu;
    asn1::bit_ref bref{pdu};
    EXPECT_EQ(meas_gap_cfg.pack(bref), asn1::OCUDUASN_SUCCESS);
    return pdu;
  }

  /// The UE sends a LocationMeasurementIndication. It indicates a start with \c info, or a stop without it.
  void send_location_measurement_indication(const std::optional<asn1::rrc_nr::location_meas_info_c>& info)
  {
    asn1::rrc_nr::ul_dcch_msg_s            ul_dcch_msg;
    asn1::rrc_nr::location_meas_ind_ies_s& ies =
        ul_dcch_msg.msg.set_c1().set_location_meas_ind().crit_exts.set_location_meas_ind();
    if (info.has_value()) {
      ies.meas_ind.set_setup() = info.value();
    } else {
      ies.meas_ind.set_release();
    }

    get_du(du_idx).push_ul_pdu(
        test_helpers::generate_ul_rrc_message_transfer(ue_ctx->du_ue_id.value(),
                                                       ue_ctx->cu_ue_id.value(),
                                                       srb_id_t::srb1,
                                                       generate_protected_ul_dcch_pdu(ul_dcch_msg, ul_count++)));
  }

  /// Waits for the UE Context Modification Request and checks that it carries the Location Measurement Information.
  [[nodiscard]] bool await_ue_context_modification_request_with_location_meas_info()
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
    if (!req->cu_to_du_rrc_info_present || !req->cu_to_du_rrc_info.ie_exts_present ||
        !req->cu_to_du_rrc_info.ie_exts.location_meas_info_present) {
      test_logger.error("UE Context Modification Request without Location Measurement Information");
      return false;
    }

    // The DU receives the LocationMeasurementInfo of the UE unchanged.
    byte_buffer   expected_info;
    asn1::bit_ref bref{expected_info};
    if (make_nr_prs_location_meas_info().pack(bref) != asn1::OCUDUASN_SUCCESS) {
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

  send_location_measurement_indication(make_nr_prs_location_meas_info());
  ASSERT_TRUE(await_ue_context_modification_request_with_location_meas_info());

  send_ue_context_modification_response(make_packed_meas_gap_cfg());

  // The CU-CP sends the gap to the UE in an RRC Reconfiguration.
  ASSERT_TRUE(this->wait_for_f1ap_tx_pdu(du_idx, f1ap_pdu));
  ASSERT_TRUE(test_helpers::is_valid_dl_rrc_message_transfer(f1ap_pdu));
  byte_buffer dl_dcch_pdu = test_helpers::extract_dl_dcch_msg(test_helpers::get_rrc_container(f1ap_pdu));
  asn1::rrc_nr::dl_dcch_msg_s dl_dcch_msg;
  {
    asn1::cbit_ref bref{dl_dcch_pdu};
    ASSERT_EQ(dl_dcch_msg.unpack(bref), asn1::OCUDUASN_SUCCESS);
  }
  ASSERT_EQ(dl_dcch_msg.msg.c1().type().value, asn1::rrc_nr::dl_dcch_msg_type_c::c1_c_::types_opts::rrc_recfg);
  const asn1::rrc_nr::rrc_recfg_s&     rrc_recfg = dl_dcch_msg.msg.c1().rrc_recfg();
  const asn1::rrc_nr::rrc_recfg_ies_s& ies       = rrc_recfg.crit_exts.rrc_recfg();
  ASSERT_TRUE(ies.meas_cfg_present);
  ASSERT_TRUE(ies.meas_cfg.meas_gap_cfg_present);
  ASSERT_TRUE(ies.meas_cfg.meas_gap_cfg.gap_fr2_present);
  EXPECT_EQ(ies.meas_cfg.meas_gap_cfg.gap_fr2.setup().gap_offset, 7);
  // The RRC Reconfiguration also carries the cell group config the DU returned.
  ASSERT_TRUE(ies.non_crit_ext_present);
  EXPECT_FALSE(ies.non_crit_ext.master_cell_group.empty());

  // The UE completes the reconfiguration, and the procedure ends without further messages to the DU.
  get_du(du_idx).push_ul_pdu(test_helpers::generate_ul_rrc_message_transfer(
      ue_ctx->du_ue_id.value(),
      ue_ctx->cu_ue_id.value(),
      srb_id_t::srb1,
      generate_rrc_reconfiguration_complete_pdu(rrc_recfg.rrc_transaction_id, ul_count++)));
  ASSERT_FALSE(this->wait_for_f1ap_tx_pdu(du_idx, f1ap_pdu, std::chrono::milliseconds{100}));
  EXPECT_NE(this->find_ue_context(du_idx, du_ue_id), nullptr);
}

TEST_F(cu_cp_location_measurement_indication_test,
       when_du_configures_no_measurement_gap_then_no_rrc_reconfiguration_is_sent_to_ue)
{
  OCUDU_TEST_REQUIREMENTS("MVP-FUNC-POS-16-3-b");

  send_location_measurement_indication(make_nr_prs_location_meas_info());
  ASSERT_TRUE(await_ue_context_modification_request_with_location_meas_info());

  send_ue_context_modification_response(byte_buffer{});

  ASSERT_FALSE(this->wait_for_f1ap_tx_pdu(du_idx, f1ap_pdu, std::chrono::milliseconds{100}));
  EXPECT_NE(this->find_ue_context(du_idx, du_ue_id), nullptr);
}

TEST_F(cu_cp_location_measurement_indication_test,
       when_du_rejects_the_ue_context_modification_then_no_rrc_reconfiguration_is_sent_and_ue_stays_connected)
{
  OCUDU_TEST_REQUIREMENTS("MVP-FUNC-POS-16-3-b");

  send_location_measurement_indication(make_nr_prs_location_meas_info());
  ASSERT_TRUE(await_ue_context_modification_request_with_location_meas_info());

  get_du(du_idx).push_ul_pdu(
      test_helpers::generate_ue_context_modification_failure(ue_ctx->cu_ue_id.value(), ue_ctx->du_ue_id.value()));

  ASSERT_FALSE(this->wait_for_f1ap_tx_pdu(du_idx, f1ap_pdu, std::chrono::milliseconds{100}));
  EXPECT_NE(this->find_ue_context(du_idx, du_ue_id), nullptr);
}

TEST_F(cu_cp_location_measurement_indication_test, when_ue_stops_location_measurements_then_nothing_is_sent_to_du)
{
  OCUDU_TEST_REQUIREMENTS("MVP-FUNC-POS-16-3-b");

  send_location_measurement_indication(std::nullopt);

  ASSERT_FALSE(this->wait_for_f1ap_tx_pdu(du_idx, f1ap_pdu, std::chrono::milliseconds{100}));
  EXPECT_NE(this->find_ue_context(du_idx, du_ue_id), nullptr);
}
