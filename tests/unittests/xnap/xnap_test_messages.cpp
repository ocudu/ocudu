// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "xnap_test_messages.h"
#include "lib/xnap/procedures/xn_setup_procedure_asn1_helpers.h"
#include "lib/xnap/xnap_asn1_converters.h"
#include "ocudu/asn1/asn1_utils.h"
#include "ocudu/asn1/rrc_nr/rrc_nr.h"
#include "ocudu/asn1/xnap/common.h"
#include "ocudu/asn1/xnap/xnap_ies.h"
#include "ocudu/asn1/xnap/xnap_pdu_contents.h"
#include "ocudu/ran/plmn_identity.h"
#include "ocudu/xnap/xnap_message.h"
#include "ocudu/xnap/xnap_types.h"

using namespace ocudu;
using namespace ocucp;
using namespace asn1::xnap;

// Adds AS-Config to a packed RRC HandoverPreparationInformation, reporting the source's DRB1 <-> QFI1 mapping for
// PDU session 1 in the embedded RRCReconfiguration (TS 38.331 Section 11.2.3). The rest of the container, in
// particular the UE capabilities the target needs, is preserved.
static byte_buffer add_as_config_drb_mapping(const byte_buffer& packed_ho_prep)
{
  asn1::rrc_nr::ho_prep_info_s ho_prep_info;
  asn1::cbit_ref               bref({packed_ho_prep.begin(), packed_ho_prep.end()});
  report_fatal_error_if_not(ho_prep_info.unpack(bref) == asn1::OCUDUASN_SUCCESS,
                            "Failed to unpack HandoverPreparationInformation");

  // Build the source's RRCReconfiguration carrying the full radio bearer configuration.
  asn1::rrc_nr::rrc_recfg_s source_recfg;
  auto&                     recfg_ies     = source_recfg.crit_exts.set_rrc_recfg();
  recfg_ies.radio_bearer_cfg_present      = true;
  asn1::rrc_nr::drb_to_add_mod_s asn1_drb = {};
  asn1_drb.drb_id                         = 1;
  asn1_drb.cn_assoc_present               = true;
  auto& asn1_sdap_cfg                     = asn1_drb.cn_assoc.set_sdap_cfg();
  asn1_sdap_cfg.pdu_session               = 1;
  asn1_sdap_cfg.sdap_hdr_dl               = asn1::rrc_nr::sdap_cfg_s::sdap_hdr_dl_opts::absent;
  asn1_sdap_cfg.sdap_hdr_ul               = asn1::rrc_nr::sdap_cfg_s::sdap_hdr_ul_opts::absent;
  asn1_sdap_cfg.default_drb               = true;
  asn1_sdap_cfg.mapped_qos_flows_to_add.push_back(1);
  recfg_ies.radio_bearer_cfg.drb_to_add_mod_list.push_back(asn1_drb);

  byte_buffer   packed_recfg;
  asn1::bit_ref recfg_packer{packed_recfg};
  report_fatal_error_if_not(source_recfg.pack(recfg_packer) == asn1::OCUDUASN_SUCCESS,
                            "Failed to pack AS-Config RRCReconfiguration");

  auto& ies                = ho_prep_info.crit_exts.c1().ho_prep_info();
  ies.source_cfg_present   = true;
  ies.source_cfg.rrc_recfg = std::move(packed_recfg);

  byte_buffer   repacked;
  asn1::bit_ref packer{repacked};
  report_fatal_error_if_not(ho_prep_info.pack(packer) == asn1::OCUDUASN_SUCCESS,
                            "Failed to pack HandoverPreparationInformation");
  return repacked;
}

cu_cp_served_cell_info ocudu::ocucp::generate_served_cell_info(pci_t pci, const nr_cell_global_id_t& cgi, tac_t tac)
{
  cu_cp_served_cell_info served_cell;
  served_cell.nr_cgi       = cgi;
  served_cell.nr_pci       = pci;
  served_cell.five_gs_tac  = tac;
  served_cell.served_plmns = {cgi.plmn_id};

  cu_cp_tdd_info tdd_info;
  tdd_info.nr_freq_info.nr_arfcn = 632628;
  tdd_info.nr_freq_info.freq_band_list_nr.push_back(cu_cp_freq_band_nr_item{.freq_band_ind_nr = 78});
  tdd_info.tx_bw.nr_scs    = subcarrier_spacing::kHz30;
  tdd_info.tx_bw.nr_nrb    = 51;
  served_cell.nr_mode_info = tdd_info;

  return served_cell;
}

