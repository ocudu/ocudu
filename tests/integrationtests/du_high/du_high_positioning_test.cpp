// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "tests/integrationtests/du_high/test_utils/du_high_env_simulator.h"
#include "tests/test_doubles/f1ap/f1ap_test_message_validators.h"
#include "tests/test_doubles/f1ap/f1ap_test_messages.h"
#include "tests/test_doubles/pdcp/pdcp_pdu_generator.h"
#include "ocudu/asn1/f1ap/f1ap_pdu_contents.h"
#include "ocudu/asn1/f1ap/f1ap_pdu_contents_ue.h"

using namespace ocudu;
using namespace odu;
using namespace asn1::f1ap;

class du_high_tester : public du_high_env_simulator, public testing::Test
{
public:
  du_high_tester() : du_high_env_simulator(du_high_env_sim_params{.srs_period = srs_periodicity::sl80}) {}
};

TEST_F(du_high_tester, when_trp_information_request_is_received_then_response_is_sent_to_cu)
{
  // DU receives TRP INFORMATION REQUEST.
  cu_notifier.f1ap_ul_msgs.clear();
  f1ap_message msg = ocudu::test_helpers::generate_trp_information_request();
  this->du_hi->get_f1ap_pdu_handler().handle_message(msg);
  this->test_logger.info("STATUS: TRP INFORMATION REQUEST received by DU. Waiting for TRP INFORMATION RESPONSE ACK...");

  // Wait for TRP INFORMATION RESPONSE to be sent to the CU.
  EXPECT_TRUE(this->run_until([this]() { return not cu_notifier.f1ap_ul_msgs.empty(); }));
  ASSERT_TRUE(ocudu::test_helpers::is_valid_f1ap_trp_information_response(cu_notifier.f1ap_ul_msgs.rbegin()->second));
}

TEST_F(du_high_tester, when_positioning_information_request_is_received_then_response_is_sent_to_cu)
{
  // DU receives TRP INFORMATION REQUEST.
  cu_notifier.f1ap_ul_msgs.clear();
  f1ap_message trp_info_req = ocudu::test_helpers::generate_trp_information_request();
  this->du_hi->get_f1ap_pdu_handler().handle_message(trp_info_req);
  this->test_logger.info("STATUS: TRP INFORMATION REQUEST received by DU. Waiting for TRP INFORMATION RESPONSE ACK...");

  // Wait for TRP INFORMATION RESPONSE to be sent to the CU.
  EXPECT_TRUE(this->run_until([this]() { return not cu_notifier.f1ap_ul_msgs.empty(); }));
  ASSERT_TRUE(test_helpers::is_valid_f1ap_trp_information_response(cu_notifier.f1ap_ul_msgs.rbegin()->second));

  // Create UE.
  rnti_t rnti = to_rnti(0x4601);
  ASSERT_TRUE(add_ue(rnti));
  ASSERT_TRUE(run_rrc_setup(rnti));
  ASSERT_TRUE(run_ue_context_setup(rnti));

  gnb_du_ue_f1ap_id_t du_ue_id = int_to_gnb_du_ue_f1ap_id(
      cu_notifier.f1ap_ul_msgs.rbegin()->second.pdu.init_msg().value.ul_rrc_msg_transfer()->gnb_du_ue_f1ap_id);
  gnb_cu_ue_f1ap_id_t cu_ue_id = int_to_gnb_cu_ue_f1ap_id(
      cu_notifier.f1ap_ul_msgs.rbegin()->second.pdu.init_msg().value.ul_rrc_msg_transfer()->gnb_cu_ue_f1ap_id);

  // DU receives POSITIONING INFORMATION REQUEST.
  cu_notifier.f1ap_ul_msgs.clear();
  f1ap_message pos_info_req = test_helpers::generate_positioning_information_request(du_ue_id, cu_ue_id);
  this->du_hi->get_f1ap_pdu_handler().handle_message(pos_info_req);
  this->test_logger.info(
      "STATUS: POSITIONING INFORMATION REQUEST received by DU. Waiting for POSITIONING INFORMATION RESPONSE ACK...");

  // Wait for POSITIONING INFORMATION RESPONSE to be sent to the CU.
  EXPECT_TRUE(this->run_until([this]() { return not cu_notifier.f1ap_ul_msgs.empty(); }));
  ASSERT_TRUE(test_helpers::is_valid_f1ap_positioning_information_response(cu_notifier.f1ap_ul_msgs.rbegin()->second));
}

