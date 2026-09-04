// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "lib/xnap/procedures/retrieve_ue_context_asn1_helpers.h"
#include "ocudu/asn1/xnap/common.h"
#include "ocudu/xnap/xnap_message.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace ocudu::ocucp;

// Packs and unpacks the PDU, so that the conversion round trip also covers the ASN.1 encoding. Anything the encoder
// drops, for instance an IE left unmarked, show up as a mismatch after the round trip.
template <typename T>
static void pack_and_unpack(T& pdu)
{
  byte_buffer   packed;
  asn1::bit_ref packer{packed};
  ASSERT_EQ(pdu.pack(packer), asn1::OCUDUASN_SUCCESS) << "Failed to pack the XNAP PDU";

  pdu = {};
  asn1::cbit_ref unpacker{packed};
  ASSERT_EQ(pdu.unpack(unpacker), asn1::OCUDUASN_SUCCESS) << "Failed to unpack the XNAP PDU";
}

/// Builds an ASN.1 Retrieve UE Context Request carrying an RRC Reestablishment UE Context ID, as a peer sends it.
static asn1::xnap::retrieve_ue_context_request_s make_asn1_reest_request()
{
  asn1::xnap::retrieve_ue_context_request_s asn1_request;
  asn1_request->new_ng_ra_nnode_ue_xn_ap_id = 5;

  auto& asn1_reest_id = asn1_request->ue_context_id.set_rrrc_reest();
  asn1_reest_id.c_rnti.from_number(to_underlying(to_rnti(0x4601)));
  asn1_reest_id.fail_cell_pci.set_nr() = 42;

  asn1_request->mac_i.from_number(0xabcd);
  asn1_request->new_ng_ran_cell_id.set_nr().from_number(nr_cell_identity::create(0x19b0).value().value());

  return asn1_request;
}

TEST(retrieve_ue_context_asn1_helpers_test, reestablishment_request_is_converted_from_asn1)
{
  xnap_message msg;
  msg.pdu.set_init_msg();
  msg.pdu.init_msg().load_info_obj(ASN1_XNAP_ID_RETRIEVE_UE_CONTEXT);
  msg.pdu.init_msg().value.retrieve_ue_context_request() = make_asn1_reest_request();
  ASSERT_NO_FATAL_FAILURE(pack_and_unpack(msg.pdu));

  xnap_retrieve_ue_context_request converted;
  ASSERT_TRUE(asn1_to_retrieve_ue_context_request(converted, msg.pdu.init_msg().value.retrieve_ue_context_request()));

  ASSERT_TRUE(std::holds_alternative<xnap_ue_context_id_for_rrc_reest>(converted.ue_context_id));
  const auto& reest_id = std::get<xnap_ue_context_id_for_rrc_reest>(converted.ue_context_id);
  ASSERT_EQ(reest_id.c_rnti, to_rnti(0x4601));
  ASSERT_EQ(reest_id.fail_cell_pci, 42);
  ASSERT_EQ(converted.mac_i, (security::sec_short_mac_i{0xab, 0xcd}));
  ASSERT_EQ(converted.target_nci, nr_cell_identity::create(0x19b0).value());
  ASSERT_FALSE(converted.rrc_resume_cause.has_value());
}

TEST(retrieve_ue_context_asn1_helpers_test, resume_request_is_converted_from_asn1)
{
  asn1::xnap::retrieve_ue_context_request_s asn1_request = make_asn1_reest_request();

  // Replace the identity with an RRC Resume one, which carries an I-RNTI and the cell the UE accessed here.
  auto& asn1_resume_id = asn1_request->ue_context_id.set_rrc_resume();
  asn1_resume_id.i_rnti.set_i_rnti_short().from_number(0x1234);
  asn1_resume_id.allocated_c_rnti.from_number(to_underlying(to_rnti(0x4602)));
  asn1_resume_id.access_pci.set_nr()     = 7;
  asn1_request->rrc_resume_cause_present = true;
  asn1_request->rrc_resume_cause.value   = asn1::xnap::rrc_resume_cause_opts::rna_upd;

  xnap_message msg;
  msg.pdu.set_init_msg();
  msg.pdu.init_msg().load_info_obj(ASN1_XNAP_ID_RETRIEVE_UE_CONTEXT);
  msg.pdu.init_msg().value.retrieve_ue_context_request() = asn1_request;
  ASSERT_NO_FATAL_FAILURE(pack_and_unpack(msg.pdu));

  xnap_retrieve_ue_context_request converted;
  ASSERT_TRUE(asn1_to_retrieve_ue_context_request(converted, msg.pdu.init_msg().value.retrieve_ue_context_request()));

  ASSERT_TRUE(std::holds_alternative<xnap_ue_context_id_for_rrc_resume>(converted.ue_context_id));
  const auto& resume_id = std::get<xnap_ue_context_id_for_rrc_resume>(converted.ue_context_id);
  ASSERT_TRUE(std::holds_alternative<short_i_rnti_t>(resume_id.i_rnti));
  ASSERT_EQ(std::get<short_i_rnti_t>(resume_id.i_rnti).value(), 0x1234);
  ASSERT_EQ(resume_id.allocated_c_rnti, to_rnti(0x4602));
  ASSERT_EQ(resume_id.access_pci, 7);
  ASSERT_EQ(converted.rrc_resume_cause, resume_cause_t::rna_upd);
}