xnap_message ocudu::ocucp::generate_xn_setup_response_with_served_cell(const xnap_configuration&  peer_cfg,
                                                                       pci_t                      served_pci,
                                                                       const nr_cell_global_id_t& served_cgi)
{
  const cu_cp_served_cell_info served_cell =
      generate_served_cell_info(served_pci, served_cgi, peer_cfg.tai_support_list.front().tac);

  return generate_asn1_xn_setup_response(peer_cfg, {&served_cell, 1});
}

xnap_message ocudu::ocucp::generate_handover_request(local_xnap_ue_id_t local_xnap_ue_id,
                                                     bool               include_drb_to_qos_flow_mapping,
                                                     bool               include_as_config_drb_mapping)
{
  xnap_message xnap_msg;

  xnap_msg.pdu.set_init_msg();
  xnap_msg.pdu.init_msg().load_info_obj(ASN1_XNAP_ID_HO_PREP);

  auto& ho_request = xnap_msg.pdu.init_msg().value.ho_request();

  ho_request->source_ng_ra_nnode_ue_xn_ap_id = to_underlying(local_xnap_ue_id);
  ho_request->cause.set_radio_network() =
      asn1::xnap::cause_radio_network_layer_opts::options::ho_desirable_for_radio_reasons;
  ho_request->target_cell_global_id.set_nr() =
      cgi_to_asn1(nr_cell_global_id_t{plmn_identity::test_value(), nr_cell_identity::create({411, 22}, 0).value()});
  ho_request->guami = guami_to_asn1(
      guami_t{.plmn = plmn_identity::test_value(), .amf_set_id = 1, .amf_pointer = 1, .amf_region_id = 1});
  ho_request->ue_context_info_ho_request.ng_c_ue_ref = 1;

  ho_request->ue_context_info_ho_request.cp_tnl_info_source.set_endpoint_ip_address();
  tla_to_asn1_bitstring(ho_request->ue_context_info_ho_request.cp_tnl_info_source.endpoint_ip_address(),
                        transport_layer_address::create_from_string("127.0.0.1"));
  ho_request->ue_context_info_ho_request.ue_security_cap.nr_encyption_algorithms.from_number(49152);
  ho_request->ue_context_info_ho_request.ue_security_cap.nr_integrity_protection_algorithms.from_number(49152);
  ho_request->ue_context_info_ho_request.security_info.key_ng_ran_star.from_string(
      "1111111000001101100111110001011010001110110010111010001100110111100111011000110110011010000110000011000010111000"
      "0010001100001010000001111111100000100111101011000011110000110101110010001010001010101000101100101100100000001110"
      "00010001000110001101110101100110");
  ho_request->ue_context_info_ho_request.ue_ambr.dl_ue_ambr = 1000000000;
  ho_request->ue_context_info_ho_request.ue_ambr.ul_ue_ambr = 1000000000;

  pdu_session_res_to_be_setup_item_s pdu_session_item;
  pdu_session_item.pdu_session_id = 1;
  pdu_session_item.s_nssai =
      s_nssai_to_asn1(s_nssai_t{.sst = slice_service_type{1}, .sd = slice_differentiator::create(1).value()});
  up_transport_layer_info_to_asn1(
      pdu_session_item.ul_ng_u_tnl_at_up_f,
      up_transport_layer_info{transport_layer_address::create_from_string("127.0.0.1"), gtpu_teid_t{12345}});
  pdu_session_item.pdu_session_type = pdu_session_type_e::ipv4;

  qos_flows_to_be_setup_item_s qos_flow_item;
  qos_flow_item.qfi = 1;
  qos_flow_item.qos_flow_level_qos_params.qos_characteristics.set_non_dyn();
  qos_flow_item.qos_flow_level_qos_params.qos_characteristics.non_dyn().five_qi = 9;
  qos_flow_item.qos_flow_level_qos_params.alloc_and_retention_prio.pre_emption_cap =
      asn1::xnap::allocand_retention_prio_s::pre_emption_cap_opts::options::shall_not_trigger_preemption;
  qos_flow_item.qos_flow_level_qos_params.alloc_and_retention_prio.pre_emption_vulnerability =
      asn1::xnap::allocand_retention_prio_s::pre_emption_vulnerability_opts::options::not_preemptable;

  pdu_session_item.qos_flows_to_be_setup_list.push_back(qos_flow_item);

  if (include_drb_to_qos_flow_mapping) {
    // Report this source's own DRB-to-QoS-flow mapping (DRB1 <-> QFI1, matching the admitted PDU session).
    pdu_session_item.dataforwardinginfofrom_source_present = true;
    drb_to_qos_flow_map_item_s drb_to_qos_flow_map_item;
    drb_to_qos_flow_map_item.drb_id = 1;
    qos_flow_item_s assoc_qos_flow;
    assoc_qos_flow.qfi = 1;
    drb_to_qos_flow_map_item.qos_flows_list.push_back(assoc_qos_flow);
    pdu_session_item.dataforwardinginfofrom_source.source_drb_to_qos_flow_map.push_back(drb_to_qos_flow_map_item);
  }

  ho_request->ue_context_info_ho_request.pdu_session_res_to_be_setup_list.push_back(pdu_session_item);

  ho_request->ue_context_info_ho_request.rrc_context =
      make_byte_buffer(
          "00217b8680ce811d1960097e360e1317000183f1300098a09a00000020400f13400389a00000000e268208010010134a0f0040000000"
          "00000040000000247001040000259650100400002596500052388008404008010100200400200801052050")
          .value();

  if (include_as_config_drb_mapping) {
    ho_request->ue_context_info_ho_request.rrc_context =
        add_as_config_drb_mapping(ho_request->ue_context_info_ho_request.rrc_context);
  }

  asn1::xnap::last_visited_cell_item_c last_visited_cell;
  last_visited_cell.set_ng_ran_cell() = make_byte_buffer("0000f11000066c0000800000").value();
  ho_request->ue_history_info.push_back(last_visited_cell);

  return xnap_msg;
}

