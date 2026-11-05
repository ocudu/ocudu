// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "cu_cp_test_environment.h"
#include "tests/test_doubles/f1ap/f1ap_test_message_validators.h"
#include "tests/test_doubles/f1ap/f1ap_test_messages.h"
#include "tests/test_doubles/ngap/ngap_test_message_validators.h"
#include "tests/test_doubles/rrc/rrc_test_messages.h"
#include "tests/unittests/ngap/ngap_test_messages.h"
#include "ocudu/asn1/f1ap/f1ap_pdu_contents.h"
#include "ocudu/asn1/f1ap/f1ap_pdu_contents_ue.h"
#include "ocudu/asn1/ngap/ngap_pdu_contents.h"
#include "ocudu/asn1/rrc_nr/dl_ccch_msg.h"
#include "ocudu/f1ap/f1ap_message.h"
#include "ocudu/ngap/ngap_message.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace ocucp;

/// Two AMFs of one AMF Set serve the PLMN of the CU-CP. AMF 0 serves the GUAMI with AMF Pointer 0, AMF 1 the GUAMI
/// with AMF Pointer 1.
class cu_cp_amf_pooling_test : public cu_cp_test_environment, public ::testing::Test
{
public:
  cu_cp_amf_pooling_test() :
    cu_cp_test_environment(
        cu_cp_test_env_params{8, 8, 8192, 8, {{default_supported_tracking_area}, {default_supported_tracking_area}}})
  {
  }

  /// Starts the CU-CP with the given relative AMF capacities, and sets up a DU and a CU-UP.
  void start_cu_cp(uint16_t amf_0_capacity, uint16_t amf_1_capacity, uint16_t amf_1_set_id = 1)
  {
    start_cu_cp(generate_ng_setup_response(plmn_identity::test_value(), 0, amf_0_capacity),
                generate_ng_setup_response(plmn_identity::test_value(), 1, amf_1_capacity, amf_1_set_id));
  }

  /// Starts the CU-CP with the given NG Setup Responses, and sets up a DU and a CU-UP. AMF 1 gets its own name.
  void start_cu_cp(const ngap_message& amf_0_response, ngap_message amf_1_response)
  {
    amf_1_response.pdu.successful_outcome().value.ng_setup_resp()->amf_name.from_string(amf_1_name);
    get_amf(0).enqueue_next_tx_pdu(amf_0_response);
    get_amf(1).enqueue_next_tx_pdu(amf_1_response);
    get_cu_cp().start();
    ngap_message ngap_pdu;
    report_fatal_error_if_not(get_amf(0).try_pop_rx_pdu(ngap_pdu), "AMF 0 received no NG Setup Request");
    report_fatal_error_if_not(get_amf(1).try_pop_rx_pdu(ngap_pdu), "AMF 1 received no NG Setup Request");

    std::optional<unsigned> ret = connect_new_du();
    report_fatal_error_if_not(ret.has_value(), "Failed to connect the DU");
    du_idx = ret.value();
    report_fatal_error_if_not(run_f1_setup(du_idx), "F1 Setup failed");

    ret = connect_new_cu_up();
    report_fatal_error_if_not(ret.has_value(), "Failed to connect the CU-UP");
    report_fatal_error_if_not(run_e1_setup(ret.value()), "E1 Setup failed");
  }

  /// Runs the RRC setup of a UE and returns the AMF that receives its Initial UE Message.
  std::optional<unsigned> attach_ue(gnb_du_ue_f1ap_id_t du_ue_id, const asn1::rrc_nr::ul_dcch_msg_s& rrc_setup_complete)
  {
    get_du(du_idx).push_ul_pdu(
        test_helpers::generate_init_ul_rrc_message_transfer(du_ue_id, to_rnti(0x4601 + fmt::underlying(du_ue_id))));
    f1ap_message f1ap_pdu;
    report_fatal_error_if_not(wait_for_f1ap_tx_pdu(du_idx, f1ap_pdu), "No RRC Setup was sent");
    report_fatal_error_if_not(test_helpers::is_valid_dl_rrc_message_transfer_with_msg4(f1ap_pdu), "Invalid Msg4");

    get_du(du_idx).push_rrc_ul_dcch_message(
        du_ue_id, srb_id_t::srb1, test_helpers::pack_ul_dcch_msg(rrc_setup_complete));

    for (unsigned attempt = 0; attempt != 10; ++attempt) {
      for (unsigned amf_idx = 0; amf_idx != 2; ++amf_idx) {
        ngap_message ngap_pdu;
        if (wait_for_ngap_tx_pdu(ngap_pdu, std::chrono::milliseconds{50}, amf_idx)) {
          // An AMF that is not set up can receive a retried NG Setup Request while the UE attaches. Skip it.
          if (is_pdu_type(ngap_pdu, asn1::ngap::ngap_elem_procs_o::init_msg_c::types::ng_setup_request)) {
            continue;
          }
          report_fatal_error_if_not(test_helpers::is_valid_init_ue_message(ngap_pdu), "Invalid Initial UE Message");
          last_ran_ue_id          = uint_to_ran_ue_id(ngap_pdu.pdu.init_msg().value.init_ue_msg()->ran_ue_ngap_id);
          last_initial_ue_message = ngap_pdu;
          return amf_idx;
        }
      }
    }
    return std::nullopt;
  }

