// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ue_context_retrieval_new_node_routine.h"

using namespace ocudu;
using namespace ocucp;

ue_context_retrieval_new_node_routine::ue_context_retrieval_new_node_routine(
    const rrc_ue_context_retrieval_request& request_,
    cu_cp_ue_index_t                        ue_index_,
    xnap_repository&                        xnap_db_,
    ue_manager&                             ue_mng_,
    ocudulog::basic_logger&                 logger_) :
  request(request_), ue_index(ue_index_), xnap_db(xnap_db_), ue_mng(ue_mng_), logger(logger_)
{
}

void ue_context_retrieval_new_node_routine::operator()(coro_context<async_task<rrc_ue_context_retrieval_response>>& ctx)
{
  CORO_BEGIN(ctx);

  logger.info("ue={}: \"{}\" started...", ue_index, name());

  // The caller falls back to RRC Setup when no peer holds the context.
  {
    std::optional<xnc_peer_index_t> peer_index = find_peer();
    if (!peer_index.has_value()) {
      CORO_EARLY_RETURN(rrc_ue_context_retrieval_response{});
    }
    xnc_index = peer_index.value();
  }

  xnap = xnap_db.find_xnap(xnc_index);
  if (xnap == nullptr) {
    logger.warning("ue={}: \"{}\" failed. Cause: XNAP with index {} not found", ue_index, name(), xnc_index);
    CORO_EARLY_RETURN(rrc_ue_context_retrieval_response{});
  }

  xnap_request.ue_index      = ue_index;
  xnap_request.ue_context_id = std::visit(
      [](const auto& ue_id) -> xnap_ue_context_id {
        using id_type = std::decay_t<decltype(ue_id)>;
        if constexpr (std::is_same_v<id_type, rrc_ue_context_retrieval_id_for_reest>) {
          return xnap_ue_context_id_for_rrc_reest{.c_rnti = ue_id.old_c_rnti, .fail_cell_pci = ue_id.old_pci};
        } else {
          return xnap_ue_context_id_for_rrc_resume{
              .i_rnti = ue_id.i_rnti, .allocated_c_rnti = ue_id.allocated_c_rnti, .access_pci = ue_id.access_pci};
        }
      },
      request.ue_id);
  xnap_request.mac_i             = request.mac_i;
  xnap_request.target_nci        = request.target_nci;
  xnap_request.max_response_time = request.max_response_time;

  CORO_AWAIT_VALUE(xnap_response, xnap->handle_retrieve_ue_context_required(xnap_request));

  CORO_RETURN(handle_retrieve_ue_context_response(std::move(xnap_response)));
}

void ue_context_retrieval_new_node_routine::release_xnap_ue_context()
{
  // The XNAP UE context was created to carry the request. A failed retrieval leaves this UE without a peer, so nothing
  // would remove it later: the UE keeps running and falls back to RRC Setup, and its removal only reaches the XNAP it
  // was associated with.
  if (xnap != nullptr) {
    xnap->get_xnap_ue_context_removal_handler().remove_ue_context(ue_index);
  }
}

std::optional<xnc_peer_index_t> ue_context_retrieval_new_node_routine::find_peer() const
{
  if (std::holds_alternative<rrc_ue_context_retrieval_id_for_reest>(request.ue_id)) {
    // The peer advertised the failure cell in its served cell list at XN setup.
    const pci_t                     old_pci    = std::get<rrc_ue_context_retrieval_id_for_reest>(request.ue_id).old_pci;
    std::optional<xnc_peer_index_t> peer_index = xnap_db.find_xnap_index_by_served_pci(old_pci);
    if (!peer_index.has_value()) {
      logger.debug("ue={}: \"{}\" failed. Cause: No Xn-C peer serves pci={}", ue_index, name(), old_pci);
    }
    return peer_index;
  }

  // The I-RNTI carries the Local NG-RAN Node Identifier of the node that allocated it, which is matched against the
  // gNB IDs of the connected peers (TS 38.300 Annex C). TS 38.300 section 9.2.2.6 has the node retrieve the context
  // "if able to resolve the gNB identity contained in the I-RNTI", so an unresolved peer leaves the caller to fall
  // back to RRC Setup.
  const auto& resume_id = std::get<rrc_ue_context_retrieval_id_for_resume>(request.ue_id);
  const auto  node_id   = std::visit([](const auto& i_rnti) { return i_rnti.node_id(); }, resume_id.i_rnti);
  const auto  nof_node_id_bits =
      std::visit([](const auto& i_rnti) { return std::decay_t<decltype(i_rnti)>::nof_node_id_bits(i_rnti.profile()); },
                 resume_id.i_rnti);

  std::optional<xnc_peer_index_t> peer_index = xnap_db.find_xnap_index_by_local_node_id(node_id, nof_node_id_bits);
  if (!peer_index.has_value()) {
    logger.debug(
        "ue={}: \"{}\" failed. Cause: No Xn-C peer carries node-id={:#x} of the I-RNTI", ue_index, name(), node_id);
  }
  return peer_index;
}

rrc_ue_context_retrieval_response
ue_context_retrieval_new_node_routine::handle_retrieve_ue_context_response(xnap_retrieve_ue_context_response response)
{
  if (!response.success) {
    logger.info("ue={}: \"{}\" failed. Cause: Peer rejected the retrieval", ue_index, name());
    release_xnap_ue_context();
    return {};
  }

  cu_cp_ue* ue = ue_mng.find_du_ue(ue_index);
  if (ue == nullptr) {
    logger.warning("ue={}: \"{}\" failed. Cause: UE not found", ue_index, name());
    release_xnap_ue_context();
    return {};
  }

  // Remember the peer, so the XNAP UE context is cleaned up when this UE is removed.
  ue->set_xnc_peer_index(xnc_index);

  // The AMBR is learned from the AMF at Initial Context Setup, which this UE went through at the peer, so the value
  // the peer reports is taken over.
  ue->set_ue_ambr(response.ue_context_info.ue_ambr);

  // Store what the bearer setup, the NGAP Path Switch and the release towards the peer take from the retrieval.
  cu_cp_ue_context_retrieval_context& retrieval_context = ue->get_context_retrieval_context().emplace();
  retrieval_context.peer_xnap_ue_id                     = response.peer_xnap_ue_id;
  retrieval_context.amf_ue_id                           = response.ue_context_info.amf_ue_id;
  retrieval_context.guami                               = response.guami;
  retrieval_context.pdu_session_res_to_be_setup_list =
      std::move(response.ue_context_info.pdu_session_res_to_be_setup_list);
  retrieval_context.rrc_context          = response.ue_context_info.rrc_context.copy();
  retrieval_context.location_report_info = response.location_report_info;

  logger.info("ue={}: \"{}\" finished successfully. Retrieved {} PDU sessions from xnc_peer={}",
              ue_index,
              name(),
              retrieval_context.pdu_session_res_to_be_setup_list.size(),
              xnc_index);

  return rrc_ue_context_retrieval_response{.success     = true,
                                           .sec_context = response.ue_context_info.security_context,
                                           .rrc_context = std::move(response.ue_context_info.rrc_context)};
}
