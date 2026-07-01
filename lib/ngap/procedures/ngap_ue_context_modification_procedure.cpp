// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ngap_ue_context_modification_procedure.h"
#include "ngap_asn1_converters.h"
#include "ocudu/asn1/ngap/common.h"
#include "ocudu/ngap/ngap_message.h"
#include "ocudu/support/async/coroutine.h"

using namespace ocudu;
using namespace ocudu::ocucp;
using namespace asn1::ngap;

ngap_ue_context_modification_procedure::ngap_ue_context_modification_procedure(
    const ngap_ue_context_modification_request& request_,
    const ngap_ue_ids&                          ue_ids_,
    ngap_cu_cp_notifier&                        cu_cp_notifier_,
    ngap_message_notifier&                      amf_notifier_,
    ngap_ue_logger&                             logger_) :
  request(request_), ue_ids(ue_ids_), cu_cp_notifier(cu_cp_notifier_), amf_notifier(amf_notifier_), logger(logger_)
{
}

void ngap_ue_context_modification_procedure::operator()(coro_context<async_task<void>>& ctx)
{
  CORO_BEGIN(ctx);

  logger.log_info("\"{}\" started...", name());

  CORO_AWAIT_VALUE(ue_ctxt_modification_routine_outcome,
                   cu_cp_notifier.on_new_ue_context_modification_request(request));

  if (not ue_ctxt_modification_routine_outcome.has_value()) {
    send_ue_context_modification_failure(
        ue_ctxt_modification_routine_outcome.error(), ue_ids.amf_ue_id, ue_ids.ran_ue_id);
    logger.log_warning("\"{}\" failed", name());
  } else {
    send_ue_context_modification_response(
        ue_ctxt_modification_routine_outcome.value(), ue_ids.amf_ue_id, ue_ids.ran_ue_id);
    logger.log_info("\"{}\" finished successfully", name());
  }

  CORO_RETURN();
}

void ngap_ue_context_modification_procedure::send_ue_context_modification_response(
    const ngap_ue_context_modification_response& msg,
    const amf_ue_id_t&                           amf_ue_id,
    const ran_ue_id_t&                           ran_ue_id)
{
  ngap_message ngap_msg = {};

  ngap_msg.pdu.set_successful_outcome();
  ngap_msg.pdu.successful_outcome().load_info_obj(ASN1_NGAP_ID_UE_CONTEXT_MOD);
  auto& ue_ctxt_mod_resp           = ngap_msg.pdu.successful_outcome().value.ue_context_mod_resp();
  ue_ctxt_mod_resp->amf_ue_ngap_id = amf_ue_id_to_uint(amf_ue_id);
  ue_ctxt_mod_resp->ran_ue_ngap_id = ran_ue_id_to_uint(ran_ue_id);

  // Fill RRC state.
  if (msg.rrc_state.has_value()) {
    ue_ctxt_mod_resp->rrc_state_present = true;
    ue_ctxt_mod_resp->rrc_state         = static_cast<asn1::ngap::rrc_state_opts::options>(msg.rrc_state.value());
  }

  // Fill user location info.
  if (msg.user_location_info.has_value()) {
    ue_ctxt_mod_resp->user_location_info_present = true;
    ue_ctxt_mod_resp->user_location_info.set_user_location_info_nr() =
        cu_cp_user_location_info_to_asn1(msg.user_location_info.value());
  }

  // Forward message to AMF.
  if (!amf_notifier.on_new_message(ngap_msg)) {
    logger.log_warning("AMF notifier is not set. Cannot send UEContextModificationResponse");
    return;
  }
}

void ngap_ue_context_modification_procedure::send_ue_context_modification_failure(
    const ngap_ue_context_modification_failure& msg,
    const amf_ue_id_t&                          amf_ue_id,
    const ran_ue_id_t&                          ran_ue_id)
{
  ngap_message ngap_msg = {};

  ngap_msg.pdu.set_unsuccessful_outcome();
  ngap_msg.pdu.unsuccessful_outcome().load_info_obj(ASN1_NGAP_ID_UE_CONTEXT_MOD);
  auto& ue_ctxt_mod_fail           = ngap_msg.pdu.unsuccessful_outcome().value.ue_context_mod_fail();
  ue_ctxt_mod_fail->amf_ue_ngap_id = amf_ue_id_to_uint(amf_ue_id);
  ue_ctxt_mod_fail->ran_ue_ngap_id = ran_ue_id_to_uint(ran_ue_id);

  ue_ctxt_mod_fail->cause = cause_to_asn1(msg.cause);

  // Forward message to AMF.
  if (!amf_notifier.on_new_message(ngap_msg)) {
    logger.log_warning("AMF notifier is not set. Cannot send UEContextModificationFailure");
    return;
  }
}
