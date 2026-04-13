// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../xnap_asn1_converters.h"
#include "ocudu/asn1/asn1_utils.h"
#include "ocudu/asn1/xnap/xnap_pdu_contents.h"
#include "ocudu/support/io/transport_layer_address.h"
#include "ocudu/xnap/xnap_handover.h"
#include <variant>

namespace ocudu::ocucp {

inline bool asn1_to_ue_context_info_ho_request(xnap_ue_context_info_ho_request&                request,
                                               const asn1::xnap::ue_context_info_ho_request_s& asn1_request)
{
  // > Fill NG-C UE associated signalling reference (AMF UE NGAP ID).
  request.amf_ue_id = asn1_request.ng_c_ue_ref;
  // > Fill CP TNL info source (AMF SCTP IP address).
  request.amf_addr = tla_from_asn1_bitstring(asn1_request.cp_tnl_info_source.endpoint_ip_address());
  // > Fill security context.
  asn1_to_security_context(request.security_context, asn1_request.ue_security_cap, asn1_request.security_info);
  // > Fill UE aggregated max bit rate.
  request.ue_ambr.dl = asn1_request.ue_ambr.dl_ue_ambr;
  request.ue_ambr.ul = asn1_request.ue_ambr.ul_ue_ambr;

  // > Fill PDU session resource to be setup list.
  for (const auto& asn1_pdu_session : asn1_request.pdu_session_res_to_be_setup_list) {
    pdu_session_id_t                 pdu_session_id = uint_to_pdu_session_id(asn1_pdu_session.pdu_session_id);
    cu_cp_pdu_session_res_setup_item pdu_session_item;
    // Fill PDU session ID.
    pdu_session_item.pdu_session_id = pdu_session_id;
    // Fill S-NSSAI.
    pdu_session_item.s_nssai = asn1_to_s_nssai(asn1_pdu_session.s_nssai);
    // Fill PDU session aggregate maximum bit rate dl.
    if (asn1_pdu_session.pdu_session_ambr_present) {
      pdu_session_item.pdu_session_aggregate_maximum_bit_rate_dl = asn1_pdu_session.pdu_session_ambr.dl_session_ambr;
      pdu_session_item.pdu_session_aggregate_maximum_bit_rate_ul = asn1_pdu_session.pdu_session_ambr.ul_session_ambr;
    }
    // Fill UL NGU UP TNL info.
    if (asn1_pdu_session.ul_ng_u_tnl_at_up_f.type() != asn1::xnap::up_transport_layer_info_c::types_opts::gtp_tunnel) {
      return false;
    }
    pdu_session_item.ul_ngu_up_tnl_info = asn1_to_up_transport_layer_info(asn1_pdu_session.ul_ng_u_tnl_at_up_f);
    // Fill PDU session type.
    pdu_session_item.pdu_session_type = asn1_to_pdu_session_type(asn1_pdu_session.pdu_session_type);
    // Fill Security indication.
    if (asn1_pdu_session.security_ind_present) {
      pdu_session_item.security_ind.emplace();
      asn1_to_security_indication(pdu_session_item.security_ind.value(), asn1_pdu_session.security_ind);
    }
    // Fill QoS flow setup request list.
    for (const auto& asn1_qos_flow : asn1_pdu_session.qos_flows_to_be_setup_list) {
      qos_flow_id_t               qos_flow_id = uint_to_qos_flow_id(asn1_qos_flow.qfi);
      qos_flow_setup_request_item qos_flow_item;
      // Fill QoS flow ID.
      qos_flow_item.qos_flow_id = qos_flow_id;
      // Fill QoS flow level QoS parameters.
      qos_flow_item.qos_flow_level_qos_params =
          asn1_to_qos_flow_level_qos_parameters(asn1_qos_flow.qos_flow_level_qos_params);
      // Fill ERAB ID.
      if (asn1_qos_flow.erab_id_present) {
        qos_flow_item.erab_id = asn1_qos_flow.erab_id;
      }
      pdu_session_item.qos_flow_setup_request_items.emplace(qos_flow_id, qos_flow_item);
    }

    request.pdu_session_res_to_be_setup_list.emplace(pdu_session_id, pdu_session_item);
  }

  // Fill RRC container (RRC handover preparation information).
  request.rrc_handover_preparation_information = asn1_request.rrc_context.copy();

  // Fill location reporting information.
  if (asn1_request.location_report_info_present) {
    request.location_report_info = asn1_to_location_report_info(asn1_request.location_report_info);
  }

  return true;
}

/// \brief Convert Handover Request ASN1 struct to common type.
/// \param[out] request The xnap_handover_request struct to fill.
/// \param[in] asn1_request The Handover Request ASN1 struct.
/// \returns True if the conversion was successful, false otherwise.
inline bool asn1_to_handover_request(xnap_handover_request& request, const asn1::xnap::ho_request_s& asn1_request)
{
  // Fill cause.
  request.cause = asn1_to_cause(asn1_request->cause);

  // Fill NR-CGI.
  if (asn1_request->target_cell_global_id.type() != asn1::xnap::target_cgi_c::types_opts::options::nr) {
    return false;
  }
  request.nr_cgi = asn1_to_cgi(asn1_request->target_cell_global_id.nr());

  // Fill GUAMI.
  request.guami = asn1_to_guami(asn1_request->guami);

  // Fill UE context info HO request.
  if (!asn1_to_ue_context_info_ho_request(request.ue_context_info_ho_request,
                                          asn1_request->ue_context_info_ho_request)) {
    return false;
  }

  // Check whether this is a Conditional Handover (CHO) preparation.
  request.is_conditional_handover = asn1_request->ch_oinfo_req_present;
  if (request.is_conditional_handover && asn1_request->ch_oinfo_req.ie_exts.cho_time_based_info_present) {
    request.cho_timeout =
        cho_window_duration_step * asn1_request->ch_oinfo_req.ie_exts.cho_time_based_info.cho_ho_win_dur;
  }

  // TODO: fill missing optional parameters.

  return true;
}

/// \brief Convert \c cu_cp_handover_request_ack common type struct to ASN.1.
/// \param[out] asn1_ho_request_ack The Handover Request Ack ASN1 struct to fill.
/// \param[in] ho_response The cu_cp_handover_request_ack common type struct.
/// \returns True if the conversion was successful, false otherwise.
inline bool handover_request_ack_to_asn1(asn1::xnap::ho_request_ack_s&     asn1_ho_request_ack,
                                         const cu_cp_handover_request_ack& ho_response)
{
  // Fill PDU session resource admitted list.
  for (const auto& admitted_item : ho_response.pdu_session_res_admitted_list) {
    if (!std::holds_alternative<cu_cp_xn_pdu_session_res_admitted_item>(admitted_item)) {
      return false;
    }
    asn1::xnap::pdu_session_res_admitted_item_s asn1_admitted_item;
    pdu_session_res_admitted_item_to_asn1(asn1_admitted_item,
                                          std::get<cu_cp_xn_pdu_session_res_admitted_item>(admitted_item));
    asn1_ho_request_ack->pdu_session_res_admitted_list.push_back(asn1_admitted_item);
  }

  // Fill PDU session resource not admitted list.
  if (!ho_response.pdu_session_failed_to_setup_list.empty()) {
    asn1_ho_request_ack->pdu_session_res_not_admitted_list_present = true;
    for (const auto& not_admitted_item : ho_response.pdu_session_failed_to_setup_list) {
      asn1::xnap::pdu_session_res_not_admitted_item_s asn1_not_admitted_item;
      // Fill PDU session ID.
      asn1_not_admitted_item.pdu_session_id = pdu_session_id_to_uint(not_admitted_item.pdu_session_id);
      // Fill cause.
      if (!std::holds_alternative<xnap_cause_t>(not_admitted_item.cause)) {
        return false;
      }
      asn1_not_admitted_item.cause_present = true;
      asn1_not_admitted_item.cause         = cause_to_asn1(std::get<xnap_cause_t>(not_admitted_item.cause));

      asn1_ho_request_ack->pdu_session_res_not_admitted_list.push_back(asn1_not_admitted_item);
    }
  }

  // Fill target to source NG RAN node transparent container (containing RRC handover command container).
  asn1_ho_request_ack->target2_source_ng_ra_nnode_transp_container = ho_response.rrc_handover_command.copy();

  // TODO: add optional parameters.

  return true;
}

/// \brief Convert \c cu_cp_handover_request_failure common type struct to ASN.1.
/// \param[out] asn1_ho_failure The Handover Request Failure ASN1 struct to fill.
/// \param[in] ho_failure The cu_cp_handover_request_failure common type struct.
/// \returns True if the conversion was successful, false otherwise.
inline bool handover_preparation_failure_to_asn1(asn1::xnap::ho_prep_fail_s&           asn1_ho_failure,
                                                 const cu_cp_handover_request_failure& ho_failure)
{
  // Fill cause.
  if (!std::holds_alternative<xnap_cause_t>(ho_failure.cause)) {
    return false;
  }
  asn1_ho_failure->cause = cause_to_asn1(std::get<xnap_cause_t>(ho_failure.cause));

  return true;
}

} // namespace ocudu::ocucp