  /// Runs the RRC setup of a UE that sends the given 5G-S-TMSI, and returns the AMF of its Initial UE Message.
  std::optional<unsigned> attach_ue_with_5g_s_tmsi(gnb_du_ue_f1ap_id_t du_ue_id, uint8_t amf_pointer)
  {
    // 5G-S-TMSI with AMF Set ID 1 (TS 23.003 section 2.11).
    uint64_t five_g_s_tmsi = (uint64_t{1} << 38U) + (uint64_t{amf_pointer} << 32U) + 0x1234;
    return attach_ue(du_ue_id, test_helpers::create_rrc_setup_complete(1, five_g_s_tmsi));
  }

  /// Name of AMF 1.
  const std::string amf_1_name = "open5gs-amf1";
  unsigned          du_idx     = 0;
  /// RAN UE NGAP ID of the last Initial UE Message.
  ran_ue_id_t last_ran_ue_id = ran_ue_id_t::invalid;
  /// Last Initial UE Message.
  ngap_message last_initial_ue_message;
};

TEST_F(cu_cp_amf_pooling_test, when_amf_has_capacity_zero_then_new_ues_are_served_by_the_other_amf)
{
  start_cu_cp(0, 255);

  for (unsigned i = 0; i != 5; ++i) {
    ASSERT_EQ(attach_ue(int_to_gnb_du_ue_f1ap_id(i), test_helpers::create_rrc_setup_complete()), 1);
  }
}

TEST_F(cu_cp_amf_pooling_test, when_amf_does_not_support_the_plmn_then_it_serves_no_ues)
{
  // AMF 1 supports only another PLMN. Without its PLMN Support List, the UEs would go to AMF 1.
  ngap_message amf_1_response = generate_ng_setup_response(plmn_identity::test_value(), 1, 255);
  amf_1_response.pdu.successful_outcome().value.ng_setup_resp()->plmn_support_list[0].plmn_id =
      plmn_identity::parse("99902").value().to_bytes();
  start_cu_cp(generate_ng_setup_response(plmn_identity::test_value(), 0, 0), amf_1_response);

  for (unsigned i = 0; i != 5; ++i) {
    ASSERT_EQ(attach_ue(int_to_gnb_du_ue_f1ap_id(i), test_helpers::create_rrc_setup_complete()), 0);
  }
}

TEST_F(cu_cp_amf_pooling_test, when_du_is_lost_then_each_amf_gets_an_ng_reset_for_its_own_ues)
{
  start_cu_cp(255, 255);
  ASSERT_EQ(attach_ue_with_5g_s_tmsi(int_to_gnb_du_ue_f1ap_id(0), 0), 0);
  ASSERT_EQ(attach_ue_with_5g_s_tmsi(int_to_gnb_du_ue_f1ap_id(1), 1), 1);

  ASSERT_TRUE(drop_du_connection(du_idx));

  for (unsigned amf_idx = 0; amf_idx != 2; ++amf_idx) {
    ngap_message ngap_pdu;
    ASSERT_TRUE(wait_for_ngap_tx_pdu(ngap_pdu, std::chrono::milliseconds{1000}, amf_idx))
        << "AMF " << amf_idx << " received no NG Reset";
    ASSERT_TRUE(test_helpers::is_valid_ng_reset(ngap_pdu));
    const auto& reset_type = ngap_pdu.pdu.init_msg().value.ng_reset()->reset_type;
    ASSERT_EQ(reset_type.type().value, asn1::ngap::reset_type_c::types_opts::part_of_ng_interface);
    ASSERT_EQ(reset_type.part_of_ng_interface().size(), 1) << "AMF " << amf_idx << " got the NG Reset of another AMF";
  }
}

TEST_F(cu_cp_amf_pooling_test, when_ue_sends_5g_s_tmsi_then_it_is_served_by_the_amf_of_its_amf_pointer)
{
  // Without the 5G-S-TMSI, the UE would go to AMF 0.
  start_cu_cp(255, 0);

  ASSERT_EQ(attach_ue_with_5g_s_tmsi(int_to_gnb_du_ue_f1ap_id(0), 1), 1);
}

TEST_F(cu_cp_amf_pooling_test, when_ue_sends_registered_amf_then_it_is_served_by_this_amf)
{
  // Without the registered AMF, the UE would go to AMF 0.
  start_cu_cp(255, 0);

  // Registered AMF with AMF Region ID 2, AMF Set ID 1 and AMF Pointer 1.
  asn1::rrc_nr::ul_dcch_msg_s rrc_setup_complete = test_helpers::create_rrc_setup_complete();
  auto&                       ies = rrc_setup_complete.msg.c1().rrc_setup_complete().crit_exts.rrc_setup_complete();
  ies.registered_amf_present      = true;
  ies.registered_amf.amf_id.from_number(0x020041);

  ASSERT_EQ(attach_ue(int_to_gnb_du_ue_f1ap_id(0), rrc_setup_complete), 1);
}

