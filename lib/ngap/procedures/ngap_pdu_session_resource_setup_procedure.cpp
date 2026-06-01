// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ngap_pdu_session_resource_setup_procedure.h"
#include "../ngap_asn1_helpers.h"
#include "ngap_error_indication_helper.h"
#include "ocudu/asn1/ngap/common.h"
#include "ocudu/ngap/ngap.h"
#include "ocudu/ngap/ngap_message.h"

using namespace ocudu;
using namespace ocudu::ocucp;
using namespace asn1::ngap;

ngap_pdu_session_resource_setup_procedure::ngap_pdu_session_resource_setup_procedure(
    const ngap_pdu_session_resource_setup_request&     request_,
    const asn1::ngap::pdu_session_res_setup_request_s& asn1_request_,
    ngap_ue_context_list&                              ue_ctxt_list_,
    ngap_cu_cp_notifier&                               cu_cp_notifier_,
    ngap_metrics_aggregator&                           metrics_handler_,
    ngap_message_notifier&                             amf_notifier_) :
  request(request_),
  asn1_request(asn1_request_),
  ue_ctxt_list(ue_ctxt_list_),
  cu_cp_notifier(cu_cp_notifier_),
  metrics_handler(metrics_handler_),
  amf_notifier(amf_notifier_)
{
  // Map PDU session ID to S-NSSAI for metrics.
  for (const auto& pdu_session : request.pdu_session_res_setup_items) {
    pdu_session_id_to_snssai[pdu_session.pdu_session_id] = pdu_session.s_nssai;
  }
}

void ngap_pdu_session_resource_setup_procedure::operator()(coro_context<async_task<void>>& ctx)
{
  CORO_BEGIN(ctx);

  ue_ctxt = ue_ctxt_list.find(uint_to_ran_ue_id(asn1_request->ran_ue_ngap_id));
  if (ue_ctxt == nullptr) {
    ocudulog::fetch_basic_logger("NGAP").warning(
        "ue={} ran_ue={} amf_ue={}: UE context not found. Dropping PDUSessionResourceSetupRequest",
        request.ue_index,
        asn1_request->ran_ue_ngap_id,
        asn1_request->amf_ue_ngap_id);
    CORO_EARLY_RETURN();
  }

  ue_ctxt->logger.log_debug("\"{}\" started...", name());

  if (!ue_ctxt->get_cu_cp_ue()->is_security_enabled()) {
    ue_ctxt->logger.log_warning("Dropping PDUSessionResourceSetupRequest. Security context is not activated");
    send_error_indication(
        amf_notifier, ue_ctxt->logger.get_basic_logger(), ue_ctxt->ue_ids.ran_ue_id, ue_ctxt->ue_ids.amf_ue_id, {});
    CORO_EARLY_RETURN();
  }

  // Verify PDU Session Resource Setup Request.
  verification_outcome = verify_pdu_session_resource_setup_request(request, asn1_request, ue_ctxt->logger);

  if (verification_outcome.request.pdu_session_res_setup_items.empty()) {
    ue_ctxt->logger.log_info("Validation of PDUSessionResourceSetupRequest failed");
    response = verification_outcome.response;
  } else {
    // Add NAS PDU to PDU Session Resource Setup Request.
    if (asn1_request->nas_pdu_present) {
      verification_outcome.request.nas_pdu = asn1_request->nas_pdu.copy();
    }

    // Handle mandatory IEs.
    CORO_AWAIT_VALUE(response, cu_cp_notifier.on_new_pdu_session_resource_setup_request(verification_outcome.request));

    // Combine validation response with DU processor response.
    combine_pdu_session_resource_setup_response();
  }

  if (!response.pdu_session_res_failed_to_setup_items.empty()) {
    ue_ctxt->logger.log_warning("Some or all PDUSessionResourceSetupItems failed to setup");
  }

  if (send_pdu_session_resource_setup_response()) {
    ue_ctxt->logger.log_debug("\"{}\" finished successfully", name());
  } else {
    ue_ctxt->logger.log_debug("\"{}\" failed", name());
  }

  CORO_RETURN();
}

void ngap_pdu_session_resource_setup_procedure::combine_pdu_session_resource_setup_response()
{
  for (const auto& setup_item : verification_outcome.response.pdu_session_res_setup_response_items) {
    response.pdu_session_res_setup_response_items.emplace(setup_item.pdu_session_id, setup_item);
  }
  for (const auto& failed_item : verification_outcome.response.pdu_session_res_failed_to_setup_items) {
    response.pdu_session_res_failed_to_setup_items.emplace(failed_item.pdu_session_id, failed_item);
  }
  if (!response.crit_diagnostics.has_value()) {
    response.crit_diagnostics = verification_outcome.response.crit_diagnostics;
  }
}

bool ngap_pdu_session_resource_setup_procedure::send_pdu_session_resource_setup_response()
{
  ngap_message ngap_msg = {};

  ngap_msg.pdu.set_successful_outcome();
  ngap_msg.pdu.successful_outcome().load_info_obj(ASN1_NGAP_ID_PDU_SESSION_RES_SETUP);

  fill_asn1_pdu_session_res_setup_response(ngap_msg.pdu.successful_outcome().value.pdu_session_res_setup_resp(),
                                           response);

  auto& pdu_session_res_setup_resp           = ngap_msg.pdu.successful_outcome().value.pdu_session_res_setup_resp();
  pdu_session_res_setup_resp->amf_ue_ngap_id = amf_ue_id_to_uint(ue_ctxt->ue_ids.amf_ue_id);
  pdu_session_res_setup_resp->ran_ue_ngap_id = ran_ue_id_to_uint(ue_ctxt->ue_ids.ran_ue_id);

  // Notify metrics handler about successful PDU sessions.
  for (const auto& pdu_session : response.pdu_session_res_setup_response_items) {
    metrics_handler.aggregate_successful_pdu_session_setup(pdu_session_id_to_snssai.at(pdu_session.pdu_session_id));
  }
  // Notify metrics handler about failed PDU sessions.
  for (const auto& pdu_session : response.pdu_session_res_failed_to_setup_items) {
    metrics_handler.aggregate_failed_pdu_session_setup(pdu_session_id_to_snssai.at(pdu_session.pdu_session_id),
                                                       pdu_session.unsuccessful_transfer.cause);
  }

  // Forward message to AMF.
  if (!amf_notifier.on_new_message(ngap_msg)) {
    ue_ctxt->logger.log_warning("AMF notifier is not set. Cannot send PDUSessionResourceSetupResponse");
    return false;
  }

  return true;
}