xnap_message ocudu::ocucp::generate_handover_preparation_failure(peer_xnap_ue_id_t peer_xnap_ue_id)
{
  xnap_message xnap_msg;

  xnap_msg.pdu.set_unsuccessful_outcome();
  xnap_msg.pdu.unsuccessful_outcome().load_info_obj(ASN1_XNAP_ID_HO_PREP);

  auto& ho_prep_fail = xnap_msg.pdu.unsuccessful_outcome().value.ho_prep_fail();

  ho_prep_fail->source_ng_ra_nnode_ue_xn_ap_id = to_underlying(peer_xnap_ue_id);

  // Fill cause.
  ho_prep_fail->cause.set_radio_network() = asn1::xnap::cause_radio_network_layer_opts::options::unspecified;

  return xnap_msg;
}

xnap_message ocudu::ocucp::generate_handover_request_ack(local_xnap_ue_id_t local_xnap_ue_id,
                                                         peer_xnap_ue_id_t  peer_xnap_ue_id)
{
  xnap_message xnap_msg;

  xnap_msg.pdu.set_successful_outcome();
  xnap_msg.pdu.successful_outcome().load_info_obj(ASN1_XNAP_ID_HO_PREP);

  auto& ho_request_ack = xnap_msg.pdu.successful_outcome().value.ho_request_ack();

  ho_request_ack->source_ng_ra_nnode_ue_xn_ap_id = to_underlying(peer_xnap_ue_id);
  ho_request_ack->target_ng_ra_nnode_ue_xn_ap_id = to_underlying(local_xnap_ue_id);

  // Fill target to source ng ran node transparent container.
  // Create RRC container.
  byte_buffer rrc_container =
      make_byte_buffer(
          "081a115568220201204550001e1004bcc012121600020509a0000193f7c7000000243434840be2e0260030258380f80408d078100009"
          "39dc601349798002692f120200046402051320c6b6c6bb003704020000080800041a235246c013497890000023271adb19127c058332"
          "55ff8092748837146e30dc71b9637dfab6387580221603400c162300e0102908024985950001ff000000000306e10840003c02ca0041"
          "8000001034c080a28500071c48000133557c841c001040c2050c1c9c48a163068e1e408800004280004005a8000864428000c645a800"
          "10024280014025a8001862428001c625a800200842800240c8200a0320902c0c8280c0320b0340c8300e0320d03c0c83810162080440"
          "e829024b92a4a1814388e8acf1379340e9041e2efc0c10e0000001c7feb311aa6ab940b000010cbb00000000000000000008422b5514"
          "011c00401020800388402710038082042000710804e10070204104000e21009c200e0608108001c420138601c10104100038840270c0"
          "020000002086020406080706800071c40000002004000806000809002200a60000231002271c00600040")
          .value();
  ho_request_ack->target2_source_ng_ra_nnode_transp_container = std::move(rrc_container);

  return xnap_msg;
}

