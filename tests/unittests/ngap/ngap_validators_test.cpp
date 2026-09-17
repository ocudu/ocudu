// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "lib/ngap/ngap_asn1_helpers.h"
#include "lib/ngap/ngap_validators/ngap_validators.h"
#include "ngap_test_helpers.h"
#include "ocudu/adt/format.h"
#include "ocudu/cu_cp/cu_cp_configuration_helpers.h"
#include "ocudu/ngap/ngap_types.h"
#include "ocudu/ran/cu_cp_types.h"
#include "ocudu/ran/cu_types.h"
#include <functional>
#include <gtest/gtest.h>
#include <sys/types.h>

using namespace ocudu;
using namespace ocucp;

class ngap_validator_test : public ngap_test
{
public:
  asn1::ngap::pdu_session_res_setup_item_su_req_s generate_pdu_session_resource_setup_item(pdu_session_id_t psi)
  {
    asn1::ngap::pdu_session_res_setup_item_su_req_s pdu_session_res_item;

    pdu_session_res_item.pdu_session_id = to_underlying(psi);

    // Fill PDU Session NAS PDU.
    pdu_session_res_item.pdu_session_nas_pdu.from_string("7e02e9b0a23c027e006801006e2e0115c211000901000631310101ff08060"
                                                         "6014a06014a2905010c02010c2204010027db79000608204101"
                                                         "01087b002080802110030000108106ac1503648306ac150364000d04ac150"
                                                         "364001002054e251c036f61690469707634066d6e6330393906"
                                                         "6d636332303804677072731201");

    // Fill S-NSSAI.
    pdu_session_res_item.s_nssai.sst.from_number(1);
    pdu_session_res_item.s_nssai.sd_present = true;
    pdu_session_res_item.s_nssai.sd.from_string("0027db");

    // Fill PDU Session Resource Setup Request Transfer.
    pdu_session_res_item.pdu_session_res_setup_request_transfer.from_string(
        "0000040082000a0c400000003040000000008b000a01f00a321302000028d600860001000088000700010000091c00");

    return pdu_session_res_item;
  }

  ngap_message
  generate_pdu_session_resource_setup_request_with_one_unique_and_two_duplicate_pdu_sessions(amf_ue_id_t amf_ue_id,
                                                                                             ran_ue_id_t ran_ue_id)
  {
    ngap_message ngap_msg = generate_invalid_pdu_session_resource_setup_request_message(amf_ue_id, ran_ue_id);

    auto& pdu_session_res_setup_req = ngap_msg.pdu.init_msg().value.pdu_session_res_setup_request();

    // Fill unique PDU session.
    pdu_session_id_t unique_psi = uint_to_pdu_session_id(2);
    pdu_session_res_setup_req->pdu_session_res_setup_list_su_req.push_back(
        generate_pdu_session_resource_setup_item(unique_psi));

    return ngap_msg;
  }

  ngap_message
  generate_pdu_session_resource_setup_request_with_one_already_setup_and_one_new_pdu_session(amf_ue_id_t amf_ue_id,
                                                                                             ran_ue_id_t ran_ue_id)
  {
    pdu_session_id_t setup_psi = uint_to_pdu_session_id(1);
    ngap_message     ngap_msg  = generate_valid_pdu_session_resource_setup_request_message(
        amf_ue_id, ran_ue_id, {{setup_psi, {pdu_session_type_t::ipv4, {{uint_to_qos_flow_id(1), 9}}}}});

    auto& pdu_session_res_setup_req = ngap_msg.pdu.init_msg().value.pdu_session_res_setup_request();

    // Fill unique PDU session.
    pdu_session_id_t new_psi = uint_to_pdu_session_id(2);
    pdu_session_res_setup_req->pdu_session_res_setup_list_su_req.push_back(
        generate_pdu_session_resource_setup_item(new_psi));

    return ngap_msg;
  }