TEST(retrieve_ue_context_asn1_helpers_test, request_for_an_e_utra_cell_is_rejected)
{
  asn1::xnap::retrieve_ue_context_request_s asn1_request              = make_asn1_reest_request();
  asn1_request->ue_context_id.rrrc_reest().fail_cell_pci.set_e_utra() = 42;

  xnap_retrieve_ue_context_request converted;
  ASSERT_FALSE(asn1_to_retrieve_ue_context_request(converted, asn1_request))
      << "A UE Context ID identifying an E-UTRA cell must be rejected";
}

//----------------------------------------------------------------------------------//
// Target side: converting a request to, and a response from, ASN.1                 //
//----------------------------------------------------------------------------------//

static xnap_retrieve_ue_context_request make_reest_request()
{
  xnap_retrieve_ue_context_request request;
  request.ue_context_id = xnap_ue_context_id_for_rrc_reest{.c_rnti = to_rnti(0x4601), .fail_cell_pci = 42};
  request.mac_i         = {0xab, 0xcd};
  request.target_nci    = nr_cell_identity::create(0x19b0).value();
  return request;
}

TEST(retrieve_ue_context_asn1_helpers_test, reestablishment_request_survives_a_conversion_round_trip)
{
  const xnap_retrieve_ue_context_request request = make_reest_request();

  xnap_message msg;
  msg.pdu.set_init_msg();
  msg.pdu.init_msg().load_info_obj(ASN1_XNAP_ID_RETRIEVE_UE_CONTEXT);
  auto& asn1_request                        = msg.pdu.init_msg().value.retrieve_ue_context_request();
  asn1_request->new_ng_ra_nnode_ue_xn_ap_id = 5;
  retrieve_ue_context_request_to_asn1(asn1_request, request);
  ASSERT_NO_FATAL_FAILURE(pack_and_unpack(msg.pdu));

  xnap_retrieve_ue_context_request converted;
  ASSERT_TRUE(asn1_to_retrieve_ue_context_request(converted, msg.pdu.init_msg().value.retrieve_ue_context_request()));

  ASSERT_TRUE(std::holds_alternative<xnap_ue_context_id_for_rrc_reest>(converted.ue_context_id));
  const auto& reest_id = std::get<xnap_ue_context_id_for_rrc_reest>(converted.ue_context_id);
  ASSERT_EQ(reest_id.c_rnti, to_rnti(0x4601));
  ASSERT_EQ(reest_id.fail_cell_pci, 42);
  ASSERT_EQ(converted.mac_i, request.mac_i);
  ASSERT_EQ(converted.target_nci, request.target_nci);
  ASSERT_FALSE(converted.rrc_resume_cause.has_value());
}

TEST(retrieve_ue_context_asn1_helpers_test, resume_request_survives_a_conversion_round_trip)
{
  xnap_retrieve_ue_context_request request;
  request.ue_context_id = xnap_ue_context_id_for_rrc_resume{
      .i_rnti = short_i_rnti_t::from_uint(0x1234).value(), .allocated_c_rnti = to_rnti(0x4602), .access_pci = 7};
  request.mac_i            = {0x12, 0x34};
  request.target_nci       = nr_cell_identity::create(0x19b1).value();
  request.rrc_resume_cause = resume_cause_t::rna_upd;

  xnap_message msg;
  msg.pdu.set_init_msg();
  msg.pdu.init_msg().load_info_obj(ASN1_XNAP_ID_RETRIEVE_UE_CONTEXT);
  auto& asn1_request                        = msg.pdu.init_msg().value.retrieve_ue_context_request();
  asn1_request->new_ng_ra_nnode_ue_xn_ap_id = 5;
  retrieve_ue_context_request_to_asn1(asn1_request, request);
  ASSERT_NO_FATAL_FAILURE(pack_and_unpack(msg.pdu));

  xnap_retrieve_ue_context_request converted;
  ASSERT_TRUE(asn1_to_retrieve_ue_context_request(converted, msg.pdu.init_msg().value.retrieve_ue_context_request()));

  ASSERT_TRUE(std::holds_alternative<xnap_ue_context_id_for_rrc_resume>(converted.ue_context_id));
  const auto& resume_id = std::get<xnap_ue_context_id_for_rrc_resume>(converted.ue_context_id);
  ASSERT_TRUE(std::holds_alternative<short_i_rnti_t>(resume_id.i_rnti));
  ASSERT_EQ(std::get<short_i_rnti_t>(resume_id.i_rnti).value(), 0x1234);
  ASSERT_EQ(resume_id.allocated_c_rnti, to_rnti(0x4602));
  ASSERT_EQ(resume_id.access_pci, 7);
  ASSERT_EQ(converted.mac_i, request.mac_i);
  ASSERT_EQ(converted.target_nci, request.target_nci);
  ASSERT_EQ(converted.rrc_resume_cause, resume_cause_t::rna_upd);
}