xnap_message ocudu::ocucp::generate_sn_status_transfer(local_xnap_ue_id_t           local_xnap_ue_id,
                                                       peer_xnap_ue_id_t            peer_xnap_ue_id,
                                                       const std::vector<drb_id_t>& extra_drb_ids)
{
  xnap_message xnap_msg;

  xnap_msg.pdu.set_init_msg();
  xnap_msg.pdu.init_msg().load_info_obj(ASN1_XNAP_ID_S_N_STATUS_TRANSFER);

  auto& sn_status_transfer = xnap_msg.pdu.init_msg().value.sn_status_transfer();

  sn_status_transfer->source_ng_ra_nnode_ue_xn_ap_id = to_underlying(local_xnap_ue_id);
  sn_status_transfer->target_ng_ra_nnode_ue_xn_ap_id = to_underlying(peer_xnap_ue_id);

  drbs_subject_to_status_transfer_item_s drb_item;
  drb_item.drb_id = 1;
  drb_item.pdcp_status_transfer_ul.set_pdcp_sn_12bits();
  drb_item.pdcp_status_transfer_dl.set_pdcp_sn_12bits();

  sn_status_transfer->drbs_subject_to_status_transfer_list.push_back(drb_item);

  for (drb_id_t extra_drb_id : extra_drb_ids) {
    drbs_subject_to_status_transfer_item_s extra_drb_item;
    extra_drb_item.drb_id = static_cast<uint8_t>(extra_drb_id);
    extra_drb_item.pdcp_status_transfer_ul.set_pdcp_sn_12bits();
    extra_drb_item.pdcp_status_transfer_dl.set_pdcp_sn_12bits();

    sn_status_transfer->drbs_subject_to_status_transfer_list.push_back(extra_drb_item);
  }

  return xnap_msg;
}

xnap_message ocudu::ocucp::generate_ue_context_release(local_xnap_ue_id_t local_xnap_ue_id,
                                                       peer_xnap_ue_id_t  peer_xnap_ue_id)
{
  xnap_message xnap_msg;

  xnap_msg.pdu.set_init_msg();
  xnap_msg.pdu.init_msg().load_info_obj(ASN1_XNAP_ID_U_E_CONTEXT_RELEASE);

  auto& ue_context_release = xnap_msg.pdu.init_msg().value.ue_context_release();

  ue_context_release->source_ng_ra_nnode_ue_xn_ap_id = to_underlying(peer_xnap_ue_id);
  ue_context_release->target_ng_ra_nnode_ue_xn_ap_id = to_underlying(local_xnap_ue_id);

  return xnap_msg;
}

xnap_message ocudu::ocucp::generate_retrieve_ue_context_request(peer_xnap_ue_id_t peer_xnap_ue_id,
                                                                pci_t             fail_cell_pci,
                                                                nr_cell_identity  target_nci)
{
  xnap_message xnap_msg;
  xnap_msg.pdu.set_init_msg();
  xnap_msg.pdu.init_msg().load_info_obj(ASN1_XNAP_ID_RETRIEVE_UE_CONTEXT);

  auto& request = xnap_msg.pdu.init_msg().value.retrieve_ue_context_request();

  // This is sent from the target to the source, so the new NG-RAN node UE XnAP ID is the peer XNAP UE ID.
  request->new_ng_ra_nnode_ue_xn_ap_id = to_underlying(peer_xnap_ue_id);

  auto& reest_id = request->ue_context_id.set_rrrc_reest();
  reest_id.c_rnti.from_number(to_underlying(rnti_t::MIN_CRNTI));
  reest_id.fail_cell_pci.set_nr() = fail_cell_pci;

  request->mac_i.from_number(0xabcd);
  request->new_ng_ran_cell_id.set_nr().from_number(target_nci.value());

  return xnap_msg;
}