  ngap_message generate_pdu_session_resource_setup_request_with_non_gbr_qos_flows_and_no_aggregate_maximum_bitrate(
      amf_ue_id_t      amf_ue_id,
      ran_ue_id_t      ran_ue_id,
      pdu_session_id_t psi)
  {
    ngap_message ngap_msg = generate_valid_pdu_session_resource_setup_request_message(
        amf_ue_id, ran_ue_id, {{psi, {pdu_session_type_t::ipv4, {{uint_to_qos_flow_id(1), 9}}}}});

    auto& asn1_request                         = ngap_msg.pdu.init_msg().value.pdu_session_res_setup_request();
    asn1_request->ue_aggr_max_bit_rate_present = false;

    // Fill PDU Session Resource Setup Request Transfer.
    asn1::ngap::pdu_session_res_setup_request_transfer_s asn1_setup_req_transfer;

    asn1_setup_req_transfer->ul_ngu_up_tnl_info.set_gtp_tunnel();
    auto addr = transport_layer_address::create_from_string("127.0.0.1");
    tla_to_asn1_bitstring(asn1_setup_req_transfer->ul_ngu_up_tnl_info.gtp_tunnel().transport_layer_address, addr);
    asn1_setup_req_transfer->ul_ngu_up_tnl_info.gtp_tunnel().gtp_teid.from_number(1);

    asn1_setup_req_transfer->pdu_session_type = asn1::ngap::pdu_session_type_opts::options::ipv4;

    asn1::ngap::qos_flow_setup_request_item_s asn1_qos_flow_item;
    asn1_qos_flow_item.qos_flow_id = 1;

    asn1_qos_flow_item.qos_flow_level_qos_params.qos_characteristics.set_dyn5qi();
    asn1_qos_flow_item.qos_flow_level_qos_params.qos_characteristics.dyn5qi().prio_level_qos                 = 1;
    asn1_qos_flow_item.qos_flow_level_qos_params.qos_characteristics.dyn5qi().packet_delay_budget            = 1;
    asn1_qos_flow_item.qos_flow_level_qos_params.qos_characteristics.dyn5qi().packet_error_rate.per_exponent = 1;
    asn1_qos_flow_item.qos_flow_level_qos_params.qos_characteristics.dyn5qi().packet_error_rate.per_scalar   = 1;

    asn1_qos_flow_item.qos_flow_level_qos_params.alloc_and_retention_prio.prio_level_arp = 1;
    asn1_qos_flow_item.qos_flow_level_qos_params.alloc_and_retention_prio.pre_emption_cap =
        asn1::ngap::pre_emption_cap_opts::options::shall_not_trigger_pre_emption;
    asn1_qos_flow_item.qos_flow_level_qos_params.alloc_and_retention_prio.pre_emption_vulnerability =
        asn1::ngap::pre_emption_vulnerability_opts::options::not_pre_emptable;

    asn1_qos_flow_item.qos_flow_level_qos_params.add_qos_flow_info_present = true;
    asn1_qos_flow_item.qos_flow_level_qos_params.add_qos_flow_info =
        asn1::ngap::add_qos_flow_info_opts::options::more_likely;
    asn1_qos_flow_item.qos_flow_level_qos_params.reflective_qos_attribute_present = true;
    asn1_qos_flow_item.qos_flow_level_qos_params.reflective_qos_attribute =
        asn1::ngap::reflective_qos_attribute_opts::options::subject_to;

    asn1_setup_req_transfer->qos_flow_setup_request_list.push_back(asn1_qos_flow_item);

    // Pack pdu_session_res_release_resp_transfer_s.
    asn1_request->pdu_session_res_setup_list_su_req.begin()->pdu_session_res_setup_request_transfer =
        pack_into_pdu(asn1_setup_req_transfer);

    return ngap_msg;
  }

  /// \brief Apply \c edit to the PDU Session Resource Setup Request Transfer of the first PDU session of the request.
  [[nodiscard]] bool
  edit_setup_request_transfer(ngap_message& ngap_msg,
                              const std::function<void(asn1::ngap::pdu_session_res_setup_request_transfer_s&)>& edit)
  {
    auto& asn1_request = ngap_msg.pdu.init_msg().value.pdu_session_res_setup_request();
    auto& asn1_item    = *asn1_request->pdu_session_res_setup_list_su_req.begin();

    asn1::ngap::pdu_session_res_setup_request_transfer_s asn1_transfer;
    asn1::cbit_ref                                       bref(asn1_item.pdu_session_res_setup_request_transfer);
    if (asn1_transfer.unpack(bref) != asn1::OCUDUASN_SUCCESS) {
      return false;
    }

    edit(asn1_transfer);

    asn1_item.pdu_session_res_setup_request_transfer = pack_into_pdu(asn1_transfer);

    return true;
  }

  /// \brief Remove the GBR QoS Flow Information IE from a QoS flow of the first PDU session of the given request.
  [[nodiscard]] bool remove_gbr_qos_information(ngap_message& ngap_msg, qos_flow_id_t qos_flow_id)
  {
    return edit_setup_request_transfer(
        ngap_msg, [qos_flow_id](asn1::ngap::pdu_session_res_setup_request_transfer_s& asn1_transfer) {
          for (auto& asn1_qos_flow : asn1_transfer->qos_flow_setup_request_list) {
            if (asn1_qos_flow.qos_flow_id == to_underlying(qos_flow_id)) {
              asn1_qos_flow.qos_flow_level_qos_params.gbr_qos_info_present = false;
            }
          }
        });
  }

  /// \brief Remove the PDU Session Aggregate Maximum Bit Rate IE from the first PDU session of the given request.
  [[nodiscard]] bool remove_pdu_session_aggregate_maximum_bit_rate(ngap_message& ngap_msg)
  {
    return edit_setup_request_transfer(ngap_msg,
                                       [](asn1::ngap::pdu_session_res_setup_request_transfer_s& asn1_transfer) {
                                         asn1_transfer->pdu_session_aggr_max_bit_rate_present = false;
                                       });
  }

