// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/slotted_vector.h"
#include "ocudu/ngap/ngap_types.h"
#include "ocudu/pdcp/pdcp_config.h"
#include "ocudu/ran/cause/ngap_cause.h"
#include "ocudu/ran/cause/xnap_cause.h"
#include "ocudu/ran/cu_cp_types.h"
#include "ocudu/ran/cu_types.h"
#include "ocudu/ran/pdcp/pdcp_sn_size.h"
#include "ocudu/ran/rb_id.h"
#include "ocudu/ran/up_transport_layer_info.h"
#include "ocudu/security/security.h"

namespace ocudu::ocucp {

struct cu_cp_data_forwarding_resp_drb_item {
  drb_id_t                               drb_id = drb_id_t::drb1;
  std::optional<up_transport_layer_info> dl_forwarding_up_tnl;
  std::optional<up_transport_layer_info> ul_forwarding_up_tnl;
};

struct cu_cp_qos_flow_with_data_forwarding_item {
  qos_flow_id_t qos_flow_id = qos_flow_id_t::invalid;
  // NG handover specific field.
  std::optional<bool> data_forwarding_accepted;
};

struct cu_cp_qos_flow_with_cause_item {
  qos_flow_id_t                                           qos_flow_id = qos_flow_id_t::invalid;
  std::variant<ngap_cause_t, std::optional<xnap_cause_t>> cause;
};

struct cu_cp_data_forwarding_info_from_target_ng_ran_node {
  // XNAP handover specific fields.
  std::vector<qos_flow_id_t>             qos_flows_accepted_for_data_forwarding_list;
  std::optional<up_transport_layer_info> pdu_session_level_dl_data_forwarding_info;
  std::optional<up_transport_layer_info> pdu_session_level_ul_data_forwarding_info;