TEST_F(cu_cp_amf_pooling_test, when_one_amf_is_lost_then_only_its_ues_are_released_and_the_plmn_stays_served)
{
  start_cu_cp(255, 255);
  ASSERT_EQ(attach_ue_with_5g_s_tmsi(int_to_gnb_du_ue_f1ap_id(0), 0), 0);
  ASSERT_EQ(attach_ue_with_5g_s_tmsi(int_to_gnb_du_ue_f1ap_id(1), 1), 1);

  ASSERT_TRUE(drop_amf_connection(0));

  // The UE that AMF 0 serves is released.
  f1ap_message f1ap_pdu;
  ASSERT_TRUE(wait_for_f1ap_tx_pdu(du_idx, f1ap_pdu));
  ASSERT_TRUE(test_helpers::is_valid_ue_context_release_command(f1ap_pdu));
  ASSERT_EQ(int_to_gnb_du_ue_f1ap_id(f1ap_pdu.pdu.init_msg().value.ue_context_release_cmd()->gnb_du_ue_f1ap_id),
            int_to_gnb_du_ue_f1ap_id(0));
  get_du(du_idx).push_ul_pdu(test_helpers::generate_ue_context_release_complete(f1ap_pdu));

  // AMF 1 still serves the PLMN, so the cell stays active and the UE that AMF 1 serves stays connected.
  ASSERT_FALSE(wait_for_f1ap_tx_pdu(du_idx, f1ap_pdu, std::chrono::milliseconds{100}));

  // A new UE is served by AMF 1.
  ASSERT_EQ(attach_ue(int_to_gnb_du_ue_f1ap_id(2), test_helpers::create_rrc_setup_complete()), 1);
}

TEST_F(cu_cp_amf_pooling_test, when_other_amf_sends_dl_nas_transport_for_a_ue_then_it_takes_over_the_ue)
{
  start_cu_cp(255, 255);
  ASSERT_EQ(attach_ue_with_5g_s_tmsi(int_to_gnb_du_ue_f1ap_id(0), 0), 0);

  // AMF 1 takes over the UE from AMF 0 with the RAN UE NGAP ID of the Initial UE Message to AMF 0 (TS 38.413 section
  // 8.6.2.2).
  get_amf(1).push_tx_pdu(generate_downlink_nas_transport_message(uint_to_amf_ue_id(7), last_ran_ue_id));
  f1ap_message f1ap_pdu;
  ASSERT_TRUE(wait_for_f1ap_tx_pdu(du_idx, f1ap_pdu)) << "The NAS message of AMF 1 was not sent to the UE";
  ASSERT_TRUE(test_helpers::is_valid_dl_rrc_message_transfer(f1ap_pdu));

  // The UL NAS messages of the UE go to AMF 1.
  asn1::rrc_nr::ul_dcch_msg_s ul_dcch_msg;
  ul_dcch_msg.msg.set_c1().set_ul_info_transfer().crit_exts.set_ul_info_transfer().ded_nas_msg.from_string("7e004c");
  get_du(du_idx).push_rrc_ul_dcch_message(
      int_to_gnb_du_ue_f1ap_id(0), srb_id_t::srb1, test_helpers::pack_ul_dcch_msg(ul_dcch_msg));
  ngap_message ngap_pdu;
  ASSERT_TRUE(wait_for_ngap_tx_pdu(ngap_pdu, std::chrono::milliseconds{500}, 1)) << "AMF 1 received no UL NAS";
  ASSERT_TRUE(test_helpers::is_valid_ul_nas_transport_message(ngap_pdu));
  ASSERT_EQ(ngap_pdu.pdu.init_msg().value.ul_nas_transport()->amf_ue_ngap_id, 7);
  ASSERT_FALSE(get_amf(0).try_pop_rx_pdu(ngap_pdu)) << "AMF 0 still received a message for the UE";
}