  /// \brief Generate a PDU Session Resource Setup Request with a single delay critical GBR QoS flow that uses a
  /// dynamic 5QI.
  ngap_message generate_pdu_session_resource_setup_request_with_delay_critical_qos_flow(amf_ue_id_t      amf_ue_id,
                                                                                        ran_ue_id_t      ran_ue_id,
                                                                                        pdu_session_id_t psi,
                                                                                        bool set_max_data_burst_volume)
  {
    ngap_message ngap_msg = generate_valid_pdu_session_resource_setup_request_message(
        amf_ue_id, ran_ue_id, {{psi, {pdu_session_type_t::ipv4, {{uint_to_qos_flow_id(1), 9}}}}});

    auto& asn1_request = ngap_msg.pdu.init_msg().value.pdu_session_res_setup_request();

    // Fill PDU Session Resource Setup Request Transfer.
    asn1::ngap::pdu_session_res_setup_request_transfer_s asn1_setup_req_transfer;

    asn1_setup_req_transfer->pdu_session_aggr_max_bit_rate_present                          = true;
    asn1_setup_req_transfer->pdu_session_aggr_max_bit_rate.pdu_session_aggr_max_bit_rate_dl = 1000000000U;
    asn1_setup_req_transfer->pdu_session_aggr_max_bit_rate.pdu_session_aggr_max_bit_rate_ul = 1000000000U;

    asn1_setup_req_transfer->ul_ngu_up_tnl_info.set_gtp_tunnel();
    auto addr = transport_layer_address::create_from_string("127.0.0.1");
    tla_to_asn1_bitstring(asn1_setup_req_transfer->ul_ngu_up_tnl_info.gtp_tunnel().transport_layer_address, addr);
    asn1_setup_req_transfer->ul_ngu_up_tnl_info.gtp_tunnel().gtp_teid.from_number(1);

    asn1_setup_req_transfer->pdu_session_type = asn1::ngap::pdu_session_type_opts::options::ipv4;

    asn1::ngap::qos_flow_setup_request_item_s asn1_qos_flow_item;
    asn1_qos_flow_item.qos_flow_id = 1;

    auto& asn1_dyn_5qi               = asn1_qos_flow_item.qos_flow_level_qos_params.qos_characteristics.set_dyn5qi();
    asn1_dyn_5qi.prio_level_qos      = 1;
    asn1_dyn_5qi.packet_delay_budget = 1;
    asn1_dyn_5qi.packet_error_rate.per_exponent = 1;
    asn1_dyn_5qi.packet_error_rate.per_scalar   = 1;
    asn1_dyn_5qi.delay_crit_present             = true;
    asn1_dyn_5qi.delay_crit                     = asn1::ngap::delay_crit_opts::delay_crit;
    asn1_dyn_5qi.averaging_win_present          = true;
    asn1_dyn_5qi.averaging_win                  = 2000;
    if (set_max_data_burst_volume) {
      asn1_dyn_5qi.max_data_burst_volume_present = true;
      asn1_dyn_5qi.max_data_burst_volume         = 1000;
    }

    asn1_qos_flow_item.qos_flow_level_qos_params.alloc_and_retention_prio.prio_level_arp = 1;
    asn1_qos_flow_item.qos_flow_level_qos_params.alloc_and_retention_prio.pre_emption_cap =
        asn1::ngap::pre_emption_cap_opts::options::shall_not_trigger_pre_emption;
    asn1_qos_flow_item.qos_flow_level_qos_params.alloc_and_retention_prio.pre_emption_vulnerability =
        asn1::ngap::pre_emption_vulnerability_opts::options::not_pre_emptable;

    // The GBR QoS Flow Information IE is present, as required for a GBR QoS flow.
    asn1_qos_flow_item.qos_flow_level_qos_params.gbr_qos_info_present                     = true;
    asn1_qos_flow_item.qos_flow_level_qos_params.gbr_qos_info.max_flow_bit_rate_dl        = 1000000U;
    asn1_qos_flow_item.qos_flow_level_qos_params.gbr_qos_info.max_flow_bit_rate_ul        = 1000000U;
    asn1_qos_flow_item.qos_flow_level_qos_params.gbr_qos_info.guaranteed_flow_bit_rate_dl = 1000000U;
    asn1_qos_flow_item.qos_flow_level_qos_params.gbr_qos_info.guaranteed_flow_bit_rate_ul = 1000000U;

    asn1_setup_req_transfer->qos_flow_setup_request_list.push_back(asn1_qos_flow_item);

    asn1_request->pdu_session_res_setup_list_su_req.begin()->pdu_session_res_setup_request_transfer =
        pack_into_pdu(asn1_setup_req_transfer);

    return ngap_msg;
  }

