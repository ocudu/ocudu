// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../../ue_manager/ue_manager_impl.h"
#include "../../xnap_repository.h"
#include "ocudu/rrc/rrc_ue.h"
#include "ocudu/support/async/async_task.h"

namespace ocudu::ocucp {

/// \brief New NG-RAN node side of the UE context retrieval procedure (TS 38.423 section 8.2.4).
///
/// Runs when a UE reestablishes or resumes at this node but no local UE context matches the identity it presented,
/// because the context is still held by a peer NG-RAN node. Resolves that peer, retrieves the context over Xn and
/// stores what the subsequent bearer setup, Path Switch and release towards the peer need.
///
/// How the peer is resolved follows the identity: a reestablishing UE reports the PCI of the cell it failed on, which
/// the peer advertised in its served cell list at XN setup, while a resuming UE reports an I-RNTI, which encodes the
/// gNB ID of the node that allocated it (TS 38.300 Annex F).
///
/// The retrieval must complete before this node answers the UE, as Msg4 is integrity protected with keys derived from
/// the retrieved KgNB*. It therefore runs inside the timer the UE started when it sent Msg3: T301 for a
/// reestablishment, T319 for a resume.
class ue_context_retrieval_new_node_routine
{
public:
  ue_context_retrieval_new_node_routine(const rrc_ue_context_retrieval_request& request_,
                                        cu_cp_ue_index_t                        ue_index_,
                                        xnap_repository&                        xnap_db_,
                                        ue_manager&                             ue_mng_,
                                        ocudulog::basic_logger&                 logger_);

  void operator()(coro_context<async_task<rrc_ue_context_retrieval_response>>& ctx);

  static const char* name() { return "UE Context Retrieval New NG-RAN Node Routine"; }

private:
  /// Resolves the peer holding the UE context from the identity the UE presented.
  std::optional<xnc_peer_index_t> find_peer() const;

  /// Removes the XNAP UE context created for a retrieval that did not succeed.
  void release_xnap_ue_context();

  /// Stores the retrieved context on the UE and builds the response for the RRC.
  rrc_ue_context_retrieval_response
  handle_retrieve_ue_context_response(xnap_retrieve_ue_context_response xnap_response);

  const rrc_ue_context_retrieval_request request;
  const cu_cp_ue_index_t                 ue_index;
  xnap_repository&                       xnap_db;
  ue_manager&                            ue_mng;
  ocudulog::basic_logger&                logger;

  xnc_peer_index_t                  xnc_index = xnc_peer_index_t::invalid;
  xnap_interface*                   xnap      = nullptr;
  xnap_retrieve_ue_context_request  xnap_request;
  xnap_retrieve_ue_context_response xnap_response;
};

} // namespace ocudu::ocucp
