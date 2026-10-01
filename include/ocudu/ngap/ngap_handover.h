// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/aggregate_maximum_bit_rate.h"
#include "ocudu/ran/cu_cp_inactive.h"
#include "ocudu/ran/cu_cp_location_reporting_types.h"
#include "ocudu/ran/cu_cp_pdu_session.h"
#include "ocudu/ran/cu_cp_types.h"
#include "ocudu/ran/guami.h"
#include "ocudu/ran/inter_cu_handover_messages.h"
#include "ocudu/ran/plmn_identity.h"
#include "ocudu/ran/tac.h"
#include "ocudu/security/security.h"
#include <map>

namespace ocudu::ocucp {

struct ngap_ue_source_handover_context {
  /// This source's own DRB-to-QoS-flow mapping per PDU session, so it can be signalled to the target via the DRBs
  /// to QoS Flows Mapping List IE (TS 38.413 Section 9.3.1.29), letting the target prefer the same DRB numbering.
  std::map<pdu_session_id_t, std::vector<cu_cp_drbs_to_qos_flows_map_item>> pdu_sessions;
  /// Storage for the RRCContainer required in SourceNGRANNode-ToTargetNGRANNode-TransparentContainer (see TS 38.413)
  byte_buffer rrc_container;
};

struct target_ran_node_id_t {
  gnb_id_t      gnb_id;
  plmn_identity plmn = plmn_identity::test_value();
  tac_t         tac;
};

struct ngap_handover_preparation_request {
  cu_cp_ue_index_t                                                          ue_index = cu_cp_ue_index_t::invalid;
  target_ran_node_id_t                                                      target_id;
  nr_cell_identity                                                          nci;
  std::map<pdu_session_id_t, std::vector<cu_cp_drbs_to_qos_flows_map_item>> pdu_sessions;
};

struct ngap_handover_preparation_response {
  // Place-holder for possible return values.
  bool success = false;
};

enum class ngap_handov_type { intra5gs = 0, fivegs_to_eps, eps_to_5gs, fivegs_to_utran };

struct ngap_erab_info_item {
  uint8_t             erab_id;
  std::optional<bool> dl_forwarding;
};

enum class ngap_cell_size { verysmall = 0, small, medium, large };

struct ngap_cell_type {
  ngap_cell_size cell_size;
};

struct ngap_last_visited_ngran_cell_info {
  nr_cell_global_id_t         global_cell_id;
  ngap_cell_type              cell_type;
  uint16_t                    time_ue_stayed_in_cell;
  std::optional<uint16_t>     time_ue_stayed_in_cell_enhanced_granularity;
  std::optional<ngap_cause_t> ho_cause_value;
};

struct ngap_last_visited_cell_item {
  ngap_last_visited_ngran_cell_info last_visited_cell_info;
};

struct ngap_source_ngran_node_to_target_ngran_node_transparent_container {
  byte_buffer                                  rrc_container;
  std::vector<cu_cp_pdu_session_res_info_item> pdu_session_res_info_list;
  std::vector<ngap_erab_info_item>             erab_info_list;
  nr_cell_global_id_t                          target_cell_id;
  std::optional<uint16_t>                      idx_to_rfsp;
  std::vector<ngap_last_visited_cell_item>     ue_history_info;
};

struct ngap_handover_request {
  cu_cp_ue_index_t                                                      ue_index = cu_cp_ue_index_t::invalid;
  ngap_handov_type                                                      handov_type;
  ngap_cause_t                                                          cause;
  aggregate_maximum_bit_rate_t                                          ue_aggr_max_bit_rate;
  std::optional<cu_cp_core_network_assist_info_for_inactive>            core_network_assist_info_for_inactive;
  security::security_context                                            security_context;
  std::optional<bool>                                                   new_security_context_ind;
  byte_buffer                                                           nasc;
  slotted_id_vector<pdu_session_id_t, cu_cp_pdu_session_res_setup_item> pdu_session_res_setup_list_ho_req;
  std::vector<s_nssai_t>                                                allowed_nssai;
  // TODO: Add optional trace_activation
  std::optional<uint64_t>                                           masked_imeisv;
  ngap_source_ngran_node_to_target_ngran_node_transparent_container source_to_target_transparent_container;
  // TODO: Add optional mob_restrict_list
  std::optional<location_report_request>                      location_report_request_type;
  std::optional<cu_cp_rrc_inactive_transition_report_request> rrc_inactive_transition_report_request;
  guami_t                                                     guami;
  // TODO: Add optional redirection_voice_fallback
  // TODO: Add optional cn_assisted_ran_tuning
};

} // namespace ocudu::ocucp