namespace {

struct pos_req_params {
  bool ue_connected;
  bool rsrp_meas;
};

} // namespace

class du_high_pos_req_tester : public du_high_env_simulator, public ::testing::TestWithParam<pos_req_params>
{
public:
  du_high_pos_req_tester() :
    du_high_env_simulator(du_high_env_sim_params{.srs_period = srs_periodicity::sl80}),
    srs_offset(GetParam().ue_connected ? 0U : 1U)
  {
    if (GetParam().rsrp_meas) {
      meas_requests = {pos_meas_type_opts::options::ul_rtoa, pos_meas_type_opts::options::ul_srs_rsrp};
    } else {
      meas_requests = {pos_meas_type_opts::options::ul_rtoa};
    }
  }

  unsigned                                             srs_offset;
  std::vector<asn1::f1ap::pos_meas_type_opts::options> meas_requests;
  ocudulog::basic_logger&                              du_logger = ocudulog::fetch_basic_logger("D1-F1");
};

TEST_P(du_high_pos_req_tester, when_positioning_measurement_request_is_received_then_response_is_sent_to_cu)
{
  // DU receives TRP INFORMATION REQUEST.
  cu_notifier.f1ap_ul_msgs.clear();
  f1ap_message trp_info_req = test_helpers::generate_trp_information_request();
  this->du_hi->get_f1ap_pdu_handler().handle_message(trp_info_req);
  this->test_logger.info("STATUS: TRP INFORMATION REQUEST received by DU. Waiting for TRP INFORMATION RESPONSE ACK...");

  // Wait for TRP INFORMATION RESPONSE to be sent to the CU.
  EXPECT_TRUE(this->run_until([this]() { return not cu_notifier.f1ap_ul_msgs.empty(); }));
  ASSERT_TRUE(test_helpers::is_valid_f1ap_trp_information_response(cu_notifier.f1ap_ul_msgs.rbegin()->second));

  // Create UE.
  rnti_t rnti = to_rnti(0x4601);
  ASSERT_TRUE(add_ue(rnti));
  ASSERT_TRUE(run_rrc_setup(rnti));
  ASSERT_TRUE(run_ue_context_setup(rnti));

  gnb_du_ue_f1ap_id_t du_ue_id = int_to_gnb_du_ue_f1ap_id(
      cu_notifier.f1ap_ul_msgs.rbegin()->second.pdu.init_msg().value.ul_rrc_msg_transfer()->gnb_du_ue_f1ap_id);
  gnb_cu_ue_f1ap_id_t cu_ue_id = int_to_gnb_cu_ue_f1ap_id(
      cu_notifier.f1ap_ul_msgs.rbegin()->second.pdu.init_msg().value.ul_rrc_msg_transfer()->gnb_cu_ue_f1ap_id);

  // DU receives POSITIONING INFORMATION REQUEST.
  cu_notifier.f1ap_ul_msgs.clear();
  f1ap_message pos_info_req = test_helpers::generate_positioning_information_request(du_ue_id, cu_ue_id);
  this->du_hi->get_f1ap_pdu_handler().handle_message(pos_info_req);
  this->test_logger.info(
      "STATUS: POSITIONING INFORMATION REQUEST received by DU. Waiting for POSITIONING INFORMATION RESPONSE ACK...");

  // Wait for POSITIONING INFORMATION RESPONSE to be sent to the CU.
  EXPECT_TRUE(this->run_until([this]() { return not cu_notifier.f1ap_ul_msgs.empty(); }));
  ASSERT_TRUE(test_helpers::is_valid_f1ap_positioning_information_response(cu_notifier.f1ap_ul_msgs.rbegin()->second));

  // DU receives POSITIONING MEASUREMENT REQUEST.
  cu_notifier.f1ap_ul_msgs.clear();
  // For connected UE, SRS offset 0U (SRS offset of the existing RNTI as configured in the base class); while set SRS
  // offset 1U to simulate a request for a neighbor UE.
  // NOTE: the Positioning Measurement Request doesn't include any indication about the UE whose position should be
  // measured. It only includes the TRP list + the SRS configuration that needs to be measured. The DU forwards this to
  // the cells corresponding to the TRP list; if the cell detects the SRS config corresponds to an existing UE then it
  // will assume the measurement request is for that UE; else it will consider the request is for a neighbor UE.
  f1ap_message pos_meas_req = test_helpers::generate_positioning_measurement_request(
      {trp_id_t::min}, lmf_meas_id_t::min, ran_meas_id_t::min, meas_requests, subcarrier_spacing::kHz15, srs_offset);

  this->du_hi->get_f1ap_pdu_handler().handle_message(pos_meas_req);
  this->test_logger.info(
      "STATUS: POSITIONING MEASUREMENT REQUEST received by DU. Waiting for POSITIONING MEASUREMENT RESPONSE...");
  {
    asn1::json_writer js;
    pos_meas_req.pdu.to_json(js);
    du_logger.info("Containerized Positioning Measurement Request MSG: {}", js.to_string());
  }

  // Wait for POSITIONING MEASUREMENT RESPONSE to be sent to the CU.
  EXPECT_TRUE(this->run_until([this]() { return not cu_notifier.f1ap_ul_msgs.empty(); }));
  const auto pos_meas_resp = cu_notifier.f1ap_ul_msgs.rbegin()->second;
  {
    asn1::json_writer js;
    pos_meas_resp.pdu.to_json(js);
    du_logger.info("Containerized Positioning Measurement Response MSG: {}", js.to_string());
  }
  ASSERT_TRUE(test_helpers::is_valid_f1ap_positioning_measurement_response(pos_meas_resp));
  const auto& resp_msg = pos_meas_resp.pdu.successful_outcome().value.positioning_meas_resp();
  const auto& req_msg  = pos_meas_req.pdu.init_msg().value.positioning_meas_request();
  ASSERT_EQ(req_msg->transaction_id, resp_msg->transaction_id);
  ASSERT_EQ(req_msg->lmf_meas_id, resp_msg->lmf_meas_id);
  ASSERT_EQ(req_msg->ran_meas_id, resp_msg->ran_meas_id);
  ASSERT_TRUE(resp_msg->pos_meas_result_list_present);
  ASSERT_EQ(req_msg->trp_meas_request_list.size(), resp_msg->pos_meas_result_list.size());
  ASSERT_EQ(req_msg->trp_meas_request_list[0].trp_id, resp_msg->pos_meas_result_list[0].trp_id);
  ASSERT_EQ(meas_requests.size(), resp_msg->pos_meas_result_list[0].pos_meas_result.size());
  ASSERT_EQ(asn1::f1ap::measured_results_value_c::types_opts::ul_rtoa,
            resp_msg->pos_meas_result_list[0].pos_meas_result[0].measured_results_value.type().value);
  ASSERT_TRUE(resp_msg->pos_meas_result_list[0].pos_meas_result[0].time_stamp.slot_idx.type().value ==
                  asn1::f1ap::time_stamp_slot_idx_c::types_opts::scs_15 or
              resp_msg->pos_meas_result_list[0].pos_meas_result[0].time_stamp.slot_idx.type().value ==
                  asn1::f1ap::time_stamp_slot_idx_c::types_opts::scs_30);
  // The slot idx corresponding to the time-stamp should coincide with the SRS offset set in the SRS Config contained in
  // the Positioning Measurement Request. Only SCS 15 kHz and SCS 30 kHz are supported in the test environment. For the
  // neighboring UE test.
  auto slot_idx = resp_msg->pos_meas_result_list[0].pos_meas_result[0].time_stamp.slot_idx.type().value ==
                          asn1::f1ap::time_stamp_slot_idx_c::types_opts::scs_15
                      ? resp_msg->pos_meas_result_list[0].pos_meas_result[0].time_stamp.slot_idx.scs_15()
                      : resp_msg->pos_meas_result_list[0].pos_meas_result[0].time_stamp.slot_idx.scs_30();
  ASSERT_EQ(slot_idx, srs_offset);
  if (GetParam().rsrp_meas) {
    // This is the value that corresponds to the RSRP -84.6 dBSF hard-coded in the SRS indication.
    const uint8_t expected_rsrp = 61;
    ASSERT_EQ(asn1::f1ap::measured_results_value_c::types_opts::ul_srs_rsrp,
              resp_msg->pos_meas_result_list[0].pos_meas_result[1].measured_results_value.type().value);
    ASSERT_EQ(expected_rsrp, resp_msg->pos_meas_result_list[0].pos_meas_result[1].measured_results_value.ul_srs_rsrp());
    slot_idx = resp_msg->pos_meas_result_list[0].pos_meas_result[1].time_stamp.slot_idx.type().value ==
                       asn1::f1ap::time_stamp_slot_idx_c::types_opts::scs_15
                   ? resp_msg->pos_meas_result_list[0].pos_meas_result[1].time_stamp.slot_idx.scs_15()
                   : resp_msg->pos_meas_result_list[0].pos_meas_result[1].time_stamp.slot_idx.scs_30();
    ASSERT_TRUE(slot_idx == srs_offset);
  }
}

