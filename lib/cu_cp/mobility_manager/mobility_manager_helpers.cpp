// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "mobility_manager_helpers.h"
#include "ocudu/ran/cu_cp_types.h"
#include "ocudu/ran/plmn_identity.h"

using namespace ocudu;
using namespace ocucp;

ngap_handover_preparation_request ocudu::ocucp::generate_ngap_handover_preparation_request(
    cu_cp_ue_index_t                                          source_ue_index,
    gnb_id_t                                                  target_gnb_id,
    plmn_identity                                             target_plmn,
    tac_t                                                     target_tac,
    nr_cell_identity                                          target_nci,
    const std::map<pdu_session_id_t, up_pdu_session_context>& pdu_sessions)
{
  ngap_handover_preparation_request request;
  request.ue_index         = source_ue_index;
  request.target_id.gnb_id = target_gnb_id;
  request.target_id.plmn   = target_plmn;
  request.target_id.tac    = target_tac;
  request.nci              = target_nci;

  // Create a map of all PDU sessions and their associated QoS flows.
  for (const auto& pdu_session : pdu_sessions) {
    std::vector<qos_flow_id_t> qos_flows;
    for (const auto& drb : pdu_session.second.drbs) {
      for (const auto& qos_flow : drb.second.qos_flows) {
        qos_flows.push_back(qos_flow.first);
      }
    }
    request.pdu_sessions.insert({pdu_session.first, qos_flows});
  }
  return request;
}

xnap_handover_request
ocudu::ocucp::generate_xnap_handover_request(cu_cp_ue_index_t                                          source_ue_index,
                                             nr_cell_global_id_t                                       target_nr_cgi,
                                             guami_t                                                   guami,
                                             amf_ue_id_t                                               source_amf_ue_id,
                                             aggregate_maximum_bit_rate_t                              ue_ambr,
                                             const security::security_context&                         security_context,
                                             const std::map<pdu_session_id_t, up_pdu_session_context>& pdu_sessions,
                                             const byte_buffer& rrc_handover_preparation_information,
                                             const std::optional<location_report_request>& location_report_cfg)
{
  xnap_handover_request request;
  request.ue_index                             = source_ue_index;
  request.cause                                = xnap_cause_radio_network_t::ho_desirable_for_radio_reasons;
  request.nr_cgi                               = target_nr_cgi;
  request.guami                                = guami;
  request.ue_context_info_ho_request.amf_ue_id = amf_ue_id_to_uint(source_amf_ue_id);
  // TODO: Fill correct AMF address.
  request.ue_context_info_ho_request.amf_addr         = transport_layer_address::create_from_string("127.0.0.1");
  request.ue_context_info_ho_request.security_context = security_context;
  request.ue_context_info_ho_request.ue_ambr          = ue_ambr;
  for (const auto& [pid, pdu_session_ctxt] : pdu_sessions) {
    cu_cp_pdu_session_res_setup_item pdu_session_item;
    pdu_session_item.pdu_session_id = pid;

    // TODO: move pdu session specific members to pdu session context.
    // For now the PDU session specific information is extracted from the first DRB of the PDU session context.
    const auto& session_ctxt = pdu_session_ctxt.drbs.begin()->second;
    pdu_session_item.s_nssai = session_ctxt.s_nssai;

    // Fill UL NGU UP TNL info.
    pdu_session_item.ul_ngu_up_tnl_info = pdu_session_ctxt.ul_ngu_up_tnl_info;

    // Fill PDU session type.
    pdu_session_item.pdu_session_type = pdu_session_ctxt.type;

    // Iterate over all DRBs of the PDU session and collect all QoS flows.
    for (const auto& [drb_id, drb_ctxt] : pdu_session_ctxt.drbs) {
      for (const auto& [qfi, qos_flow] : drb_ctxt.qos_flows) {
        qos_flow_setup_request_item qos_flow_setup_item = {};
        // Set QFI.
        qos_flow_setup_item.qos_flow_id = qfi;
        // Fill QoS flow level QoS parameters.
        qos_flow_setup_item.qos_flow_level_qos_params = qos_flow.qos_params;
        pdu_session_item.qos_flow_setup_request_items.emplace(qfi, qos_flow_setup_item);
      }
    }

    request.ue_context_info_ho_request.pdu_session_res_to_be_setup_list.emplace(pid, pdu_session_item);
  }
  request.ue_context_info_ho_request.rrc_handover_preparation_information = rrc_handover_preparation_information.copy();
  request.ue_context_info_ho_request.location_report_info                 = location_report_cfg;

  return request;
}