TEST_F(cu_cp_amf_pooling_test, when_the_old_amf_uses_the_ue_after_a_take_over_then_the_ue_stays_with_the_new_amf)
{
  start_cu_cp(255, 255);
  ASSERT_EQ(attach_ue_with_5g_s_tmsi(int_to_gnb_du_ue_f1ap_id(0), 0), 0);
  get_amf(1).push_tx_pdu(generate_downlink_nas_transport_message(uint_to_amf_ue_id(7), last_ran_ue_id));
  f1ap_message f1ap_pdu;
  ASSERT_TRUE(wait_for_f1ap_tx_pdu(du_idx, f1ap_pdu)) << "The NAS message of AMF 1 was not sent to the UE";

  // AMF 0 sends a message for the UE after AMF 1 took it over. The UE association stays with AMF 1.
  get_amf(0).push_tx_pdu(generate_downlink_nas_transport_message(uint_to_amf_ue_id(5), last_ran_ue_id));
  ngap_message ngap_pdu;
  ASSERT_TRUE(wait_for_ngap_tx_pdu(ngap_pdu, std::chrono::milliseconds{500}, 0)) << "AMF 0 received no answer";
  ASSERT_TRUE(test_helpers::is_valid_error_indication(ngap_pdu));
  ASSERT_FALSE(get_du(du_idx).try_pop_dl_pdu(f1ap_pdu)) << "The NAS message of AMF 0 was sent to the UE";

  asn1::rrc_nr::ul_dcch_msg_s ul_dcch_msg;
  ul_dcch_msg.msg.set_c1().set_ul_info_transfer().crit_exts.set_ul_info_transfer().ded_nas_msg.from_string("7e004c");
  get_du(du_idx).push_rrc_ul_dcch_message(
      int_to_gnb_du_ue_f1ap_id(0), srb_id_t::srb1, test_helpers::pack_ul_dcch_msg(ul_dcch_msg));
  ASSERT_TRUE(wait_for_ngap_tx_pdu(ngap_pdu, std::chrono::milliseconds{500}, 1)) << "AMF 1 received no UL NAS";
  ASSERT_TRUE(test_helpers::is_valid_ul_nas_transport_message(ngap_pdu));
}

TEST_F(cu_cp_amf_pooling_test, when_other_amf_sends_initial_context_setup_request_for_a_ue_then_it_takes_over_the_ue)
{
  start_cu_cp(255, 255);
  ASSERT_EQ(attach_ue_with_5g_s_tmsi(int_to_gnb_du_ue_f1ap_id(0), 0), 0);

  // AMF 1 takes over the UE with the RAN UE NGAP ID of the Initial UE Message to AMF 0 (TS 38.413 section 8.3.1.2).
  get_amf(1).push_tx_pdu(generate_valid_initial_context_setup_request_message(uint_to_amf_ue_id(7), last_ran_ue_id));
  f1ap_message f1ap_pdu;
  ASSERT_TRUE(wait_for_f1ap_tx_pdu(du_idx, f1ap_pdu)) << "The CU-CP did not start the Initial Context Setup";
  ngap_message ngap_pdu;
  ASSERT_FALSE(get_amf(1).try_pop_rx_pdu(ngap_pdu)) << "AMF 1 got an answer before the Initial Context Setup ended";
  ASSERT_FALSE(get_amf(0).try_pop_rx_pdu(ngap_pdu)) << "AMF 0 still received a message for the UE";
}

TEST_F(cu_cp_amf_pooling_test, when_amf_requests_reroute_nas_then_initial_ue_message_goes_to_an_amf_of_the_set)
{
  // AMF 1 is the only AMF of the AMF Set 2.
  start_cu_cp(255, 255, 2);
  ASSERT_EQ(attach_ue_with_5g_s_tmsi(int_to_gnb_du_ue_f1ap_id(0), 0), 0);

  // AMF 0 requests to reroute the Initial UE Message to the AMF Set 2 (TS 38.413 section 8.6.5).
  get_amf(0).push_tx_pdu(generate_reroute_nas_request(last_ran_ue_id, last_initial_ue_message, 2));

  // AMF 1 receives the rerouted Initial UE Message, marked as rerouted by the AMF Set ID IE.
  ngap_message ngap_pdu;
  ASSERT_TRUE(wait_for_ngap_tx_pdu(ngap_pdu, std::chrono::milliseconds{500}, 1))
      << "AMF 1 received no InitialUEMessage";
  ASSERT_TRUE(test_helpers::is_valid_init_ue_message(ngap_pdu));
  const auto& init_ue_msg = ngap_pdu.pdu.init_msg().value.init_ue_msg();
  ASSERT_TRUE(init_ue_msg->amf_set_id_present);
  ASSERT_EQ(init_ue_msg->amf_set_id.to_number(), 2);
  ASSERT_EQ(init_ue_msg->nas_pdu, last_initial_ue_message.pdu.init_msg().value.init_ue_msg()->nas_pdu);
  ran_ue_id_t new_ran_ue_id = uint_to_ran_ue_id(init_ue_msg->ran_ue_ngap_id);

  // The NAS messages of AMF 1 reach the UE.
  get_amf(1).push_tx_pdu(generate_downlink_nas_transport_message(uint_to_amf_ue_id(9), new_ran_ue_id));
  f1ap_message f1ap_pdu;
  ASSERT_TRUE(wait_for_f1ap_tx_pdu(du_idx, f1ap_pdu)) << "The NAS message of AMF 1 was not sent to the UE";
  ASSERT_TRUE(test_helpers::is_valid_dl_rrc_message_transfer(f1ap_pdu));
  ASSERT_FALSE(get_amf(0).try_pop_rx_pdu(ngap_pdu)) << "AMF 0 still received a message for the UE";
}