  asn1::ngap::pdu_session_res_modify_item_mod_req_s generate_pdu_session_resource_modify_item(pdu_session_id_t psi)
  {
    asn1::ngap::pdu_session_res_modify_item_mod_req_s pdu_session_res_item;

    pdu_session_res_item.pdu_session_id = to_underlying(psi);

    // Fill PDU session resource modify request transfer.
    asn1::ngap::pdu_session_res_modify_request_transfer_s pdu_session_res_modify_request_transfer;

    // Fill QoS flow add or modify request item.
    pdu_session_res_modify_request_transfer->qos_flow_add_or_modify_request_list_present = true;
    asn1::ngap::qos_flow_add_or_modify_request_item_s qos_flow_add_item;

    // Fill QoS flow identifier.
    qos_flow_add_item.qos_flow_id = 1;

    pdu_session_res_modify_request_transfer->qos_flow_add_or_modify_request_list.push_back(qos_flow_add_item);

    pdu_session_res_item.pdu_session_res_modify_request_transfer =
        pack_into_pdu(pdu_session_res_modify_request_transfer);

    return pdu_session_res_item;
  }

  ngap_message generate_pdu_session_resource_modify_request_with_one_unique_and_two_duplicate_pdu_sessions(
      amf_ue_id_t      amf_ue_id,
      ran_ue_id_t      ran_ue_id,
      pdu_session_id_t unique_psi,
      pdu_session_id_t duplicate_psi)
  {
    ngap_message ngap_msg =
        generate_invalid_pdu_session_resource_modify_request_message(amf_ue_id, ran_ue_id, duplicate_psi);

    auto& pdu_session_res_modify_req = ngap_msg.pdu.init_msg().value.pdu_session_res_modify_request();

    // Fill unique PDU session.
    pdu_session_res_modify_req->pdu_session_res_modify_list_mod_req.push_back(
        generate_pdu_session_resource_modify_item(unique_psi));

    return ngap_msg;
  }
};

// Test handling of valid PDU session resource modification request.
TEST_F(ngap_validator_test, when_valid_request_received_then_pdu_session_setup_succeeds)
{
  cu_cp_ue_index_t ue_index  = uint_to_ue_index(0);
  amf_ue_id_t      amf_ue_id = uint_to_amf_ue_id(0);
  ran_ue_id_t      ran_ue_id = uint_to_ran_ue_id(0);

  ngap_message ngap_msg = generate_valid_pdu_session_resource_setup_request_message(
      amf_ue_id, ran_ue_id, {{uint_to_pdu_session_id(1), {pdu_session_type_t::ipv4, {{uint_to_qos_flow_id(1), 9}}}}});

  auto& asn1_request = ngap_msg.pdu.init_msg().value.pdu_session_res_setup_request();

  ngap_pdu_session_resource_setup_request request;
  fill_ngap_pdu_session_resource_setup_request(request, asn1_request->pdu_session_res_setup_list_su_req);

  ngap_ue_logger ue_logger{"NGAP", {ue_index, ran_ue_id}};
  // Verify PDU session resource setup request.
  auto verification_outcome = verify_pdu_session_resource_setup_request(request, asn1_request, ue_logger);

  ASSERT_EQ(verification_outcome.request.pdu_session_res_setup_items.size(), 1U);
  ASSERT_EQ(verification_outcome.response.pdu_session_res_setup_response_items.size(), 0U);
  ASSERT_EQ(verification_outcome.response.pdu_session_res_failed_to_setup_items.size(), 0U);
}

// Test handling of duplicate PDU session ids in PDU session resource setup request.
TEST_F(ngap_validator_test, when_duplicate_pdu_session_id_then_pdu_session_setup_fails)
{
  cu_cp_ue_index_t ue_index  = uint_to_ue_index(0);
  amf_ue_id_t      amf_ue_id = uint_to_amf_ue_id(0);
  ran_ue_id_t      ran_ue_id = uint_to_ran_ue_id(0);

  ngap_message ngap_msg = generate_invalid_pdu_session_resource_setup_request_message(amf_ue_id, ran_ue_id);

  auto& asn1_request = ngap_msg.pdu.init_msg().value.pdu_session_res_setup_request();

  ngap_pdu_session_resource_setup_request request;
  fill_ngap_pdu_session_resource_setup_request(request, asn1_request->pdu_session_res_setup_list_su_req);

  ngap_ue_logger ue_logger{"NGAP", {ue_index, ran_ue_id}};
  // Verify PDU session resource setup request.
  auto verification_outcome = verify_pdu_session_resource_setup_request(request, asn1_request, ue_logger);

  ASSERT_TRUE(verification_outcome.request.pdu_session_res_setup_items.empty());
  ASSERT_EQ(verification_outcome.response.pdu_session_res_failed_to_setup_items.size(), 1U);
}