xnap_message ocudu::ocucp::generate_retrieve_ue_context_request_for_resume(peer_xnap_ue_id_t peer_xnap_ue_id,
                                                                           short_i_rnti_t    i_rnti,
                                                                           nr_cell_identity  target_nci,
                                                                           uint16_t          resume_mac_i)
{
  xnap_message xnap_msg;
  xnap_msg.pdu.set_init_msg();
  xnap_msg.pdu.init_msg().load_info_obj(ASN1_XNAP_ID_RETRIEVE_UE_CONTEXT);

  auto& request = xnap_msg.pdu.init_msg().value.retrieve_ue_context_request();

  // This is sent from the target to the source, so the new NG-RAN node UE XnAP ID is the peer XNAP UE ID.
  request->new_ng_ra_nnode_ue_xn_ap_id = to_underlying(peer_xnap_ue_id);

  auto& resume_id = request->ue_context_id.set_rrc_resume();
  resume_id.i_rnti.set_i_rnti_short().from_number(i_rnti.value());
  resume_id.allocated_c_rnti.from_number(to_underlying(rnti_t::MIN_CRNTI));
  resume_id.access_pci.set_nr() = 0;

  request->mac_i.from_number(resume_mac_i);
  request->new_ng_ran_cell_id.set_nr().from_number(target_nci.value());

  return xnap_msg;
}

xnap_message ocudu::ocucp::generate_retrieve_ue_context_response(local_xnap_ue_id_t local_xnap_ue_id,
                                                                 peer_xnap_ue_id_t  peer_xnap_ue_id)
{
  xnap_message xnap_msg;
  xnap_msg.pdu.set_successful_outcome();
  xnap_msg.pdu.successful_outcome().load_info_obj(ASN1_XNAP_ID_RETRIEVE_UE_CONTEXT);

  auto& response = xnap_msg.pdu.successful_outcome().value.retrieve_ue_context_resp();

  // This is sent from the source to the target, so the new NG-RAN node UE XnAP ID is the local XNAP UE ID and the old
  // NG-RAN node UE XnAP ID is the peer XNAP UE ID.
  response->new_ng_ra_nnode_ue_xn_ap_id = to_underlying(local_xnap_ue_id);
  response->old_ng_ra_nnode_ue_xn_ap_id = to_underlying(peer_xnap_ue_id);

  response->guami = guami_to_asn1(
      guami_t{.plmn = plmn_identity::test_value(), .amf_set_id = 1, .amf_pointer = 1, .amf_region_id = 1});

  auto& ue_context_info           = response->ue_context_info_retr_ue_ctxt_resp;
  ue_context_info.ng_c_ue_sig_ref = 1;
  ue_context_info.sig_tnl_at_source.set_endpoint_ip_address();
  tla_to_asn1_bitstring(ue_context_info.sig_tnl_at_source.endpoint_ip_address(),
                        transport_layer_address::create_from_string("127.0.0.1"));
  ue_context_info.ue_security_cap.nr_encyption_algorithms.from_number(49152);
  ue_context_info.ue_security_cap.nr_integrity_protection_algorithms.from_number(49152);
  ue_context_info.security_info.key_ng_ran_star.from_string(
      "1111111000001101100111110001011010001110110010111010001100110111100111011000110110011010000110000011000010111000"
      "0010001100001010000001111111100000100111101011000011110000110101110010001010001010101000101100101100100000001110"
      "00010001000110001101110101100110");
  ue_context_info.ue_ambr.dl_ue_ambr = 1000000000;
  ue_context_info.ue_ambr.ul_ue_ambr = 1000000000;

  pdu_session_res_to_be_setup_item_s pdu_session_item;
  pdu_session_item.pdu_session_id = 1;
  pdu_session_item.s_nssai =
      s_nssai_to_asn1(s_nssai_t{.sst = slice_service_type{1}, .sd = slice_differentiator::create(1).value()});
  up_transport_layer_info_to_asn1(
      pdu_session_item.ul_ng_u_tnl_at_up_f,
      up_transport_layer_info{transport_layer_address::create_from_string("127.0.0.1"), gtpu_teid_t{12345}});
  pdu_session_item.pdu_session_type = pdu_session_type_e::ipv4;

  qos_flows_to_be_setup_item_s qos_flow_item;
  qos_flow_item.qfi = 1;
  qos_flow_item.qos_flow_level_qos_params.qos_characteristics.set_non_dyn();
  qos_flow_item.qos_flow_level_qos_params.qos_characteristics.non_dyn().five_qi = 9;
  qos_flow_item.qos_flow_level_qos_params.alloc_and_retention_prio.pre_emption_cap =
      asn1::xnap::allocand_retention_prio_s::pre_emption_cap_opts::options::shall_not_trigger_preemption;
  qos_flow_item.qos_flow_level_qos_params.alloc_and_retention_prio.pre_emption_vulnerability =
      asn1::xnap::allocand_retention_prio_s::pre_emption_vulnerability_opts::options::not_preemptable;
  qos_flow_item.qos_flow_level_qos_params.alloc_and_retention_prio.prio_level = 1;
  pdu_session_item.qos_flows_to_be_setup_list.push_back(qos_flow_item);

  ue_context_info.pdu_session_res_to_be_setup_list.push_back(pdu_session_item);

  // A real HandoverPreparationInformation, so that the target can recover the UE capabilities and the source's
  // AS-Config from it, as it does for a handover.
  ue_context_info.rrc_context =
      make_byte_buffer(
          "00217b8680ce811d1960097e360e1317000183f1300098a09a00000020400f13400389a00000000e268208010010134a0f0040000000"
          "00000040000000247001040000259650100400002596500052388008404008010100200400200801052050")
          .value();

  return xnap_msg;
}