TEST_F(cu_cp_amf_pooling_test, when_reroute_request_has_nssai_information_then_the_rerouted_message_carries_it)
{
  start_cu_cp(255, 255, 2);
  ASSERT_EQ(attach_ue_with_5g_s_tmsi(int_to_gnb_du_ue_f1ap_id(0), 0), 0);

  ngap_message reroute_msg = generate_reroute_nas_request(last_ran_ue_id, last_initial_ue_message, 2);
  auto&        reroute     = reroute_msg.pdu.init_msg().value.reroute_nas_request();
  asn1::ngap::allowed_nssai_item_s allowed;
  allowed.s_nssai.sst.from_number(1);
  reroute->allowed_nssai_present = true;
  reroute->allowed_nssai.push_back(allowed);
  reroute->partially_allowed_nssai_present = true;
  asn1::ngap::partially_allowed_nssai_item_s partially_allowed;
  partially_allowed.s_nssai.sst.from_number(3);
  reroute->partially_allowed_nssai.push_back(partially_allowed);
  reroute->source_to_target_amf_info_reroute_present                      = true;
  reroute->source_to_target_amf_info_reroute.rejected_nssai_in_ta_present = true;
  get_amf(0).push_tx_pdu(reroute_msg);

  // The NG-RAN node propagates the NSSAI information (TS 38.413 sections 8.6.1.2 and 8.6.5.2).
  ngap_message ngap_pdu;
  ASSERT_TRUE(wait_for_ngap_tx_pdu(ngap_pdu, std::chrono::milliseconds{500}, 1))
      << "AMF 1 received no InitialUEMessage";
  const auto& init_ue_msg = ngap_pdu.pdu.init_msg().value.init_ue_msg();
  ASSERT_TRUE(init_ue_msg->allowed_nssai_present);
  ASSERT_EQ(init_ue_msg->allowed_nssai.size(), 1);
  ASSERT_TRUE(init_ue_msg->partially_allowed_nssai_present);
  ASSERT_EQ(init_ue_msg->partially_allowed_nssai.size(), 1);
  ASSERT_TRUE(init_ue_msg->source_to_target_amf_info_reroute_present);
}

TEST_F(cu_cp_amf_pooling_test, when_reroute_request_names_an_unknown_ue_then_the_error_indication_has_its_id)
{
  start_cu_cp(255, 255, 2);
  ASSERT_EQ(attach_ue_with_5g_s_tmsi(int_to_gnb_du_ue_f1ap_id(0), 0), 0);

  get_amf(0).push_tx_pdu(generate_reroute_nas_request(uint_to_ran_ue_id(999), last_initial_ue_message, 2));

  // The Error Indication includes the AP ID received from the AMF (TS 38.413 section 10.6).
  ngap_message ngap_pdu;
  ASSERT_TRUE(wait_for_ngap_tx_pdu(ngap_pdu, std::chrono::milliseconds{500}, 0)) << "AMF 0 received no answer";
  ASSERT_TRUE(test_helpers::is_valid_error_indication(ngap_pdu));
  ASSERT_TRUE(ngap_pdu.pdu.init_msg().value.error_ind()->ran_ue_ngap_id_present);
  ASSERT_EQ(ngap_pdu.pdu.init_msg().value.error_ind()->ran_ue_ngap_id, 999);
}

TEST_F(cu_cp_amf_pooling_test, when_the_requesting_amf_is_in_the_target_amf_set_then_another_amf_is_selected)
{
  // Both AMFs are in the AMF Set 1. Without the exclusion of AMF 0, most rerouted UEs would go back to AMF 0.
  start_cu_cp(255, 1);
  ASSERT_EQ(attach_ue_with_5g_s_tmsi(int_to_gnb_du_ue_f1ap_id(0), 0), 0);

  get_amf(0).push_tx_pdu(generate_reroute_nas_request(last_ran_ue_id, last_initial_ue_message, 1));

  ngap_message ngap_pdu;
  ASSERT_TRUE(wait_for_ngap_tx_pdu(ngap_pdu, std::chrono::milliseconds{500}, 1))
      << "AMF 1 received no InitialUEMessage";
  ASSERT_TRUE(test_helpers::is_valid_init_ue_message(ngap_pdu));
}

TEST_F(cu_cp_amf_pooling_test, when_no_other_amf_of_the_target_amf_set_is_connected_then_the_ue_is_released)
{
  start_cu_cp(255, 255, 2);
  ASSERT_EQ(attach_ue_with_5g_s_tmsi(int_to_gnb_du_ue_f1ap_id(0), 0), 0);

  get_amf(0).push_tx_pdu(generate_reroute_nas_request(last_ran_ue_id, last_initial_ue_message, 3));

  f1ap_message f1ap_pdu;
  ASSERT_TRUE(wait_for_f1ap_tx_pdu(du_idx, f1ap_pdu)) << "The UE was not released";
  ASSERT_TRUE(test_helpers::is_valid_ue_context_release_command(f1ap_pdu));
}

