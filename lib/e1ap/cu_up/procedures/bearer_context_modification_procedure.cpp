// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "bearer_context_modification_procedure.h"
#include "common/e1ap_asn1_converters.h"
#include "cu_up/e1ap_cu_up_asn1_helpers.h"
#include "ocudu/asn1/e1ap/common.h"
#include "ocudu/e1ap/common/e1ap_message.h"
#include "ocudu/e1ap/cu_up/e1ap_cu_up_bearer_context_update.h"

using namespace ocudu;
using namespace ocudu::ocuup;

bearer_context_modification_procedure::bearer_context_modification_procedure(
    const e1ap_ue_context&                          ue_ctxt_,
    const asn1::e1ap::bearer_context_mod_request_s& request_,
    e1ap_message_notifier&                          pdu_notifier_,
    e1ap_cu_up_manager_notifier&                    cu_up_notifier_,
    e1ap_cu_up_metrics_collector&                   metrics_) :
  ue_ctxt(ue_ctxt_), request(request_), pdu_notifier(pdu_notifier_), cu_up_notifier(cu_up_notifier_), metrics(metrics_)
{
  prepare_failure_message();
}

bearer_context_modification_procedure::~bearer_context_modification_procedure()
{
  if (bearer_context_mod_response_msg.success) {
    metrics.add_successful_context_modification();
  }
}

void bearer_context_modification_procedure::operator()(coro_context<async_task<void>>& ctx)
{
  CORO_BEGIN(ctx);

  if (not validate_request()) {
    pdu_notifier.on_new_message(e1ap_msg);
    CORO_EARLY_RETURN();
  }

  if (!fill_e1ap_bearer_context_modification_request(bearer_context_mod, request, ue_ctxt.ue_ids.ue_index)) {
    ue_ctxt.logger.log_error(
        "Sending BearerContextModificationFailure. Cause: Invalid BearerContextModificationRequest");
    pdu_notifier.on_new_message(e1ap_msg);
    CORO_EARLY_RETURN();
  }

  // Here, we transfer to the UE control executor to safely delete PDU sessions,
  // change keys, perform PDCP retransmissions, etc.
  CORO_AWAIT_VALUE(bearer_context_mod_response_msg,
                   cu_up_notifier.on_bearer_context_modification_request_received(bearer_context_mod));

  // Could not find UE.
  if (bearer_context_mod_response_msg.ue_index == INVALID_CU_UP_UE_INDEX) {
    ue_ctxt.logger.log_error("Sending BearerContextModificationFailure: Cause: Invalid UE index");
    pdu_notifier.on_new_message(e1ap_msg);
    CORO_EARLY_RETURN();
  }

  // PDU sessions failed to setup.
  if (not bearer_context_mod_response_msg.success) {
    if (bearer_context_mod_response_msg.cause.has_value()) {
      e1ap_msg.pdu.unsuccessful_outcome().value.bearer_context_mod_fail()->cause =
          cause_to_asn1(bearer_context_mod_response_msg.cause.value());
    } else {
      e1ap_msg.pdu.unsuccessful_outcome().value.bearer_context_mod_fail()->cause =
          cause_to_asn1(e1ap_cause_radio_network_t::unspecified);
    }
    ue_ctxt.logger.log_warning("Sending BearerContextModificationFailure");
    pdu_notifier.on_new_message(e1ap_msg);
    CORO_EARLY_RETURN();
  }

  // Bearer modification successful.
  e1ap_msg.pdu.set_successful_outcome();
  e1ap_msg.pdu.successful_outcome().load_info_obj(ASN1_E1AP_ID_BEARER_CONTEXT_MOD);
  e1ap_msg.pdu.successful_outcome().value.bearer_context_mod_resp()->gnb_cu_cp_ue_e1ap_id =
      request->gnb_cu_cp_ue_e1ap_id;
  e1ap_msg.pdu.successful_outcome().value.bearer_context_mod_resp()->gnb_cu_up_ue_e1ap_id =
      request->gnb_cu_up_ue_e1ap_id;
  e1ap_msg.pdu.successful_outcome().value.bearer_context_mod_resp()->sys_bearer_context_mod_resp_present = true;
  fill_asn1_bearer_context_modification_response(
      e1ap_msg.pdu.successful_outcome().value.bearer_context_mod_resp()->sys_bearer_context_mod_resp,
      bearer_context_mod_response_msg);
  ue_ctxt.logger.log_debug("Sending BearerContextModificationResponse");
  pdu_notifier.on_new_message(e1ap_msg);
  CORO_RETURN();
}

void bearer_context_modification_procedure::prepare_failure_message()
{
  e1ap_msg.pdu.set_unsuccessful_outcome();
  e1ap_msg.pdu.unsuccessful_outcome().load_info_obj(ASN1_E1AP_ID_BEARER_CONTEXT_MOD);
  e1ap_msg.pdu.unsuccessful_outcome().value.bearer_context_mod_fail()->gnb_cu_cp_ue_e1ap_id =
      request->gnb_cu_cp_ue_e1ap_id;
  e1ap_msg.pdu.unsuccessful_outcome().value.bearer_context_mod_fail()->gnb_cu_up_ue_e1ap_id =
      request->gnb_cu_up_ue_e1ap_id;
  e1ap_msg.pdu.unsuccessful_outcome().value.bearer_context_mod_fail()->cause.set_protocol();
}

bool bearer_context_modification_procedure::validate_request()
{
  // We only support NG-RAN bearers.
  if (request->sys_bearer_context_mod_request_present &&
      request->sys_bearer_context_mod_request.type() !=
          asn1::e1ap::sys_bearer_context_mod_request_c::types::ng_ran_bearer_context_mod_request) {
    ue_ctxt.logger.log_error("Sending BearerContextModificationFailure. Cause: Not handling E-UTRAN Bearers");
    return false;
  }
  return true;
}
