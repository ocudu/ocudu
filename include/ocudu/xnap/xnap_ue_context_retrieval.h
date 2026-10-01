// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/byte_buffer.h"
#include "ocudu/ran/aggregate_maximum_bit_rate.h"
#include "ocudu/ran/cause/common.h"
#include "ocudu/ran/cause/xnap_cause.h"
#include "ocudu/ran/cu_cp_cell_configuration.h"
#include "ocudu/ran/cu_cp_location_reporting_types.h"
#include "ocudu/ran/cu_cp_pdu_session.h"
#include "ocudu/ran/cu_cp_types.h"
#include "ocudu/ran/guami.h"
#include "ocudu/ran/i_rnti.h"
#include "ocudu/ran/nr_cgi.h"
#include "ocudu/ran/pci.h"
#include "ocudu/ran/rnti.h"
#include "ocudu/security/security.h"
#include "ocudu/support/io/transport_layer_address.h"
#include "ocudu/xnap/xnap_types.h"
#include <chrono>
#include <optional>
#include <variant>

namespace ocudu::ocucp {

/// Common type UE Context ID for RRC Resume, one alternative of the UE Context ID IE of TS 38.423 section 9.2.3.40.
struct xnap_ue_context_id_for_rrc_resume {
  /// I-RNTI the UE included in the RRC Resume Request. The I-RNTI types have no default constructor, so the variant
  /// is explicitly initialized to keep the UE Context ID default-constructible.
  std::variant<short_i_rnti_t, full_i_rnti_t> i_rnti = short_i_rnti_t{short_i_rnti_profile::profile_0, 0, 0};
  /// C-RNTI allocated by the new NG-RAN node for the resuming UE.
  rnti_t allocated_c_rnti;
  /// PCI of the cell the UE accessed at the new NG-RAN node.
  pci_t access_pci;
};

/// Common type UE Context ID for RRC Reestablishment, one alternative of the UE Context ID IE of TS 38.423
/// section 9.2.3.40.
struct xnap_ue_context_id_for_rrc_reest {
  /// C-RNTI the UE had in the cell it declared a failure on.
  rnti_t c_rnti;
  /// PCI of the cell the UE declared a failure on.
  pci_t fail_cell_pci;
};

/// Common type UE Context ID, defined in TS 38.423 section 9.2.3.40. Identifies the UE at the old NG-RAN node, which
/// holds the context the new NG-RAN node is retrieving.
using xnap_ue_context_id = std::variant<xnap_ue_context_id_for_rrc_resume, xnap_ue_context_id_for_rrc_reest>;

/// Common type Retrieve UE Context Request, defined in TS 38.423 section 9.1.1.8.
struct xnap_retrieve_ue_context_request {
  /// Index of the UE at the node issuing the request. Filled by the old NG-RAN node with the resolved UE index once
  /// the UE Context ID has been matched against a local UE.
  cu_cp_ue_index_t ue_index = cu_cp_ue_index_t::invalid;
  /// Identity of the UE at the old NG-RAN node.
  xnap_ue_context_id ue_context_id;
  /// ShortMAC-I (RRC Reestablishment) or ResumeMAC-I (RRC Resume) the UE computed with the AS keys it had at the old
  /// NG-RAN node. Only that node can verify it, as only it holds those keys.
  security::sec_short_mac_i mac_i = {};
  /// Identity of the cell the UE accessed at the new NG-RAN node, carried by the New NG-RAN Cell Identity IE.
  /// Corresponds to the targetCellIdentity the UE used in VarShortMAC-Input/VarResumeMAC-Input (TS 38.331
  /// section 5.3.7.4), so the old NG-RAN node must verify the MAC-I against exactly this value.
  nr_cell_identity target_nci = nr_cell_identity::min();
  /// Configuration of the target cell, resolved by the old NG-RAN node from the served cell list the peer advertised
  /// at XN setup. Deriving KgNB* needs the target PCI and ARFCN-DL, and TS 33.501 section 6.11 has the old NG-RAN node
  /// obtain them "from a cell configuration database by means of the target Cell-ID". Empty when the target cell is
  /// not in the peer's served cell list.
  std::optional<cu_cp_served_cell_info> target_cell;
  /// Resume cause, only present when the UE Context ID identifies an RRC Resume.
  std::optional<resume_cause_t> rrc_resume_cause;
  /// How long the new NG-RAN node waits for the old one to answer. TS 38.423 defines no timer for this procedure, but
  /// the wait is spent inside the UE's T301 before the RRC Setup fallback can start, so the caller sizes it against
  /// T301.
  std::chrono::milliseconds max_response_time{1000};
};

/// Common type UE Context Information in the Retrieve UE Context Response, defined in TS 38.423 section 9.2.1.13.
struct xnap_ue_context_info_retrieve_ue_context_response {
  /// AMF UE NGAP ID the UE has at the AMF, carried by the NG-C UE associated Signalling reference IE. The AMF UE
  /// NGAP ID is 40 bits wide (TS 38.413 section 9.3.3.1), so it does not fit an unsigned.
  uint64_t amf_ue_id = 0;
  /// Address of the SCTP association the old NG-RAN node uses towards the AMF serving the UE.
  transport_layer_address                                               amf_addr;
  security::security_context                                            security_context;
  aggregate_maximum_bit_rate_t                                          ue_ambr;
  slotted_id_vector<pdu_session_id_t, cu_cp_pdu_session_res_setup_item> pdu_session_res_to_be_setup_list;
  /// Packed RRC HandoverPreparationInformation carrying the UE capabilities and the AS configuration the UE had at
  /// the old NG-RAN node.
  byte_buffer rrc_context;
};

/// Common type Retrieve UE Context Response, defined in TS 38.423 section 9.1.1.9. Also used to report a Retrieve UE
/// Context Failure (TS 38.423 section 9.1.1.10), in which case \c success is false and \c cause is set.
struct xnap_retrieve_ue_context_response {
  bool                                              success = false;
  guami_t                                           guami;
  xnap_ue_context_info_retrieve_ue_context_response ue_context_info;
  /// Location reporting the old NG-RAN node asks the new one to run for this UE (TS 38.423 section 8.2.4.2).
  std::optional<location_report_request> location_report_info;
  /// XNAP UE ID the peer allocated for this UE.
  peer_xnap_ue_id_t peer_xnap_ue_id = peer_xnap_ue_id_t::invalid;
  /// Cause of the failure. Only set when \c success is false.
  std::optional<xnap_cause_t> cause;
};

} // namespace ocudu::ocucp
