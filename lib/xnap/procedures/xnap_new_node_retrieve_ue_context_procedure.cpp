// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "xnap_new_node_retrieve_ue_context_procedure.h"
#include "../xnap_asn1_utils.h"
#include "retrieve_ue_context_asn1_helpers.h"
#include "ocudu/asn1/xnap/common.h"
#include "ocudu/ocudulog/ocudulog.h"
#include "ocudu/xnap/xnap_message.h"
#include "ocudu/xnap/xnap_types.h"

using namespace ocudu;
using namespace ocucp;
using namespace asn1::xnap;

xnap_new_node_retrieve_ue_context_procedure::xnap_new_node_retrieve_ue_context_procedure(
    const xnap_retrieve_ue_context_request& request_,
    xnap_ue_context_list&                   ue_ctxt_list_,
    xnap_message_notifier&                  tx_notifier_) :
  request(request_), ue_ctxt_list(ue_ctxt_list_), tx_notifier(tx_notifier_)
{
}

void xnap_new_node_retrieve_ue_context_procedure::operator()(
    coro_context<async_task<xnap_retrieve_ue_context_response>>& ctx)
{
  CORO_BEGIN(ctx);

  ue_ctxt = ue_ctxt_list.find(request.ue_index);
  if (ue_ctxt == nullptr) {
    ocudulog::fetch_basic_logger("XNAP").error(
        "ue={}: \"{}\" failed. Cause: UE context not found", request.ue_index, name());
    CORO_EARLY_RETURN(xnap_retrieve_ue_context_response{});
  }

  ue_ctxt->logger.log_info("\"{}\" started...", name());

  if (ue_ctxt->ue_ids.local_xnap_ue_id == local_xnap_ue_id_t::invalid) {
    ue_ctxt->logger.log_error("\"{}\" failed. Cause: Invalid LOCAL XNAP UE ID", name());
    CORO_EARLY_RETURN(xnap_retrieve_ue_context_response{});
  }

  // Subscribe to the publisher before sending, to receive the RETRIEVE UE CONTEXT RESPONSE/FAILURE message.
  transaction_sink.subscribe_to(ue_ctxt->retrieve_ue_context_outcome, request.max_response_time);

  if (!send_retrieve_ue_context_request()) {
    ue_ctxt->logger.log_warning("\"{}\" failed. Cause: Could not send Retrieve UE Context Request", name());
    CORO_EARLY_RETURN(xnap_retrieve_ue_context_response{});
  }

  CORO_AWAIT(transaction_sink);

  if (!transaction_sink.successful()) {
    if (transaction_sink.timeout_expired()) {
      ue_ctxt->logger.log_warning(
          "\"{}\" failed. Cause: Timeout receiving Retrieve UE Context Response/Failure after {}ms",
          name(),
          request.max_response_time.count());
    } else if (transaction_sink.failed()) {
      ue_ctxt->logger.log_warning("\"{}\" failed. Cause: Received Retrieve UE Context Failure ({})",
                                  name(),
                                  asn1_utils::get_cause_str(transaction_sink.failure()->cause));
    } else {
      // Neither a Retrieve UE Context Failure nor a timeout, e.g. the transaction was cancelled because XNAP is
      // stopping.
      ue_ctxt->logger.log_warning("\"{}\" failed. Cause: Transaction cancelled", name());
    }

    CORO_EARLY_RETURN(xnap_retrieve_ue_context_response{});
  }

  if (!asn1_to_retrieve_ue_context_response(response, transaction_sink.response())) {
    ue_ctxt->logger.log_warning("\"{}\" failed. Cause: Received invalid Retrieve UE Context Response", name());
    CORO_EARLY_RETURN(xnap_retrieve_ue_context_response{});
  }

  // Learn the XNAP UE ID the old NG-RAN node allocated for this UE. The response is sent from the old to the new
  // NG-RAN node, so the old NG-RAN node UE XnAP ID is the peer UE ID.
  response.peer_xnap_ue_id = uint_to_peer_xnap_ue_id(transaction_sink.response()->old_ng_ra_nnode_ue_xn_ap_id);
  ue_ctxt_list.update_peer_xnap_ue_id(ue_ctxt->ue_ids.local_xnap_ue_id, response.peer_xnap_ue_id);

  // TODO: Report the Xn-U tunnel endpoints of the admitted bearers to the old NG-RAN node with an Xn-U Address
  // Indication (TS 38.423 section 8.2.6), so that it can forward the data it still holds for the UE. This needs an
  // Xn-U connection between the two nodes.

  ue_ctxt->logger.log_info("\"{}\" finished successfully", name());

  CORO_RETURN(response);
}

bool xnap_new_node_retrieve_ue_context_procedure::send_retrieve_ue_context_request()
{
  xnap_message msg = {};
  msg.pdu.set_init_msg();
  msg.pdu.init_msg().load_info_obj(ASN1_XNAP_ID_RETRIEVE_UE_CONTEXT);
  retrieve_ue_context_request_s& asn1_request = msg.pdu.init_msg().value.retrieve_ue_context_request();

  // This is sent from the new to the old NG-RAN node, so the new NG-RAN node UE XnAP ID is the local XNAP UE ID.
  asn1_request->new_ng_ra_nnode_ue_xn_ap_id = local_xnap_ue_id_to_uint(ue_ctxt->ue_ids.local_xnap_ue_id);

  retrieve_ue_context_request_to_asn1(asn1_request, request);

  if (!tx_notifier.on_new_message(msg)) {
    ue_ctxt->logger.log_warning("Cannot send Retrieve UE Context Request");
    return false;
  }

  return true;
}