  // Common fields.
  std::vector<cu_cp_data_forwarding_resp_drb_item> data_forwarding_resp_drb_item_list;
};

struct cu_cp_ng_pdu_session_res_admitted_item {
  pdu_session_id_t                                      pdu_session_id = pdu_session_id_t::invalid;
  up_transport_layer_info                               dl_ngu_up_tnl_info;
  std::optional<up_transport_layer_info>                dl_forwarding_up_tnl_info;
  std::optional<security_result_t>                      security_result;
  std::vector<cu_cp_qos_flow_with_data_forwarding_item> qos_flows_setup_list;
  std::vector<cu_cp_qos_flow_with_cause_item>           qos_flows_failed_to_setup_list;
  cu_cp_data_forwarding_info_from_target_ng_ran_node    data_forwarding_info_from_target;
};

struct cu_cp_xn_pdu_session_res_admitted_item {
  pdu_session_id_t                                                  pdu_session_id = pdu_session_id_t::invalid;
  up_transport_layer_info                                           dl_ngu_up_tnl_info;
  std::vector<cu_cp_qos_flow_with_data_forwarding_item>             qos_flows_setup_list;
  std::vector<cu_cp_qos_flow_with_cause_item>                       qos_flows_failed_to_setup_list;
  std::optional<cu_cp_data_forwarding_info_from_target_ng_ran_node> data_forwarding_info_from_target;
};

using cu_cp_pdu_session_res_admitted_item =
    std::variant<cu_cp_ng_pdu_session_res_admitted_item, cu_cp_xn_pdu_session_res_admitted_item>;

struct cu_cp_pdu_session_with_cause_item {
  pdu_session_id_t                         pdu_session_id = pdu_session_id_t::invalid;
  std::variant<ngap_cause_t, xnap_cause_t> cause;
};

struct cu_cp_handover_request_ack {
  cu_cp_ue_index_t                                 ue_index = cu_cp_ue_index_t::invalid;
  std::vector<cu_cp_pdu_session_res_admitted_item> pdu_session_res_admitted_list;
  std::vector<cu_cp_pdu_session_with_cause_item>   pdu_session_failed_to_setup_list;
  byte_buffer                                      rrc_handover_command;
};

struct cu_cp_handover_request_failure {
  cu_cp_ue_index_t                         ue_index = cu_cp_ue_index_t::invalid;
  std::variant<ngap_cause_t, xnap_cause_t> cause;
};

using cu_cp_handover_resource_allocation_response =
    std::variant<cu_cp_handover_request_ack, cu_cp_handover_request_failure>;

struct cu_cp_drb_status_ul_t {
  pdcp_sn_size    sn_size;
  pdcp_count_info ul_count;
};

struct cu_cp_drb_status_dl_t {
  pdcp_sn_size    sn_size;
  pdcp_count_info dl_count;
};

struct cu_cp_drbs_subject_to_status_transfer_item {
  drb_id_t              drb_id;
  cu_cp_drb_status_dl_t drb_status_dl;
  cu_cp_drb_status_ul_t drb_status_ul;
};

struct cu_cp_status_transfer {
  cu_cp_ue_index_t                                                        ue_index = cu_cp_ue_index_t::invalid;
  slotted_id_vector<drb_id_t, cu_cp_drbs_subject_to_status_transfer_item> drbs_subject_to_status_transfer_list;
};

struct cu_cp_user_plane_security_info {
  security_result_t     security_result;
  security_indication_t security_ind;
};

struct cu_cp_pdu_session_res_to_be_switched_dl_item {
  pdu_session_id_t                              pdu_session_id = pdu_session_id_t::invalid;
  up_transport_layer_info                       dl_ngu_up_tnl_info;
  bool                                          dl_ngu_tnl_info_reused = false;
  std::optional<cu_cp_user_plane_security_info> user_plane_security_info;
  std::vector<qos_flow_id_t>                    qos_flow_accepted_list;
};

struct cu_cp_path_switch_request {
  cu_cp_ue_index_t                                          ue_index = cu_cp_ue_index_t::invalid;
  unsigned                                                  source_amf_ue_ngap_id;
  cu_cp_user_location_info_nr                               user_location_info;
  security::supported_algorithms                            supported_enc_algos;
  security::supported_algorithms                            supported_int_algos;
  std::vector<cu_cp_pdu_session_res_to_be_switched_dl_item> pdu_session_res_to_be_switched_dl_list;
  std::vector<cu_cp_pdu_session_with_cause_item>            pdu_session_res_failed_to_setup_list_ps_req;
  // Note: per 3GPP TS 38.413, section 9.2.3.8 this is a establishment cause, although it is named "RRC resume cause".
  std::optional<establishment_cause_t> rrc_resume_cause;
};

struct cu_cp_pdu_session_res_switched_item {
  pdu_session_id_t                       pdu_session_id = pdu_session_id_t::invalid;
  std::optional<up_transport_layer_info> ul_ngu_up_tnl_info;
  std::optional<security_indication_t>   security_ind;
};

struct cu_cp_pdu_session_res_released_item {
  pdu_session_id_t pdu_session_id = pdu_session_id_t::invalid;
  ngap_cause_t     cause;
};

struct cu_cp_path_switch_request_ack {
  cu_cp_ue_index_t                                           ue_index = cu_cp_ue_index_t::invalid;
  security::security_context                                 security_context;
  std::vector<cu_cp_pdu_session_res_switched_item>           pdu_session_res_switched_list;
  std::vector<cu_cp_pdu_session_res_released_item>           pdu_session_res_released_list;
  std::vector<s_nssai_t>                                     allowed_nssai;
  std::optional<ngap_core_network_assist_info_for_inactive>  core_network_assist_info_for_inactive;
  std::optional<ngap_rrc_inactive_transition_report_request> rrc_inactive_transition_report_request;
};

struct cu_cp_path_switch_request_failure {
  cu_cp_ue_index_t                                 ue_index = cu_cp_ue_index_t::invalid;
  std::vector<cu_cp_pdu_session_res_released_item> pdu_session_res_released_list;
};

using cu_cp_path_switch_response = std::variant<cu_cp_path_switch_request_ack, cu_cp_path_switch_request_failure>;

} // namespace ocudu::ocucp