TEST_F(ngap_validator_test, when_unique_and_duplicate_pdu_session_id_then_pdu_session_setup_partly_fails)
{
  cu_cp_ue_index_t ue_index  = uint_to_ue_index(0);
  amf_ue_id_t      amf_ue_id = uint_to_amf_ue_id(0);
  ran_ue_id_t      ran_ue_id = uint_to_ran_ue_id(0);

  ngap_message ngap_msg =
      generate_pdu_session_resource_setup_request_with_one_unique_and_two_duplicate_pdu_sessions(amf_ue_id, ran_ue_id);

  auto& asn1_request = ngap_msg.pdu.init_msg().value.pdu_session_res_setup_request();

  ngap_pdu_session_resource_setup_request request;
  fill_ngap_pdu_session_resource_setup_request(request, asn1_request->pdu_session_res_setup_list_su_req);

  ngap_ue_logger ue_logger{"NGAP", {ue_index, ran_ue_id}};
  // Verify PDU session resource setup request.
  auto verification_outcome = verify_pdu_session_resource_setup_request(request, asn1_request, ue_logger);

  ASSERT_EQ(verification_outcome.request.pdu_session_res_setup_items.size(), 1U);
  ASSERT_EQ(verification_outcome.response.pdu_session_res_failed_to_setup_items.size(), 1U);
}

// Test handling of PDU session resource setup request with non-GBR QoS flows but no PDU session aggregate maximum bit
// rate.
TEST_F(
    ngap_validator_test,
    when_request_pdu_session_contains_non_gbr_qos_flows_but_no_aggregate_maximum_bitrate_then_pdu_session_setup_fails)
{
  pdu_session_id_t psi       = uint_to_pdu_session_id(1);
  cu_cp_ue_index_t ue_index  = uint_to_ue_index(0);
  amf_ue_id_t      amf_ue_id = uint_to_amf_ue_id(0);
  ran_ue_id_t      ran_ue_id = uint_to_ran_ue_id(0);

  ngap_message ngap_msg =
      generate_pdu_session_resource_setup_request_with_non_gbr_qos_flows_and_no_aggregate_maximum_bitrate(
          amf_ue_id, ran_ue_id, psi);

  auto& asn1_request = ngap_msg.pdu.init_msg().value.pdu_session_res_setup_request();

  ngap_pdu_session_resource_setup_request request;
  fill_ngap_pdu_session_resource_setup_request(request, asn1_request->pdu_session_res_setup_list_su_req);

  ngap_ue_logger ue_logger{"NGAP", {ue_index, ran_ue_id}};
  // Verify PDU session resource setup request.
  auto verification_outcome = verify_pdu_session_resource_setup_request(request, asn1_request, ue_logger);

  ASSERT_TRUE(verification_outcome.request.pdu_session_res_setup_items.empty());
  ASSERT_EQ(verification_outcome.response.pdu_session_res_failed_to_setup_items.size(), 1U);
}

// Test that a Non-GBR QoS flow is detected from its 5QI, and not from the optional IEs that a Non-GBR QoS flow may
// carry.
TEST_F(ngap_validator_test,
       when_plain_non_gbr_qos_flow_has_no_pdu_session_aggregate_maximum_bitrate_then_pdu_session_setup_fails)
{
  pdu_session_id_t psi       = uint_to_pdu_session_id(1);
  cu_cp_ue_index_t ue_index  = uint_to_ue_index(0);
  amf_ue_id_t      amf_ue_id = uint_to_amf_ue_id(0);
  ran_ue_id_t      ran_ue_id = uint_to_ran_ue_id(0);

  // 5QI 9 is a Non-GBR 5QI. The QoS flow carries neither the Reflective QoS Attribute nor the Additional QoS Flow
  // Information IE.
  ngap_message ngap_msg = generate_valid_pdu_session_resource_setup_request_message(
      amf_ue_id, ran_ue_id, {{psi, {pdu_session_type_t::ipv4, {{uint_to_qos_flow_id(1), 9}}}}});
  ASSERT_TRUE(remove_pdu_session_aggregate_maximum_bit_rate(ngap_msg));

  auto& asn1_request = ngap_msg.pdu.init_msg().value.pdu_session_res_setup_request();

  ngap_pdu_session_resource_setup_request request;
  fill_ngap_pdu_session_resource_setup_request(request, asn1_request->pdu_session_res_setup_list_su_req);

  ngap_ue_logger ue_logger{"NGAP", {ue_index, ran_ue_id}};
  // Verify PDU session resource setup request.
  auto verification_outcome = verify_pdu_session_resource_setup_request(request, asn1_request, ue_logger);

  ASSERT_TRUE(verification_outcome.request.pdu_session_res_setup_items.empty());
  ASSERT_EQ(verification_outcome.response.pdu_session_res_failed_to_setup_items.size(), 1U);
}

// Test handling of an unsupported PDU session type.
TEST_F(ngap_validator_test, when_pdu_session_type_is_ipv4v6_then_pdu_session_setup_fails)
{
  pdu_session_id_t psi       = uint_to_pdu_session_id(1);
  cu_cp_ue_index_t ue_index  = uint_to_ue_index(0);
  amf_ue_id_t      amf_ue_id = uint_to_amf_ue_id(0);
  ran_ue_id_t      ran_ue_id = uint_to_ran_ue_id(0);

  ngap_message ngap_msg = generate_valid_pdu_session_resource_setup_request_message(
      amf_ue_id, ran_ue_id, {{psi, {pdu_session_type_t::ipv4v6, {{uint_to_qos_flow_id(1), 9}}}}});

  auto& asn1_request = ngap_msg.pdu.init_msg().value.pdu_session_res_setup_request();

  ngap_pdu_session_resource_setup_request request;
  fill_ngap_pdu_session_resource_setup_request(request, asn1_request->pdu_session_res_setup_list_su_req);

  ngap_ue_logger ue_logger{"NGAP", {ue_index, ran_ue_id}};
  // Verify PDU session resource setup request.
  auto verification_outcome = verify_pdu_session_resource_setup_request(request, asn1_request, ue_logger);

  ASSERT_TRUE(verification_outcome.request.pdu_session_res_setup_items.empty());
  ASSERT_EQ(verification_outcome.response.pdu_session_res_failed_to_setup_items.size(), 1U);
}