TEST_F(cu_cp_amf_pooling_test, when_reroute_request_lists_an_s_nssai_twice_then_the_ue_is_released)
{
  start_cu_cp(255, 255, 2);
  ASSERT_EQ(attach_ue_with_5g_s_tmsi(int_to_gnb_du_ue_f1ap_id(0), 0), 0);

  // The S-NSSAI is in the Allowed NSSAI and in the Partially Allowed NSSAI (TS 38.413 section 8.6.5.3).
  ngap_message reroute_msg = generate_reroute_nas_request(last_ran_ue_id, last_initial_ue_message, 2);
  auto&        reroute     = reroute_msg.pdu.init_msg().value.reroute_nas_request();
  asn1::ngap::allowed_nssai_item_s allowed;
  allowed.s_nssai.sst.from_number(1);
  reroute->allowed_nssai_present = true;
  reroute->allowed_nssai.push_back(allowed);
  reroute->partially_allowed_nssai_present = true;
  asn1::ngap::partially_allowed_nssai_item_s partially_allowed;
  partially_allowed.s_nssai.sst.from_number(1);
  reroute->partially_allowed_nssai.push_back(partially_allowed);
  get_amf(0).push_tx_pdu(reroute_msg);

  f1ap_message f1ap_pdu;
  ASSERT_TRUE(wait_for_f1ap_tx_pdu(du_idx, f1ap_pdu)) << "The UE was not released";
  ASSERT_TRUE(test_helpers::is_valid_ue_context_release_command(f1ap_pdu));
  ngap_message ngap_pdu;
  ASSERT_FALSE(get_amf(1).try_pop_rx_pdu(ngap_pdu)) << "AMF 1 received a rerouted InitialUEMessage";
}

TEST_F(cu_cp_amf_pooling_test, when_amf_updates_its_capacity_to_zero_then_new_ues_are_served_by_the_other_amf)
{
  start_cu_cp(255, 255);

  // AMF 0 sets its Relative AMF Capacity to 0 (TS 38.413 section 8.7.3).
  ngap_message update                                                      = generate_amf_configuration_update();
  update.pdu.init_msg().value.amf_cfg_upd()->relative_amf_capacity_present = true;
  update.pdu.init_msg().value.amf_cfg_upd()->relative_amf_capacity         = 0;
  get_amf(0).push_tx_pdu(update);
  ngap_message ngap_pdu;
  ASSERT_TRUE(wait_for_ngap_tx_pdu(ngap_pdu, std::chrono::milliseconds{500}, 0)) << "No acknowledge was sent";
  ASSERT_EQ(ngap_pdu.pdu.type().value, asn1::ngap::ngap_pdu_c::types_opts::successful_outcome);
  ASSERT_EQ(ngap_pdu.pdu.successful_outcome().value.type().value,
            asn1::ngap::ngap_elem_procs_o::successful_outcome_c::types_opts::amf_cfg_upd_ack);

  for (unsigned i = 0; i != 5; ++i) {
    ASSERT_EQ(attach_ue(int_to_gnb_du_ue_f1ap_id(i), test_helpers::create_rrc_setup_complete()), 1);
  }
}

TEST_F(cu_cp_amf_pooling_test, when_guami_of_the_ue_is_unavailable_then_the_ue_is_served_by_the_backup_amf)
{
  // AMF 0 also serves the GUAMI with AMF Pointer 2, which stays available. Without the backup AMF, the UE would stay
  // with AMF 0, which keeps serving its AMF Set.
  ngap_message amf_0_response = generate_ng_setup_response(plmn_identity::test_value(), 0, 255);
  auto&        served_guamis  = amf_0_response.pdu.successful_outcome().value.ng_setup_resp()->served_guami_list;
  asn1::ngap::served_guami_item_s second_guami = served_guamis[0];
  second_guami.guami.amf_pointer.from_number(2);
  served_guamis.push_back(second_guami);
  start_cu_cp(amf_0_response, generate_ng_setup_response(plmn_identity::test_value(), 1, 0));

  // AMF 0 marks its GUAMI as unavailable and names AMF 1 as backup (TS 38.413 section 8.7.6).
  get_amf(0).push_tx_pdu(generate_amf_status_indication(
      guami_t{.plmn = plmn_identity::test_value(), .amf_set_id = 1, .amf_pointer = 0, .amf_region_id = 2}, amf_1_name));

  ASSERT_EQ(attach_ue_with_5g_s_tmsi(int_to_gnb_du_ue_f1ap_id(0), 0), 1);
}

TEST_F(cu_cp_amf_pooling_test, when_amf_rejects_the_establishment_cause_then_the_ue_is_served_by_the_other_amf)
{
  start_cu_cp(255, 255);

  // AMF 0 rejects the RRC connection establishments for signalling (TS 38.413 section 8.7.7). The UE sends the
  // establishment cause mo-Signalling.
  get_amf(0).push_tx_pdu(generate_overload_start(asn1::ngap::overload_action_opts::reject_rrc_cr_sig));

  // Without the overload, the UE would go to AMF 0, which serves its AMF Pointer.
  ASSERT_EQ(attach_ue_with_5g_s_tmsi(int_to_gnb_du_ue_f1ap_id(0), 0), 1);
}

