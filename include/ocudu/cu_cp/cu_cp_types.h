// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/byte_buffer.h"
#include "ocudu/pdcp/pdcp_config.h"
#include "ocudu/ran/cause/e1ap_cause.h"
#include "ocudu/ran/cause/f1ap_cause.h"
#include "ocudu/ran/cause/ngap_cause.h"
#include "ocudu/ran/cu_cp_types.h"
#include "ocudu/ran/cu_types.h"
#include "ocudu/ran/five_g_s_tmsi.h"
#include "ocudu/ran/nr_cgi.h"
#include "ocudu/ran/rb_id.h"
#include "ocudu/ran/rnti.h"
#include <cstdint>
#include <optional>
#include <vector>

namespace ocudu::ocucp {

/// Notification from the E1AP/F1AP that transaction reference information for some UEs has been lost.
struct ue_transaction_info_loss_event {
  std::vector<cu_cp_ue_index_t> ues_lost;
};

/// Common interface reset message for E1AP (E1 Reset), F1AP (F1 Reset) and NGAP (NG Reset).
struct cu_cp_reset {
  std::variant<e1ap_cause_t, f1ap_cause_t, ngap_cause_t> cause;
  bool                                                   interface_reset = false;
  std::vector<cu_cp_ue_index_t>                          ues_to_reset;
};

/// QoS Configuration, i.e. 5QI and the associated PDCP
/// and SDAP configuration for DRBs
struct cu_cp_qos_config {
  pdcp_config pdcp;
};

/// <AMF Identifier> = <AMF Region ID><AMF Set ID><AMF Pointer>
/// with AMF Region ID length is 8 bits, AMF Set ID length is 10 bits and AMF Pointer length is 6 bits
struct cu_cp_amf_identifier_t {
  uint8_t  amf_region_id = 0;
  uint16_t amf_set_id    = 0;
  uint8_t  amf_pointer   = 0;
};

struct cu_cp_initial_ue_message {
  cu_cp_ue_index_t               ue_index = cu_cp_ue_index_t::invalid;
  byte_buffer                    nas_pdu;
  establishment_cause_t          establishment_cause;
  cu_cp_user_location_info_nr    user_location_info;
  std::optional<five_g_s_tmsi_t> five_g_s_tmsi;
  std::optional<uint16_t>        amf_set_id;
};

struct cu_cp_ul_nas_transport {
  cu_cp_ue_index_t            ue_index = cu_cp_ue_index_t::invalid;
  byte_buffer                 nas_pdu;
  cu_cp_user_location_info_nr user_location_info;
};

/// \brief Indication from a DU that a UE has successfully accessed a target cell (CHO execution).
struct cu_cp_access_success_indication {
  cu_cp_ue_index_t    ue_index        = cu_cp_ue_index_t::invalid; ///< Target UE index (sender of Access Success).
  cu_cp_ue_index_t    source_ue_index = cu_cp_ue_index_t::invalid; ///< Resolved CHO source UE index.
  nr_cell_global_id_t cgi;
};

struct cu_cp_bearer_context_release_request {
  cu_cp_ue_index_t ue_index = cu_cp_ue_index_t::invalid;
  ngap_cause_t     cause;
};

struct cu_cp_inactivity_notification {
  cu_cp_ue_index_t              ue_index    = cu_cp_ue_index_t::invalid;
  bool                          ue_inactive = false;
  std::vector<drb_id_t>         inactive_drbs;
  std::vector<pdu_session_id_t> inactive_pdu_sessions;
};

struct cu_cp_rrc_resume_request {
  cu_cp_ue_index_t    ue_index = cu_cp_ue_index_t::invalid;
  nr_cell_global_id_t cgi;
  rnti_t              new_c_rnti;
  resume_cause_t      cause;
};

} // namespace ocudu::ocucp