// Test that the UE Aggregate Maximum Bit Rate doesn't substitute the PDU Session Aggregate Maximum Bit Rate.
TEST_F(
    ngap_validator_test,
    when_request_pdu_session_contains_non_gbr_qos_flows_but_only_ue_aggregate_maximum_bitrate_then_pdu_session_setup_fails)
{
  pdu_session_id_t psi       = uint_to_pdu_session_id(1);
  cu_cp_ue_index_t ue_index  = uint_to_ue_index(0);
  amf_ue_id_t      amf_ue_id = uint_to_amf_ue_id(0);
  ran_ue_id_t      ran_ue_id = uint_to_ran_ue_id(0);

  ngap_message ngap_msg =
      generate_pdu_session_resource_setup_request_with_non_gbr_qos_flows_and_no_aggregate_maximum_bitrate(
          amf_ue_id, ran_ue_id, psi);

  auto& asn1_request = ngap_msg.pdu.init_msg().value.pdu_session_res_setup_request();

  // The UE Aggregate Maximum Bit Rate is present, the PDU Session Aggregate Maximum Bit Rate isn't.
  asn1_request->ue_aggr_max_bit_rate_present                 = true;
  asn1_request->ue_aggr_max_bit_rate.ue_aggr_max_bit_rate_dl = 1000000000U;
  asn1_request->ue_aggr_max_bit_rate.ue_aggr_max_bit_rate_ul = 1000000000U;

  ngap_pdu_session_resource_setup_request request;
  fill_ngap_pdu_session_resource_setup_request(request, asn1_request->pdu_session_res_setup_list_su_req);

  ngap_ue_logger ue_logger{"NGAP", {ue_index, ran_ue_id}};
  // Verify PDU session resource setup request.
  auto verification_outcome = verify_pdu_session_resource_setup_request(request, asn1_request, ue_logger);

  ASSERT_TRUE(verification_outcome.request.pdu_session_res_setup_items.empty());
  ASSERT_EQ(verification_outcome.response.pdu_session_res_failed_to_setup_items.size(), 1U);
}

// Test handling of a GBR QoS flow without the GBR QoS Flow Information IE.
TEST_F(ngap_validator_test, when_gbr_qos_flow_has_no_gbr_qos_information_then_pdu_session_setup_fails)
{
  pdu_session_id_t psi       = uint_to_pdu_session_id(1);
  cu_cp_ue_index_t ue_index  = uint_to_ue_index(0);
  amf_ue_id_t      amf_ue_id = uint_to_amf_ue_id(0);
  ran_ue_id_t      ran_ue_id = uint_to_ran_ue_id(0);

  // 5QI 1 is a GBR 5QI, so the GBR QoS Flow Information IE is required.
  ngap_message ngap_msg = generate_valid_pdu_session_resource_setup_request_message(
      amf_ue_id, ran_ue_id, {{psi, {pdu_session_type_t::ipv4, {{uint_to_qos_flow_id(1), 1}}}}});
  ASSERT_TRUE(remove_gbr_qos_information(ngap_msg, uint_to_qos_flow_id(1)));

  auto& asn1_request = ngap_msg.pdu.init_msg().value.pdu_session_res_setup_request();

  ngap_pdu_session_resource_setup_request request;
  fill_ngap_pdu_session_resource_setup_request(request, asn1_request->pdu_session_res_setup_list_su_req);

  ngap_ue_logger ue_logger{"NGAP", {ue_index, ran_ue_id}};
  // Verify PDU session resource setup request.
  auto verification_outcome = verify_pdu_session_resource_setup_request(request, asn1_request, ue_logger);

  // The only QoS flow of the PDU session fails, so the whole PDU session fails.
  ASSERT_TRUE(verification_outcome.request.pdu_session_res_setup_items.empty());
  ASSERT_EQ(verification_outcome.response.pdu_session_res_failed_to_setup_items.size(), 1U);
}