TEST_F(cu_cp_amf_pooling_test, when_every_amf_rejects_the_establishment_cause_then_rrc_setup_is_rejected)
{
  start_cu_cp(255, 255);

  // Both AMFs reject the RRC connection establishments for signalling (TS 38.413 section 8.7.7). The UE sends the
  // establishment cause mo-Signalling.
  get_amf(0).push_tx_pdu(generate_overload_start(asn1::ngap::overload_action_opts::reject_rrc_cr_sig));
  get_amf(1).push_tx_pdu(generate_overload_start(asn1::ngap::overload_action_opts::reject_rrc_cr_sig));

  get_du(du_idx).push_ul_pdu(
      test_helpers::generate_init_ul_rrc_message_transfer(int_to_gnb_du_ue_f1ap_id(0), to_rnti(0x4601)));
  f1ap_message f1ap_pdu;
  ASSERT_TRUE(wait_for_f1ap_tx_pdu(du_idx, f1ap_pdu)) << "No response was sent to the UE";
  ASSERT_TRUE(test_helpers::is_valid_ue_context_release_command(f1ap_pdu)) << "The UE was not rejected";
  asn1::rrc_nr::dl_ccch_msg_s ccch;
  asn1::cbit_ref              bref{f1ap_pdu.pdu.init_msg().value.ue_context_release_cmd()->rrc_container};
  ASSERT_EQ(ccch.unpack(bref), asn1::OCUDUASN_SUCCESS);
  ASSERT_EQ(ccch.msg.c1().type().value, asn1::rrc_nr::dl_ccch_msg_type_c::c1_c_::types_opts::rrc_reject);
  ASSERT_TRUE(ccch.msg.c1().rrc_reject().crit_exts.rrc_reject().wait_time_present);
}

TEST_F(cu_cp_amf_pooling_test, when_ue_requests_a_slice_then_it_is_served_by_the_amf_that_supports_it)
{
  // AMF 1 also supports the slice with SST 2 and SD 0x10. Without the requested slice, the UE would go to AMF 0.
  ngap_message                     amf_1_response = generate_ng_setup_response(plmn_identity::test_value(), 1, 0);
  asn1::ngap::slice_support_item_s slice;
  slice.s_nssai.sst.from_number(2);
  slice.s_nssai.sd_present = true;
  slice.s_nssai.sd.from_number(0x10);
  amf_1_response.pdu.successful_outcome().value.ng_setup_resp()->plmn_support_list[0].slice_support_list.push_back(
      slice);
  start_cu_cp(generate_ng_setup_response(plmn_identity::test_value(), 0, 255), amf_1_response);

  asn1::rrc_nr::ul_dcch_msg_s rrc_setup_complete = test_helpers::create_rrc_setup_complete();
  auto&                       ies = rrc_setup_complete.msg.c1().rrc_setup_complete().crit_exts.rrc_setup_complete();
  ies.s_nssai_list.resize(1);
  ies.s_nssai_list[0].set_sst_sd().from_number((2U << 24U) + 0x10U);

  ASSERT_EQ(attach_ue(int_to_gnb_du_ue_f1ap_id(0), rrc_setup_complete), 1);
}

TEST_F(cu_cp_amf_pooling_test, when_the_first_amf_fails_ng_setup_on_startup_then_the_other_amf_is_still_set_up)
{
  get_amf(0).enqueue_next_tx_pdu(generate_ng_setup_failure());
  get_amf(1).enqueue_next_tx_pdu(generate_ng_setup_response(plmn_identity::test_value(), 1, 255));

  get_cu_cp().start();
  ngap_message ngap_pdu;
  ASSERT_TRUE(get_amf(0).try_pop_rx_pdu(ngap_pdu)) << "AMF 0 received no NG Setup Request";

  // The CU-CP did not stop at the failed AMF and sent the NG Setup Request to the other AMF.
  ASSERT_TRUE(get_amf(1).try_pop_rx_pdu(ngap_pdu)) << "AMF 1 received no NG Setup Request";
  ASSERT_TRUE(is_pdu_type(ngap_pdu, asn1::ngap::ngap_elem_procs_o::init_msg_c::types::ng_setup_request));

  // AMF 1 serves the PLMN, so the cell of a new DU is activated and new UEs are served by AMF 1.
  std::optional<unsigned> ret = connect_new_du();
  ASSERT_TRUE(ret.has_value());
  du_idx = ret.value();
  ASSERT_TRUE(run_f1_setup(du_idx));
  ret = connect_new_cu_up();
  ASSERT_TRUE(ret.has_value());
  ASSERT_TRUE(run_e1_setup(ret.value()));
  ASSERT_EQ(attach_ue(int_to_gnb_du_ue_f1ap_id(0), test_helpers::create_rrc_setup_complete()), 1);
}