xnap_message ocudu::ocucp::generate_retrieve_ue_context_failure(local_xnap_ue_id_t local_xnap_ue_id)
{
  xnap_message xnap_msg;
  xnap_msg.pdu.set_unsuccessful_outcome();
  xnap_msg.pdu.unsuccessful_outcome().load_info_obj(ASN1_XNAP_ID_RETRIEVE_UE_CONTEXT);

  auto& failure = xnap_msg.pdu.unsuccessful_outcome().value.retrieve_ue_context_fail();

  // This is sent from the source to the target, so the new NG-RAN node UE XnAP ID is the local XNAP UE ID.
  failure->new_ng_ra_nnode_ue_xn_ap_id = to_underlying(local_xnap_ue_id);
  failure->cause.set_radio_network()   = cause_radio_network_layer_opts::ue_context_id_not_known;

  return xnap_msg;
}

xnap_message ocudu::ocucp::generate_handover_cancel(local_xnap_ue_id_t         local_xnap_ue_id,
                                                    peer_xnap_ue_id_t          peer_xnap_ue_id,
                                                    const nr_cell_global_id_t& cell)
{
  xnap_message xnap_msg;

  xnap_msg.pdu.set_init_msg();
  xnap_msg.pdu.init_msg().load_info_obj(ASN1_XNAP_ID_HO_CANCEL);

  auto& ho_cancel = xnap_msg.pdu.init_msg().value.ho_cancel();

  ho_cancel->source_ng_ra_nnode_ue_xn_ap_id         = to_underlying(local_xnap_ue_id);
  ho_cancel->target_ng_ra_nnode_ue_xn_ap_id_present = true;
  ho_cancel->target_ng_ra_nnode_ue_xn_ap_id         = to_underlying(peer_xnap_ue_id);
  ho_cancel->cause.set_radio_network()              = cause_radio_network_layer_opts::proc_cancelled;

  ho_cancel->target_cells_to_cancel_present = true;
  asn1::xnap::target_cell_list_item_s cell_item;
  cell_item.target_cell.set_nr() = cgi_to_asn1(cell);
  ho_cancel->target_cells_to_cancel.push_back(cell_item);

  return xnap_msg;
}
