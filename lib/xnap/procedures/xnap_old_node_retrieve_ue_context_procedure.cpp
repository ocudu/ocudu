// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "xnap_old_node_retrieve_ue_context_procedure.h"
#include "retrieve_ue_context_asn1_helpers.h"
#include "ocudu/asn1/xnap/common.h"
#include "ocudu/xnap/xnap_message.h"

using namespace ocudu;
using namespace ocucp;
using namespace asn1::xnap;

xnap_old_node_retrieve_ue_context_procedure::xnap_old_node_retrieve_ue_context_procedure(
    const xnap_retrieve_ue_context_request& request_,
    peer_xnap_ue_id_t                       peer_xnap_ue_id_,
    xnap_ue_context_list&                   ue_ctxt_list_,
    xnap_cu_cp_notifier&                    cu_cp_notifier_,
    xnap_message_notifier&                  tx_notifier_,
    ocudulog::basic_logger&                 logger_) :
  request(request_),
  peer_xnap_ue_id(peer_xnap_ue_id_),
  ue_ctxt_list(ue_ctxt_list_),
  cu_cp_notifier(cu_cp_notifier_),
  tx_notifier(tx_notifier_),
  logger(logger_)
{
}

void xnap_old_node_retrieve_ue_context_procedure::operator()(coro_context<async_task<void>>& ctx)
{
  CORO_BEGIN(ctx);

  logger.info("ue={}: \"{}\" started...", request.ue_index, name());

  // Collect the UE context to transfer. The CU-CP verifies the MAC-I and derives KgNB* for the cell the UE moved to.
  CORO_AWAIT_VALUE(response, cu_cp_notifier.on_xnap_retrieve_ue_context_request(request));

  if (!response.success) {
    logger.info("ue={}: \"{}\" failed. Rejecting the retrieval", request.ue_index, name());
    send_retrieve_ue_context_failure(response.cause.value_or(xnap_cause_radio_network_t::unspecified));
    CORO_EARLY_RETURN();
  }

  // The UE context is only associated over Xn once the retrieval is accepted, so that a rejected request leaves no
  // XNAP UE context behind.
  if (!create_xnap_ue()) {
    send_retrieve_ue_context_failure(xnap_cause_radio_network_t::unspecified);
    CORO_EARLY_RETURN();
  }

  send_retrieve_ue_context_response();

  logger.info("ue={}: \"{}\" finished successfully", request.ue_index, name());

  CORO_RETURN();
}

bool xnap_old_node_retrieve_ue_context_procedure::create_xnap_ue()
{
  local_xnap_ue_id = ue_ctxt_list.allocate_local_xnap_ue_id();
  if (local_xnap_ue_id == local_xnap_ue_id_t::invalid) {
    logger.error("ue={}: No local XNAP UE ID available", request.ue_index);
    return false;
  }

  ue_ctxt_list.add_ue(request.ue_index, local_xnap_ue_id);
  ue_ctxt_list.update_peer_xnap_ue_id(local_xnap_ue_id, peer_xnap_ue_id);

  ue_ctxt_list[request.ue_index].logger.log_debug("Created UE");

  return true;
}

void xnap_old_node_retrieve_ue_context_procedure::send_retrieve_ue_context_response()
{
  xnap_message xnap_msg;
  xnap_msg.pdu.set_successful_outcome();
  xnap_msg.pdu.successful_outcome().load_info_obj(ASN1_XNAP_ID_RETRIEVE_UE_CONTEXT);

  auto& asn1_response = xnap_msg.pdu.successful_outcome().value.retrieve_ue_context_resp();

  // This is sent from the old to the new NG-RAN node, so the new NG-RAN node UE XnAP ID is the peer XNAP UE ID and
  // the old NG-RAN node UE XnAP ID is the local XNAP UE ID.
  asn1_response->new_ng_ra_nnode_ue_xn_ap_id = to_underlying(peer_xnap_ue_id);
  asn1_response->old_ng_ra_nnode_ue_xn_ap_id = to_underlying(local_xnap_ue_id);

  retrieve_ue_context_response_to_asn1(asn1_response, response);

  if (!tx_notifier.on_new_message(xnap_msg)) {
    logger.warning("Xn-C association is not set. Cannot send RetrieveUEContextResponse");
  }
}

void xnap_old_node_retrieve_ue_context_procedure::send_retrieve_ue_context_failure(const xnap_cause_t& cause)
{
  xnap_message xnap_msg;
  xnap_msg.pdu.set_unsuccessful_outcome();
  xnap_msg.pdu.unsuccessful_outcome().load_info_obj(ASN1_XNAP_ID_RETRIEVE_UE_CONTEXT);

  auto& asn1_failure = xnap_msg.pdu.unsuccessful_outcome().value.retrieve_ue_context_fail();

  // This is sent from the old to the new NG-RAN node, so the new NG-RAN node UE XnAP ID is the peer XNAP UE ID.
  asn1_failure->new_ng_ra_nnode_ue_xn_ap_id = to_underlying(peer_xnap_ue_id);

  retrieve_ue_context_failure_to_asn1(asn1_failure, cause);

  if (!tx_notifier.on_new_message(xnap_msg)) {
    logger.warning("Xn-C association is not set. Cannot send RetrieveUEContextFailure");
  }
}