TEST_F(cu_cp_amf_pooling_test, when_the_first_amf_is_unreachable_on_startup_then_the_other_amf_is_still_set_up)
{
  // AMF 0 cannot be reached, so the CU-CP cannot set up its TNL association.
  get_amf(0).drop_connection();
  get_amf(1).enqueue_next_tx_pdu(generate_ng_setup_response(plmn_identity::test_value(), 1, 255));

  get_cu_cp().start();

  // The CU-CP did not stop at the unreachable AMF and sent the NG Setup Request to the other AMF.
  ngap_message ngap_pdu;
  ASSERT_TRUE(get_amf(1).try_pop_rx_pdu(ngap_pdu)) << "AMF 1 received no NG Setup Request";
  ASSERT_TRUE(is_pdu_type(ngap_pdu, asn1::ngap::ngap_elem_procs_o::init_msg_c::types::ng_setup_request));

  // AMF 1 serves the PLMN, so the cell of a new DU is activated and new UEs are served by AMF 1.
  std::optional<unsigned> ret = connect_new_du();
  ASSERT_TRUE(ret.has_value());
  du_idx = ret.value();
  ASSERT_TRUE(run_f1_setup(du_idx));
  ret = connect_new_cu_up();
  ASSERT_TRUE(ret.has_value());
  ASSERT_TRUE(run_e1_setup(ret.value()));
  ASSERT_EQ(attach_ue(int_to_gnb_du_ue_f1ap_id(0), test_helpers::create_rrc_setup_complete()), 1);
}

TEST_F(cu_cp_amf_pooling_test, when_the_last_amf_is_lost_and_another_amf_connects_later_then_the_plmn_is_unblocked)
{
  // AMF 1 is down on startup, so only AMF 0 connects. AMF 1 is retried in the background.
  get_amf(0).enqueue_next_tx_pdu(generate_ng_setup_response(plmn_identity::test_value(), 0, 255));
  get_amf(1).drop_connection();
  get_cu_cp().start();
  ngap_message ngap_pdu;
  ASSERT_TRUE(get_amf(0).try_pop_rx_pdu(ngap_pdu)) << "AMF 0 received no NG Setup Request";

  std::optional<unsigned> ret = connect_new_du();
  ASSERT_TRUE(ret.has_value());
  du_idx = ret.value();
  ASSERT_TRUE(run_f1_setup(du_idx));
  ret = connect_new_cu_up();
  ASSERT_TRUE(ret.has_value());
  ASSERT_TRUE(run_e1_setup(ret.value()));
  ASSERT_EQ(attach_ue(int_to_gnb_du_ue_f1ap_id(0), test_helpers::create_rrc_setup_complete()), 0);

  // AMF 0, the only connected AMF of the PLMN, is lost: its UE is released and the cell is deactivated.
  ASSERT_TRUE(drop_amf_connection(0));
  f1ap_message f1ap_pdu;
  ASSERT_TRUE(wait_for_f1ap_tx_pdu(du_idx, f1ap_pdu));
  ASSERT_TRUE(test_helpers::is_valid_ue_context_release_command(f1ap_pdu));
  get_du(du_idx).push_ul_pdu(test_helpers::generate_ue_context_release_complete(f1ap_pdu));
  ASSERT_TRUE(wait_for_f1ap_tx_pdu(du_idx, f1ap_pdu));
  ASSERT_TRUE(test_helpers::is_valid_gnb_cu_configuration_update(f1ap_pdu));
  ASSERT_TRUE(f1ap_pdu.pdu.init_msg().value.gnb_cu_cfg_upd()->cells_to_be_deactiv_list_present);
  f1ap_message ack = test_helpers::generate_gnb_cu_configuration_update_acknowledgement({});
  ack.pdu.successful_outcome().value.gnb_cu_cfg_upd_ack()->transaction_id =
      f1ap_pdu.pdu.init_msg().value.gnb_cu_cfg_upd()->transaction_id;
  get_du(du_idx).push_ul_pdu(ack);

  // AMF 1 comes up and connects through the background retry, so the cell is activated again.
  ASSERT_TRUE(reconnect_amf(1)) << "AMF 1 did not connect";
  ASSERT_TRUE(wait_for_f1ap_tx_pdu(du_idx, f1ap_pdu, std::chrono::milliseconds{1000}));
  ASSERT_TRUE(test_helpers::is_valid_gnb_cu_configuration_update(f1ap_pdu));
  ASSERT_TRUE(f1ap_pdu.pdu.init_msg().value.gnb_cu_cfg_upd()->cells_to_be_activ_list_present);
  ack.pdu.successful_outcome().value.gnb_cu_cfg_upd_ack()->transaction_id =
      f1ap_pdu.pdu.init_msg().value.gnb_cu_cfg_upd()->transaction_id;
  get_du(du_idx).push_ul_pdu(ack);

  // The PLMN is not blocked any more: a new UE is admitted and served by AMF 1.
  ASSERT_EQ(attach_ue(int_to_gnb_du_ue_f1ap_id(1), test_helpers::create_rrc_setup_complete()), 1);
}