INSTANTIATE_TEST_SUITE_P(test_positionin_for_connected_vs_neighboring_ue,
                         du_high_pos_req_tester,
                         testing::Values(pos_req_params{true, false},
                                         pos_req_params{true, true},
                                         pos_req_params{false, false},
                                         pos_req_params{false, true}),
                         [](const testing::TestParamInfo<pos_req_params>& params_item) {
                           return fmt::format("{}_{}_rsrp",
                                              params_item.param.ue_connected ? "connected_ue" : "neighboring_ue",
                                              params_item.param.rsrp_meas ? "with" : "no");
                         });

class du_high_pos_multi_cells_tester : public du_high_env_simulator, public testing::Test
{
public:
  du_high_pos_multi_cells_tester() :
    du_high_env_simulator(du_high_env_sim_params{.nof_cells = 3, .srs_period = srs_periodicity::sl80})
  {
  }

  ocudulog::basic_logger& du_logger = ocudulog::fetch_basic_logger("D1-F1");
};

TEST_F(du_high_pos_multi_cells_tester,
       when_positioning_measurement_request_is_received_for_a_ue_then_response_is_sent_to_cu)
{
  // This tests generates 3 cells connected to the same DU; 1 UE connected to cell 1 and configured with 1 SRS resource.

  // DU receives TRP INFORMATION REQUEST.
  cu_notifier.f1ap_ul_msgs.clear();
  f1ap_message trp_info_req = test_helpers::generate_trp_information_request();
  this->du_hi->get_f1ap_pdu_handler().handle_message(trp_info_req);
  this->test_logger.info("STATUS: TRP INFORMATION REQUEST received by DU. Waiting for TRP INFORMATION RESPONSE ACK...");
  {
    asn1::json_writer js;
    trp_info_req.pdu.to_json(js);
    du_logger.info("Containerized TRP INFORMATION REQUEST MSG: {}", js.to_string());
  }

  // Wait for TRP INFORMATION RESPONSE to be sent to the CU.
  EXPECT_TRUE(this->run_until([this]() { return not cu_notifier.f1ap_ul_msgs.empty(); }));
  const auto& trp_info_resp = cu_notifier.f1ap_ul_msgs.rbegin()->second;
  ASSERT_TRUE(test_helpers::is_valid_f1ap_trp_information_response(trp_info_resp));
  {
    asn1::json_writer js;
    trp_info_resp.pdu.to_json(js);
    du_logger.info("Containerized TRP INFORMATION RESPONSE MSG: {}", js.to_string());
  }

  // Create UE connected to cell idx 0.
  rnti_t rnti = to_rnti(0x4601);
  ASSERT_TRUE(add_ue(rnti, to_du_cell_index(0)));
  ASSERT_TRUE(run_rrc_setup(rnti));
  ASSERT_TRUE(run_ue_context_setup(rnti));

  gnb_du_ue_f1ap_id_t du_ue_id = int_to_gnb_du_ue_f1ap_id(
      cu_notifier.f1ap_ul_msgs.rbegin()->second.pdu.init_msg().value.ul_rrc_msg_transfer()->gnb_du_ue_f1ap_id);
  gnb_cu_ue_f1ap_id_t cu_ue_id = int_to_gnb_cu_ue_f1ap_id(
      cu_notifier.f1ap_ul_msgs.rbegin()->second.pdu.init_msg().value.ul_rrc_msg_transfer()->gnb_cu_ue_f1ap_id);

  // DU receives POSITIONING INFORMATION REQUEST.
  cu_notifier.f1ap_ul_msgs.clear();
  f1ap_message pos_info_req = test_helpers::generate_positioning_information_request(du_ue_id, cu_ue_id);
  this->du_hi->get_f1ap_pdu_handler().handle_message(pos_info_req);
  this->test_logger.info(
      "STATUS: POSITIONING INFORMATION REQUEST received by DU. Waiting for POSITIONING INFORMATION RESPONSE ACK...");

  // Wait for POSITIONING INFORMATION RESPONSE to be sent to the CU.
  EXPECT_TRUE(this->run_until([this]() { return not cu_notifier.f1ap_ul_msgs.empty(); }));
  ASSERT_TRUE(test_helpers::is_valid_f1ap_positioning_information_response(cu_notifier.f1ap_ul_msgs.rbegin()->second));

  // DU receives POSITIONING MEASUREMENT REQUEST.
  cu_notifier.f1ap_ul_msgs.clear();
  trp_id_t     trp_id_cell_0 = trp_id_t::min;
  trp_id_t     trp_id_cell_1 = uint_to_trp_id(trp_id_to_uint(trp_id_t::min) + 1);
  trp_id_t     trp_id_cell_2 = uint_to_trp_id(trp_id_to_uint(trp_id_t::min) + 2);
  f1ap_message pos_meas_req =
      test_helpers::generate_positioning_measurement_request({trp_id_cell_0, trp_id_cell_1, trp_id_cell_2},
                                                             lmf_meas_id_t::min,
                                                             ran_meas_id_t::min,
                                                             {pos_meas_type_opts::options::ul_rtoa,
                                                              pos_meas_type_opts::options::ul_srs_rsrp,
                                                              pos_meas_type_opts::options::ul_aoa},
                                                             subcarrier_spacing::kHz15);

  this->du_hi->get_f1ap_pdu_handler().handle_message(pos_meas_req);
  this->test_logger.info(
      "STATUS: POSITIONING MEASUREMENT REQUEST received by DU. Waiting for POSITIONING MEASUREMENT RESPONSE...");
  {
    asn1::json_writer js;
    pos_meas_req.pdu.to_json(js);
    du_logger.info("Containerized Positioning Measurement Request MSG: {}", js.to_string());
  }

  // Wait for POSITIONING MEASUREMENT RESPONSE to be sent to the CU.
  EXPECT_TRUE(this->run_until([this]() { return not cu_notifier.f1ap_ul_msgs.empty(); }));

  const auto& pos_meas_resp = cu_notifier.f1ap_ul_msgs.rbegin()->second;
  {
    asn1::json_writer js;
    pos_meas_resp.pdu.to_json(js);
    du_logger.info("Containerized Positioning Measurement Response MSG: {}", js.to_string());
  }

  ASSERT_TRUE(test_helpers::is_valid_f1ap_positioning_measurement_response(pos_meas_resp));
  const auto& resp_msg = pos_meas_resp.pdu.successful_outcome().value.positioning_meas_resp();
  const auto& req_msg  = pos_meas_req.pdu.init_msg().value.positioning_meas_request();
  ASSERT_EQ(req_msg->transaction_id, resp_msg->transaction_id);
  ASSERT_EQ(req_msg->lmf_meas_id, resp_msg->lmf_meas_id);
  ASSERT_EQ(req_msg->ran_meas_id, resp_msg->ran_meas_id);
  ASSERT_TRUE(resp_msg->pos_meas_result_list_present);
  ASSERT_EQ(req_msg->trp_meas_request_list.size(), resp_msg->pos_meas_result_list.size());
  const unsigned expected_srs_offset = 0U;
  for (unsigned trp_idx = 0, sz = req_msg->trp_meas_request_list.size(); trp_idx != sz; ++trp_idx) {
    ASSERT_EQ(req_msg->trp_meas_request_list[trp_idx].trp_id, resp_msg->pos_meas_result_list[trp_idx].trp_id);
    // The Positioning Measured Result should contain 3 items: UL-RTOA, UL-RSRP and UL-AoA.
    ASSERT_EQ(3U, resp_msg->pos_meas_result_list[trp_idx].pos_meas_result.size());
    ASSERT_EQ(asn1::f1ap::measured_results_value_c::types_opts::ul_rtoa,
              resp_msg->pos_meas_result_list[trp_idx].pos_meas_result[0].measured_results_value.type().value);
    const auto& pos_meas_result_rtoa = resp_msg->pos_meas_result_list[trp_idx].pos_meas_result[0];
    ASSERT_EQ(asn1::f1ap::measured_results_value_c::types_opts::ul_rtoa,
              pos_meas_result_rtoa.measured_results_value.type().value);
    ASSERT_TRUE(pos_meas_result_rtoa.time_stamp.slot_idx.type().value ==
                    asn1::f1ap::time_stamp_slot_idx_c::types_opts::scs_15 or
                pos_meas_result_rtoa.time_stamp.slot_idx.type().value ==
                    asn1::f1ap::time_stamp_slot_idx_c::types_opts::scs_30);
    // This is the value that corresponds to the RSRP -84.6 dBSF hard-coded in the SRS indication.
    const uint8_t expected_rsrp = 61;
    ASSERT_EQ(asn1::f1ap::measured_results_value_c::types_opts::ul_srs_rsrp,
              resp_msg->pos_meas_result_list[trp_idx].pos_meas_result[1].measured_results_value.type().value);
    ASSERT_EQ(expected_rsrp, resp_msg->pos_meas_result_list[0].pos_meas_result[1].measured_results_value.ul_srs_rsrp());
    // These correspond to the azimuth/zenith AoA (123.4/56.7 degrees) hard-coded in the SRS indication.
    const uint16_t expected_azimuth_aoa = 1234;
    const uint16_t expected_zenith_aoa  = 567;
    ASSERT_EQ(asn1::f1ap::measured_results_value_c::types_opts::ul_angle_of_arrival,
              resp_msg->pos_meas_result_list[trp_idx].pos_meas_result[2].measured_results_value.type().value);
    const auto& aoa_result =
        resp_msg->pos_meas_result_list[trp_idx].pos_meas_result[2].measured_results_value.ul_angle_of_arrival();
    ASSERT_EQ(expected_azimuth_aoa, aoa_result.azimuth_ao_a);
    ASSERT_TRUE(aoa_result.zenith_ao_a_present);
    ASSERT_EQ(expected_zenith_aoa, aoa_result.zenith_ao_a);
    // The slot idx corresponding to the time-stamp should coincide with the SRS offset, which in this case is 0. Only
    // SCS 15 kHz and SCS 30 kHz are supported in the test environment.
    auto slot_idx =
        pos_meas_result_rtoa.time_stamp.slot_idx.type().value == asn1::f1ap::time_stamp_slot_idx_c::types_opts::scs_15
            ? pos_meas_result_rtoa.time_stamp.slot_idx.scs_15()
            : pos_meas_result_rtoa.time_stamp.slot_idx.scs_30();
    ASSERT_EQ(expected_srs_offset, slot_idx);
  }
}
