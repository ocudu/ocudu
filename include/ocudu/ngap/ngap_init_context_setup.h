// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/cu_cp/cu_cp_location_reporting_types.h"
#include "ocudu/ngap/ngap_pdu_session.h"
#include "ocudu/ngap/ngap_types.h"
#include "ocudu/ran/crit_diagnostics.h"
#include "ocudu/ran/cu_cp_types.h"
#include "ocudu/ran/guami.h"
#include "ocudu/security/security.h"

namespace ocudu::ocucp {

struct ngap_init_context_setup_request {
  cu_cp_ue_index_t                                          ue_index = cu_cp_ue_index_t::invalid;
  std::optional<std::string>                                old_amf;
  std::optional<aggregate_maximum_bit_rate_t>               ue_aggr_max_bit_rate;
  std::optional<ngap_core_network_assist_info_for_inactive> core_network_assist_info_for_inactive;
  guami_t                                                   guami;
  std::optional<ngap_pdu_session_resource_setup_request>    pdu_session_res_setup_list_cxt_req;
  std::vector<s_nssai_t>                                    allowed_nssai;
  security::security_context                                security_context;
  // TODO: Add optional trace_activation
  // TODO: Add optional mob_restrict_list
  std::optional<byte_buffer> ue_radio_cap;
  std::optional<uint16_t>    idx_to_rfsp;
  std::optional<uint64_t>    masked_imeisv;
  std::optional<byte_buffer> nas_pdu;
  // TODO: Add optional emergency_fallback_ind
  std::optional<ngap_rrc_inactive_transition_report_request> rrc_inactive_transition_report_request;
  std::optional<cu_cp_ue_radio_cap_for_paging>               ue_radio_cap_for_paging;
  // TODO: Add optional redirection_voice_fallback
  std::optional<location_report_request> location_report_request_type;
  // TODO: Add optional cn_assisted_ran_tuning
};

struct ngap_init_context_setup_failure {
  ngap_cause_t                                                                cause;
  slotted_id_vector<pdu_session_id_t, ngap_pdu_session_res_setup_failed_item> pdu_session_res_failed_to_setup_items;
  std::optional<crit_diagnostics_t>                                           crit_diagnostics;
};

struct ngap_init_context_setup_response {
  slotted_id_vector<pdu_session_id_t, ngap_pdu_session_res_setup_response_item> pdu_session_res_setup_response_items;
  slotted_id_vector<pdu_session_id_t, ngap_pdu_session_res_setup_failed_item>   pdu_session_res_failed_to_setup_items;
  std::optional<crit_diagnostics_t>                                             crit_diagnostics;
};

} // namespace ocudu::ocucp
