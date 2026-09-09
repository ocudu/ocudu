// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/nr_cgi.h"
#include "ocudu/ran/pci.h"
#include "ocudu/ran/rb_id.h"
#include "ocudu/xnap/xnap.h"
#include "ocudu/xnap/xnap_configuration.h"
#include "ocudu/xnap/xnap_message.h"
#include "ocudu/xnap/xnap_types.h"
#include <vector>

namespace ocudu::ocucp {

/// \brief Generate the information of an NR cell an NG-RAN node serves, as advertised at XN setup.
cu_cp_served_cell_info generate_served_cell_info(pci_t pci, const nr_cell_global_id_t& cgi, tac_t tac = 7);

/// \brief Generate an XN Setup Response that advertises a single served NR cell, so that the peer context stores a
/// served cell list. Used to test lookups of the Xn-C peer by served cell PCI.
xnap_message generate_xn_setup_response_with_served_cell(const xnap_configuration&  peer_cfg,
                                                         pci_t                      served_pci,
                                                         const nr_cell_global_id_t& served_cgi);

/// \brief Generate a dummy Handover Request message. \c include_drb_to_qos_flow_mapping controls whether the
/// source's DRB-to-QoS-flow mapping (DRB1 <-> QFI1, matching the admitted PDU session) is included via the Data
/// Forwarding and Offloading Info from source NG-RAN node IE, letting the target confirm and prefer DRB1's
/// numbering (TS 38.423 Section 9.2.1.17). Pass false to simulate a source that didn't signal it.
/// \c include_as_config_drb_mapping reports the same mapping through AS-Config in the RRC container instead
/// (TS 38.331 Section 11.2.3), as this node does; use it to simulate a source that only signals it that way.
xnap_message generate_handover_request(local_xnap_ue_id_t local_xnap_ue_id,
                                       bool               include_drb_to_qos_flow_mapping = true,
                                       bool               include_as_config_drb_mapping   = false);

/// \brief Generate a dummy Handover Preparation Failure message.
xnap_message generate_handover_preparation_failure(peer_xnap_ue_id_t peer_xnap_ue_id);

/// \brief Generate a dummy Handover Request Ack message.
xnap_message generate_handover_request_ack(local_xnap_ue_id_t local_xnap_ue_id, peer_xnap_ue_id_t peer_xnap_ue_id);

/// \brief Generate a Handover Cancel message scoped to one CHO candidate cell. The arguments take the sender's view:
/// \c local_xnap_ue_id fills the Source NG-RAN node UE XnAP ID and \c peer_xnap_ue_id the Target one.
xnap_message generate_handover_cancel(local_xnap_ue_id_t         local_xnap_ue_id,
                                      peer_xnap_ue_id_t          peer_xnap_ue_id,
                                      const nr_cell_global_id_t& cell);

/// \brief Generate a dummy SN RAN Status Transfer message. \c extra_drb_ids adds further DRBs to the DRBs Subject to
/// Status Transfer List IE, e.g. to simulate a source DRB that was not admitted at the target.
xnap_message generate_sn_status_transfer(local_xnap_ue_id_t           local_xnap_ue_id,
                                         peer_xnap_ue_id_t            peer_xnap_ue_id,
                                         const std::vector<drb_id_t>& extra_drb_ids = {});

/// \brief Generate a dummy UE Context Release message.
xnap_message generate_ue_context_release(local_xnap_ue_id_t local_xnap_ue_id, peer_xnap_ue_id_t peer_xnap_ue_id);

/// \brief Generate a dummy Retrieve UE Context Request message carrying an RRC Reestablishment UE Context ID.
xnap_message generate_retrieve_ue_context_request(peer_xnap_ue_id_t peer_xnap_ue_id,
                                                  pci_t             fail_cell_pci,
                                                  nr_cell_identity  target_nci);

/// \brief Generate a dummy Retrieve UE Context Request message carrying an RRC Resume UE Context ID.
xnap_message generate_retrieve_ue_context_request_for_resume(peer_xnap_ue_id_t peer_xnap_ue_id,
                                                             short_i_rnti_t    i_rnti,
                                                             nr_cell_identity  target_nci,
                                                             uint16_t          resume_mac_i = 0xabcd);

/// \brief Generate a dummy Retrieve UE Context Response message.
xnap_message generate_retrieve_ue_context_response(local_xnap_ue_id_t local_xnap_ue_id,
                                                   peer_xnap_ue_id_t  peer_xnap_ue_id);

/// \brief Generate a dummy Retrieve UE Context Failure message.
xnap_message generate_retrieve_ue_context_failure(local_xnap_ue_id_t local_xnap_ue_id);

} // namespace ocudu::ocucp
