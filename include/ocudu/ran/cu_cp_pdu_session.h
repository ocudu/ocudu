// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/byte_buffer.h"
#include "ocudu/adt/slotted_vector.h"
#include "ocudu/ran/aggregate_maximum_bit_rate.h"
#include "ocudu/ran/cause/ngap_cause.h"
#include "ocudu/ran/crit_diagnostics.h"
#include "ocudu/ran/cu_cp_types.h"
#include "ocudu/ran/cu_types.h"
#include "ocudu/ran/s_nssai.h"
#include "ocudu/ran/up_transport_layer_info.h"
#include <optional>
#include <string>
#include <vector>

namespace ocudu::ocucp {

struct qos_flow_setup_request_item {
  qos_flow_id_t                 qos_flow_id = qos_flow_id_t::invalid;
  qos_flow_level_qos_parameters qos_flow_level_qos_params;
  std::optional<uint8_t>        erab_id;
};

struct cu_cp_pdu_session_res_setup_item {
  pdu_session_id_t                                              pdu_session_id = pdu_session_id_t::invalid;
  byte_buffer                                                   pdu_session_nas_pdu;
  s_nssai_t                                                     s_nssai;
  std::optional<uint64_t>                                       pdu_session_aggregate_maximum_bit_rate_dl;
  std::optional<uint64_t>                                       pdu_session_aggregate_maximum_bit_rate_ul;
  up_transport_layer_info                                       ul_ngu_up_tnl_info;
  pdu_session_type_t                                            pdu_session_type;
  std::optional<security_indication_t>                          security_ind;
  slotted_id_vector<qos_flow_id_t, qos_flow_setup_request_item> qos_flow_setup_request_items;
};

struct cu_cp_pdu_session_resource_setup_request {
  cu_cp_ue_index_t                                                      ue_index = cu_cp_ue_index_t::invalid;
  slotted_id_vector<pdu_session_id_t, cu_cp_pdu_session_res_setup_item> pdu_session_res_setup_items;
  aggregate_maximum_bit_rate_t                                          ue_ambr;
  plmn_identity                                                         serving_plmn = plmn_identity::test_value();
  byte_buffer                                                           nas_pdu; ///< optional NAS PDU
};

enum class cu_cp_qos_flow_map_ind { ul = 0, dl };

struct cu_cp_associated_qos_flow {
  qos_flow_id_t                         qos_flow_id = qos_flow_id_t::invalid;
  std::optional<cu_cp_qos_flow_map_ind> qos_flow_map_ind;
};

struct cu_cp_qos_flow_failed_to_setup_item {
  qos_flow_id_t qos_flow_id = qos_flow_id_t::invalid;
  ngap_cause_t  cause;
};

struct cu_cp_qos_flow_per_tnl_information {
  up_transport_layer_info                                     up_tp_layer_info;
  slotted_id_vector<qos_flow_id_t, cu_cp_associated_qos_flow> associated_qos_flow_list;
};

struct cu_cp_pdu_session_resource_setup_response_transfer {
  std::vector<cu_cp_qos_flow_per_tnl_information>                       add_dl_qos_flow_per_tnl_info;
  cu_cp_qos_flow_per_tnl_information                                    dlqos_flow_per_tnl_info;
  slotted_id_vector<qos_flow_id_t, cu_cp_associated_qos_flow>           associated_qos_flow_list;
  slotted_id_vector<qos_flow_id_t, cu_cp_qos_flow_failed_to_setup_item> qos_flow_failed_to_setup_list;
  std::optional<security_result_t>                                      security_result;
};

struct cu_cp_pdu_session_res_setup_response_item {
  pdu_session_id_t                                   pdu_session_id = pdu_session_id_t::invalid;
  cu_cp_pdu_session_resource_setup_response_transfer pdu_session_resource_setup_response_transfer;
};

struct cu_cp_pdu_session_resource_setup_unsuccessful_transfer {
  ngap_cause_t                      cause;
  std::optional<crit_diagnostics_t> crit_diagnostics;
};

struct cu_cp_pdu_session_res_setup_failed_item {
  pdu_session_id_t                                       pdu_session_id = pdu_session_id_t::invalid;
  cu_cp_pdu_session_resource_setup_unsuccessful_transfer unsuccessful_transfer;
};

struct cu_cp_pdu_session_resource_setup_response {
  slotted_id_vector<pdu_session_id_t, cu_cp_pdu_session_res_setup_response_item> pdu_session_res_setup_response_items;
  slotted_id_vector<pdu_session_id_t, cu_cp_pdu_session_res_setup_failed_item>   pdu_session_res_failed_to_setup_items;
  std::optional<crit_diagnostics_t>                                              crit_diagnostics;
};

struct cu_cp_pdu_session_res_release_cmd_transfer {
  ngap_cause_t cause;
};

struct cu_cp_pdu_session_res_to_release_item_rel_cmd {
  pdu_session_id_t                           pdu_session_id = pdu_session_id_t::invalid;
  cu_cp_pdu_session_res_release_cmd_transfer pdu_session_res_release_cmd_transfer;
};

struct cu_cp_pdu_session_resource_release_command {
  cu_cp_ue_index_t        ue_index = cu_cp_ue_index_t::invalid;
  std::optional<uint16_t> ran_paging_prio;
  byte_buffer             nas_pdu;
  slotted_id_vector<pdu_session_id_t, cu_cp_pdu_session_res_to_release_item_rel_cmd>
      pdu_session_res_to_release_list_rel_cmd;
};

struct cu_cp_volume_timed_report_item {
  uint64_t start_time_stamp;
  uint64_t end_time_stamp;
  uint64_t usage_count_ul;
  uint64_t usage_count_dl;
};

struct cu_cp_pdu_session_usage_report {
  std::string                                 rat_type;
  std::vector<cu_cp_volume_timed_report_item> pdu_session_timed_report_list;
};

struct cu_cp_qos_flows_usage_report_item {
  qos_flow_id_t                               qos_flow_id;
  std::string                                 rat_type;
  std::vector<cu_cp_volume_timed_report_item> qos_flows_timed_report_list;
};

struct cu_cp_secondary_rat_usage_info {
  std::optional<cu_cp_pdu_session_usage_report>                       pdu_session_usage_report;
  slotted_id_vector<qos_flow_id_t, cu_cp_qos_flows_usage_report_item> qos_flows_usage_report_list;
};

struct cu_cp_pdu_session_res_release_resp_transfer {
  std::optional<cu_cp_secondary_rat_usage_info> secondary_rat_usage_info;
};

struct cu_cp_pdu_session_res_released_item_rel_res {
  pdu_session_id_t                            pdu_session_id = pdu_session_id_t::invalid;
  cu_cp_pdu_session_res_release_resp_transfer resp_transfer;
};

struct cu_cp_pdu_session_resource_release_response {
  slotted_id_vector<pdu_session_id_t, cu_cp_pdu_session_res_released_item_rel_res> released_pdu_sessions;
  std::optional<cu_cp_user_location_info_nr>                                       user_location_info;
  std::optional<crit_diagnostics_t>                                                crit_diagnostics;
};

using cu_cp_qos_flow_add_or_mod_item = qos_flow_setup_request_item;

struct cu_cp_pdu_session_res_modify_request_transfer {
  // All IEs are optional
  // id-PDUSessionAggregateMaximumBitRate
  // id-UL-NGU-UP-TNLModifyList
  // id-NetworkInstance
  // id-QosFlowAddOrModifyRequestList
  slotted_id_vector<qos_flow_id_t, cu_cp_qos_flow_add_or_mod_item> qos_flow_add_or_modify_request_list;
  // id-QosFlowToReleaseList
  slotted_id_vector<qos_flow_id_t, cu_cp_qos_flow_failed_to_setup_item> qos_flow_to_release_list;
  // id-AdditionalUL-NGU-UP-TNLInformation
  // id-CommonNetworkInstance
  // id-AdditionalRedundantUL-NGU-UP-TNLInformation
  // id-RedundantCommonNetworkInstance
};

struct cu_cp_pdu_session_res_modify_item_mod_req {
  pdu_session_id_t                              pdu_session_id = pdu_session_id_t::invalid;
  byte_buffer                                   nas_pdu;
  cu_cp_pdu_session_res_modify_request_transfer transfer;
};

struct cu_cp_pdu_session_resource_modify_request {
  cu_cp_ue_index_t                                                               ue_index = cu_cp_ue_index_t::invalid;
  slotted_id_vector<pdu_session_id_t, cu_cp_pdu_session_res_modify_item_mod_req> pdu_session_res_modify_items;
};

using cu_cp_pdu_session_resource_failed_to_modify_item = cu_cp_pdu_session_res_setup_failed_item;

struct qos_flow_add_or_mod_response_item {
  qos_flow_id_t qos_flow_id = qos_flow_id_t::invalid;
};

struct cu_cp_pdu_session_res_modify_response_transfer {
  std::vector<cu_cp_qos_flow_per_tnl_information> add_dl_qos_flow_per_tnl_info;
  std::optional<slotted_id_vector<qos_flow_id_t, qos_flow_add_or_mod_response_item>>
      qos_flow_add_or_modify_response_list;
};

struct cu_cp_pdu_session_resource_modify_response_item {
  pdu_session_id_t                               pdu_session_id;
  cu_cp_pdu_session_res_modify_response_transfer transfer;
};

struct cu_cp_pdu_session_resource_modify_response {
  cu_cp_ue_index_t ue_index = cu_cp_ue_index_t::invalid;
  // id-PDUSessionResourceModifyListModRes
  slotted_id_vector<pdu_session_id_t, cu_cp_pdu_session_resource_modify_response_item> pdu_session_res_modify_list;
  // id-PDUSessionResourceFailedToModifyListModRes
  slotted_id_vector<pdu_session_id_t, cu_cp_pdu_session_resource_failed_to_modify_item>
      pdu_session_res_failed_to_modify_list;
  // id-UserLocationInformation
  // id-CriticalityDiagnostics
};

} // namespace ocudu::ocucp