// Test handling of a PDU session with both a valid QoS flow and a GBR QoS flow without the GBR QoS Flow Information IE.
TEST_F(ngap_validator_test, when_one_of_two_qos_flows_has_incomplete_qos_parameters_then_only_that_qos_flow_fails)
{
  pdu_session_id_t psi         = uint_to_pdu_session_id(1);
  qos_flow_id_t    valid_qfi   = uint_to_qos_flow_id(1);
  qos_flow_id_t    invalid_qfi = uint_to_qos_flow_id(2);
  cu_cp_ue_index_t ue_index    = uint_to_ue_index(0);
  amf_ue_id_t      amf_ue_id   = uint_to_amf_ue_id(0);
  ran_ue_id_t      ran_ue_id   = uint_to_ran_ue_id(0);

  // 5QI 9 is a Non-GBR 5QI, 5QI 1 is a GBR 5QI that requires the GBR QoS Flow Information IE.
  ngap_message ngap_msg = generate_valid_pdu_session_resource_setup_request_message(
      amf_ue_id, ran_ue_id, {{psi, {pdu_session_type_t::ipv4, {{valid_qfi, 9}, {invalid_qfi, 1}}}}});
  ASSERT_TRUE(remove_gbr_qos_information(ngap_msg, invalid_qfi));

  auto& asn1_request = ngap_msg.pdu.init_msg().value.pdu_session_res_setup_request();

  ngap_pdu_session_resource_setup_request request;
  fill_ngap_pdu_session_resource_setup_request(request, asn1_request->pdu_session_res_setup_list_su_req);

  ngap_ue_logger ue_logger{"NGAP", {ue_index, ran_ue_id}};
  // Verify PDU session resource setup request.
  auto verification_outcome = verify_pdu_session_resource_setup_request(request, asn1_request, ue_logger);

  // The PDU session is set up without the QoS flow that failed the verification.
  ASSERT_EQ(verification_outcome.response.pdu_session_res_failed_to_setup_items.size(), 0U);
  ASSERT_EQ(verification_outcome.request.pdu_session_res_setup_items.size(), 1U);
  const auto& verified_qos_flows =
      verification_outcome.request.pdu_session_res_setup_items[psi].qos_flow_setup_request_items;
  ASSERT_EQ(verified_qos_flows.size(), 1U);
  ASSERT_TRUE(verified_qos_flows.contains(valid_qfi));

  // The QoS flow that failed the verification is reported in the response.
  ASSERT_EQ(verification_outcome.response.pdu_session_res_setup_response_items.size(), 1U);
  const auto& failed_qos_flows = verification_outcome.response.pdu_session_res_setup_response_items[psi]
                                     .pdu_session_resource_setup_response_transfer.qos_flow_failed_to_setup_list;
  ASSERT_EQ(failed_qos_flows.size(), 1U);
  ASSERT_EQ(failed_qos_flows[invalid_qfi].qos_flow_id, invalid_qfi);
}

// Test handling of a delay critical QoS flow without the Maximum Data Burst Volume IE.
TEST_F(ngap_validator_test, when_delay_critical_qos_flow_has_no_max_data_burst_volume_then_pdu_session_setup_fails)
{
  pdu_session_id_t psi       = uint_to_pdu_session_id(1);
  cu_cp_ue_index_t ue_index  = uint_to_ue_index(0);
  amf_ue_id_t      amf_ue_id = uint_to_amf_ue_id(0);
  ran_ue_id_t      ran_ue_id = uint_to_ran_ue_id(0);

  ngap_message ngap_msg =
      generate_pdu_session_resource_setup_request_with_delay_critical_qos_flow(amf_ue_id, ran_ue_id, psi, false);

  auto& asn1_request = ngap_msg.pdu.init_msg().value.pdu_session_res_setup_request();

  ngap_pdu_session_resource_setup_request request;
  fill_ngap_pdu_session_resource_setup_request(request, asn1_request->pdu_session_res_setup_list_su_req);

  ngap_ue_logger ue_logger{"NGAP", {ue_index, ran_ue_id}};
  // Verify PDU session resource setup request.
  auto verification_outcome = verify_pdu_session_resource_setup_request(request, asn1_request, ue_logger);

  // The only QoS flow of the PDU session fails, so the whole PDU session fails.
  ASSERT_TRUE(verification_outcome.request.pdu_session_res_setup_items.empty());
  ASSERT_EQ(verification_outcome.response.pdu_session_res_failed_to_setup_items.size(), 1U);
}

// Test handling of a delay critical QoS flow with the Maximum Data Burst Volume IE.
TEST_F(ngap_validator_test, when_delay_critical_qos_flow_has_max_data_burst_volume_then_pdu_session_setup_succeeds)
{
  pdu_session_id_t psi       = uint_to_pdu_session_id(1);
  cu_cp_ue_index_t ue_index  = uint_to_ue_index(0);
  amf_ue_id_t      amf_ue_id = uint_to_amf_ue_id(0);
  ran_ue_id_t      ran_ue_id = uint_to_ran_ue_id(0);

  ngap_message ngap_msg =
      generate_pdu_session_resource_setup_request_with_delay_critical_qos_flow(amf_ue_id, ran_ue_id, psi, true);

  auto& asn1_request = ngap_msg.pdu.init_msg().value.pdu_session_res_setup_request();

  ngap_pdu_session_resource_setup_request request;
  fill_ngap_pdu_session_resource_setup_request(request, asn1_request->pdu_session_res_setup_list_su_req);

  ngap_ue_logger ue_logger{"NGAP", {ue_index, ran_ue_id}};
  // Verify PDU session resource setup request.
  auto verification_outcome = verify_pdu_session_resource_setup_request(request, asn1_request, ue_logger);

  ASSERT_EQ(verification_outcome.request.pdu_session_res_setup_items.size(), 1U);
  ASSERT_EQ(verification_outcome.response.pdu_session_res_failed_to_setup_items.size(), 0U);
  ASSERT_EQ(verification_outcome.response.pdu_session_res_setup_response_items.size(), 0U);
}

