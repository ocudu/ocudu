// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ngap_ue_context_release_procedure.h"
#include "../ngap_asn1_helpers.h"
#include "ocudu/asn1/ngap/common.h"
#include "ocudu/ngap/ngap_message.h"

using namespace ocudu;
using namespace ocudu::ocucp;
using namespace asn1::ngap;

ngap_ue_context_release_procedure::ngap_ue_context_release_procedure(
    const cu_cp_ue_context_release_command&                           command_,
    const ngap_ue_ids&                                                ue_ids_,
    std::unordered_map<cu_cp_ue_index_t, error_indication_request_t>& stored_error_indications_,
    ngap_cu_cp_notifier&                                              cu_cp_notifier_,
    ngap_message_notifier&                                            amf_notifier_,
    ngap_ue_logger&                                                   logger_) :
  command(command_),
  ue_ids(ue_ids_),
  stored_error_indications(stored_error_indications_),
  cu_cp_notifier(cu_cp_notifier_),
  amf_notifier(amf_notifier_),
  logger(logger_)
{
}

void ngap_ue_context_release_procedure::operator()(coro_context<async_task<void>>& ctx)
{
  CORO_BEGIN(ctx);

  logger.log_info("\"{}\" started...", name());

  // Notify DU processor about UE Context Release Command.
  CORO_AWAIT_VALUE(ue_context_release_complete, cu_cp_notifier.on_new_ue_context_release_command(command));

  // Verify response from DU processor.
  if (ue_context_release_complete.ue_index != command.ue_index) {
    logger.log_info("\"{}\" aborted. UE does not exist anymore", name());
    CORO_EARLY_RETURN();
  }

  // Note: From this point the UE is removed and only the stored context can be accessed.

  if (send_ue_context_release_complete()) {
    // Send ErrorIndication if it was stored for this UE
    if (stored_error_indications.find(ue_ids.ue_index) != stored_error_indications.end()) {
      const auto& req = stored_error_indications.at(ue_ids.ue_index);
      send_error_indication(amf_notifier, logger.get_basic_logger(), req.ran_ue_id, req.amf_ue_id, req.cause);
      // Remove stored error indication
      stored_error_indications.erase(ue_ids.ue_index);
    }

    logger.log_info("\"{}\" finished successfully", name());
  } else {
    logger.log_warning("\"{}\" failed", name());
  }
  CORO_RETURN();
}

bool ngap_ue_context_release_procedure::send_ue_context_release_complete()
{
  ngap_message ngap_msg = {};

  ngap_msg.pdu.set_successful_outcome();
  ngap_msg.pdu.successful_outcome().load_info_obj(ASN1_NGAP_ID_UE_CONTEXT_RELEASE);

  auto& asn1_ue_context_release_complete = ngap_msg.pdu.successful_outcome().value.ue_context_release_complete();
  asn1_ue_context_release_complete->amf_ue_ngap_id = amf_ue_id_to_uint(ue_ids.amf_ue_id);
  asn1_ue_context_release_complete->ran_ue_ngap_id = ran_ue_id_to_uint(ue_ids.ran_ue_id);

  fill_asn1_ue_context_release_complete(asn1_ue_context_release_complete, ue_context_release_complete);

  // Forward message to AMF.
  if (!amf_notifier.on_new_message(ngap_msg)) {
    logger.log_warning("AMF notifier is not set. Cannot send UEContextReleaseComplete");
    return false;
  }

  return true;
}