TEST(retrieve_ue_context_asn1_helpers_test, response_survives_a_conversion_round_trip)
{
  xnap_retrieve_ue_context_response response;
  response.success = true;
  response.guami = guami_t{.plmn = plmn_identity::test_value(), .amf_set_id = 1, .amf_pointer = 1, .amf_region_id = 1};

  auto& ue_context_info     = response.ue_context_info;
  ue_context_info.amf_ue_id = 11;
  ue_context_info.amf_addr  = transport_layer_address::create_from_string("127.0.0.1");
  ue_context_info.security_context.k.fill(0x5a);
  ue_context_info.security_context.ncc                 = 3;
  ue_context_info.security_context.supported_enc_algos = security::supported_algorithms{true, true, false};
  ue_context_info.security_context.supported_int_algos = security::supported_algorithms{true, false, true};
  ue_context_info.ue_ambr.dl                           = 1000000000;
  ue_context_info.ue_ambr.ul                           = 500000000;
  ue_context_info.rrc_context                          = make_byte_buffer("deadbeef").value();

  cu_cp_pdu_session_res_setup_item pdu_session_item;
  pdu_session_item.pdu_session_id = uint_to_pdu_session_id(1);
  pdu_session_item.s_nssai = s_nssai_t{.sst = slice_service_type{1}, .sd = slice_differentiator::create(1).value()};
  pdu_session_item.ul_ngu_up_tnl_info =
      up_transport_layer_info{transport_layer_address::create_from_string("127.0.0.1"), gtpu_teid_t{12345}};
  pdu_session_item.pdu_session_type = pdu_session_type_t::ipv4;
  qos_flow_setup_request_item qos_flow_item;
  qos_flow_item.qos_flow_id                        = uint_to_qos_flow_id(1);
  qos_flow_item.qos_flow_level_qos_params.qos_desc = non_dyn_5qi_descriptor{uint_to_five_qi(9), {}, {}, {}};
  qos_flow_item.qos_flow_level_qos_params.alloc_retention_prio.prio_level_arp = 8;
  pdu_session_item.qos_flow_setup_request_items.emplace(uint_to_qos_flow_id(1), qos_flow_item);
  ue_context_info.pdu_session_res_to_be_setup_list.emplace(uint_to_pdu_session_id(1), pdu_session_item);

  xnap_message msg;
  msg.pdu.set_successful_outcome();
  msg.pdu.successful_outcome().load_info_obj(ASN1_XNAP_ID_RETRIEVE_UE_CONTEXT);
  auto& asn1_response                        = msg.pdu.successful_outcome().value.retrieve_ue_context_resp();
  asn1_response->new_ng_ra_nnode_ue_xn_ap_id = 5;
  asn1_response->old_ng_ra_nnode_ue_xn_ap_id = 6;
  retrieve_ue_context_response_to_asn1(asn1_response, response);
  ASSERT_NO_FATAL_FAILURE(pack_and_unpack(msg.pdu));

  xnap_retrieve_ue_context_response converted;
  ASSERT_TRUE(
      asn1_to_retrieve_ue_context_response(converted, msg.pdu.successful_outcome().value.retrieve_ue_context_resp()));

  ASSERT_TRUE(converted.success);
  ASSERT_EQ(converted.guami.plmn, response.guami.plmn);
  ASSERT_EQ(converted.ue_context_info.amf_ue_id, 11);
  ASSERT_EQ(converted.ue_context_info.amf_addr, ue_context_info.amf_addr);
  ASSERT_EQ(converted.ue_context_info.security_context.k, ue_context_info.security_context.k);
  ASSERT_EQ(converted.ue_context_info.security_context.ncc, 3);
  ASSERT_EQ(converted.ue_context_info.security_context.supported_enc_algos,
            ue_context_info.security_context.supported_enc_algos);
  ASSERT_EQ(converted.ue_context_info.security_context.supported_int_algos,
            ue_context_info.security_context.supported_int_algos);
  ASSERT_EQ(converted.ue_context_info.ue_ambr.dl, 1000000000);
  ASSERT_EQ(converted.ue_context_info.ue_ambr.ul, 500000000);
  ASSERT_EQ(converted.ue_context_info.rrc_context, ue_context_info.rrc_context);
  ASSERT_EQ(converted.ue_context_info.pdu_session_res_to_be_setup_list.size(), 1);
  const auto& converted_pdu_session =
      converted.ue_context_info.pdu_session_res_to_be_setup_list[uint_to_pdu_session_id(1)];
  ASSERT_EQ(converted_pdu_session.ul_ngu_up_tnl_info, pdu_session_item.ul_ngu_up_tnl_info);
  ASSERT_EQ(converted_pdu_session.qos_flow_setup_request_items.size(), 1);
}