// Test handling of valid PDU session resource modification request.
TEST_F(ngap_validator_test, when_valid_request_received_then_pdu_session_modify_succeeds)
{
  pdu_session_id_t psi       = uint_to_pdu_session_id(1);
  cu_cp_ue_index_t ue_index  = uint_to_ue_index(0);
  amf_ue_id_t      amf_ue_id = uint_to_amf_ue_id(0);
  ran_ue_id_t      ran_ue_id = uint_to_ran_ue_id(0);

  ngap_message ngap_msg = generate_valid_pdu_session_resource_modify_request_message(amf_ue_id, ran_ue_id, psi);

  auto& asn1_request = ngap_msg.pdu.init_msg().value.pdu_session_res_modify_request();

  ngap_pdu_session_resource_modify_request request;
  fill_ngap_pdu_session_resource_modify_request(request, asn1_request->pdu_session_res_modify_list_mod_req);

  ngap_ue_logger ue_logger{"NGAP", {ue_index, ran_ue_id}};
  // Verify PDU session resource modify request.
  auto verification_outcome = verify_pdu_session_resource_modify_request(request, asn1_request, ue_logger);

  ASSERT_EQ(verification_outcome.request.pdu_session_res_modify_items.size(), 1U);
  ASSERT_EQ(verification_outcome.response.pdu_session_res_modify_list.size(), 0U);
  ASSERT_EQ(verification_outcome.response.pdu_session_res_failed_to_modify_list.size(), 0U);
}

// Test handling of duplicate PDU session ids in PDU session resource modification request.
TEST_F(ngap_validator_test, when_duplicate_pdu_session_id_then_pdu_session_modification_fails)
{
  cu_cp_ue_index_t ue_index  = uint_to_ue_index(0);
  amf_ue_id_t      amf_ue_id = uint_to_amf_ue_id(0);
  ran_ue_id_t      ran_ue_id = uint_to_ran_ue_id(0);

  ngap_message ngap_msg =
      generate_invalid_pdu_session_resource_modify_request_message(amf_ue_id, ran_ue_id, uint_to_pdu_session_id(1));

  auto& asn1_request = ngap_msg.pdu.init_msg().value.pdu_session_res_modify_request();

  ngap_pdu_session_resource_modify_request request;
  fill_ngap_pdu_session_resource_modify_request(request, asn1_request->pdu_session_res_modify_list_mod_req);

  ngap_ue_logger ue_logger{"NGAP", {ue_index, ran_ue_id}};
  // Verify PDU session resource modify request.
  auto verification_outcome = verify_pdu_session_resource_modify_request(request, asn1_request, ue_logger);

  ASSERT_TRUE(verification_outcome.request.pdu_session_res_modify_items.empty());
  ASSERT_EQ(verification_outcome.response.pdu_session_res_failed_to_modify_list.size(), 1U);
}

TEST_F(ngap_validator_test, when_unique_and_duplicate_pdu_session_id_then_pdu_session_modify_partly_fails)
{
  pdu_session_id_t psi           = uint_to_pdu_session_id(1);
  pdu_session_id_t duplicate_psi = uint_to_pdu_session_id(2);
  cu_cp_ue_index_t ue_index      = uint_to_ue_index(0);
  amf_ue_id_t      amf_ue_id     = uint_to_amf_ue_id(0);
  ran_ue_id_t      ran_ue_id     = uint_to_ran_ue_id(0);

  ngap_message ngap_msg = generate_pdu_session_resource_modify_request_with_one_unique_and_two_duplicate_pdu_sessions(
      amf_ue_id, ran_ue_id, psi, duplicate_psi);

  auto& asn1_request = ngap_msg.pdu.init_msg().value.pdu_session_res_modify_request();

  ngap_pdu_session_resource_modify_request request;
  fill_ngap_pdu_session_resource_modify_request(request, asn1_request->pdu_session_res_modify_list_mod_req);

  ngap_ue_logger ue_logger{"NGAP", {ue_index, ran_ue_id}};
  // Verify PDU session resource modify request.
  auto verification_outcome = verify_pdu_session_resource_modify_request(request, asn1_request, ue_logger);

  ASSERT_EQ(verification_outcome.request.pdu_session_res_modify_items.size(), 1U);
  ASSERT_EQ(verification_outcome.response.pdu_session_res_failed_to_modify_list.size(), 1U);
}
