// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/cu_cp/cu_cp_location_reporting_types.h"
#include "ocudu/cu_cp/inter_cu_handover_messages.h"
#include "ocudu/ran/cause/xnap_cause.h"
#include "ocudu/ran/cu_cp_pdu_session.h"
#include "ocudu/ran/cu_cp_types.h"
#include "ocudu/ran/cu_types.h"
#include "ocudu/ran/guami.h"
#include "ocudu/ran/nr_cgi.h"
#include "ocudu/security/security.h"
#include "ocudu/support/io/transport_layer_address.h"

namespace ocudu::ocucp {

/// Related to common type XNAP handover request, defined in TS 38.423 section 9.1.1.1.
struct xnap_ue_context_info_ho_request {
  unsigned                                                              amf_ue_id;
  transport_layer_address                                               amf_addr;
  security::security_context                                            security_context;
  aggregate_maximum_bit_rate_t                                          ue_ambr;
  slotted_id_vector<pdu_session_id_t, cu_cp_pdu_session_res_setup_item> pdu_session_res_to_be_setup_list;
  byte_buffer                                                           rrc_handover_preparation_information;
  std::optional<location_report_request>                                location_report_info;
};

/// Common type XNAP handover request, defined in TS 38.423 section 9.1.1.1.
struct xnap_handover_request {
  cu_cp_ue_index_t                ue_index;
  xnap_cause_t                    cause;
  nr_cell_global_id_t             nr_cgi;
  guami_t                         guami;
  xnap_ue_context_info_ho_request ue_context_info_ho_request;
  // TODO: add optional fields.
};

struct xnap_handover_preparation_response {
  bool success = false;
};

struct xnap_handover_target_execution_context {
  xnc_peer_index_t                                    xnc_index;
  cu_cp_ue_index_t                                    ue_index;
  unsigned                                            amf_ue_id;
  std::vector<cu_cp_xn_pdu_session_res_admitted_item> pdu_session_res_admitted_list;
  std::vector<cu_cp_pdu_session_with_cause_item>      pdu_session_failed_to_setup_list;
};

} // namespace ocudu::ocucp
