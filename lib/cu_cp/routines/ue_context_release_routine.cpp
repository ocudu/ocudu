// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ue_context_release_routine.h"
#include "ocudu/ran/cause/ngap_cause_converters.h"

using namespace ocudu;
using namespace ocudu::ocucp;
using namespace asn1::rrc_nr;

ue_context_release_routine::ue_context_release_routine(const cu_cp_ue_context_release_command& command_,
                                                       e1ap_bearer_context_manager*            e1ap_bearer_ctxt_mng_,
                                                       du_processor&                           du_proc_,
                                                       cu_cp_ue_removal_handler&               ue_removal_handler_,
                                                       ue_manager&                             ue_mng_,
                                                       ocudulog::basic_logger&                 logger_) :
  command(command_),
  e1ap_bearer_ctxt_mng(e1ap_bearer_ctxt_mng_),
  du_proc(du_proc_),
  ue_removal_handler(ue_removal_handler_),
  ue_mng(ue_mng_),
  logger(logger_)
{
  ocudu_assert(!command.cause.valueless_by_exception(), "Release command needs to be set.");
}

void ue_context_release_routine::operator()(coro_context<async_task<cu_cp_ue_context_release_complete>>& ctx)
{
  CORO_BEGIN(ctx);

  logger.debug("ue={}: \"{}\" started...", command.ue_index, name());

  // Redirect info is stored in the UE context before the NGAP round-trip (NGAP carries no redirectedCarrierInfo).
  if (cu_cp_ue* ue = ue_mng.find_du_ue(command.ue_index);
      ue != nullptr && ue->get_ue_context().pending_redirect_nr_info.has_value()) {
    command.requires_rrc_message = true;
    command.redirect_nr_info     = ue->get_ue_context().pending_redirect_nr_info;
  }

  // Per TS 23.502, the gNB shall not send RRC Release when the NG release cause is de-registration.
  if (const auto* nas_cause = std::get_if<cause_nas_t>(&command.cause);
      nas_cause != nullptr && *nas_cause == cause_nas_t::deregister) {
    command.requires_rrc_message = false;
  }

  // Prepare context release complete
  {
    release_complete.ue_index = command.ue_index;
    release_complete.pdu_session_res_list_cxt_rel_cpl =
        ue_mng.find_du_ue(command.ue_index)->get_up_resource_manager().get_pdu_sessions();

    // If the RRC UE was not created, we need to prepare the release context with the RRCReject PDU and SRB ID.
    if (ue_mng.find_du_ue(command.ue_index)->get_rrc_ue() != nullptr) {
      // Call RRC UE notifier to get the release context of the UE and add the location info to the UE context release
      // complete message
      release_context =
          ue_mng.find_du_ue(command.ue_index)
              ->get_rrc_ue()
              ->get_rrc_ue_release_context(
                  command.requires_rrc_message, command.release_wait_time, std::nullopt, command.redirect_nr_info);
      release_complete.user_location_info = release_context.user_location_info;
    } else {
      release_context.rrc_pdu = du_proc.get_rrc_du_handler().get_rrc_reject();
      // RRCReject is always sent on SRB0.
      release_context.srb_id = srb_id_t::srb0;
    }
  }

  if (e1ap_bearer_ctxt_mng != nullptr and
      not ue_mng.find_du_ue(command.ue_index)->get_up_resource_manager().get_pdu_sessions().empty()) {
    // If there is an active E1AP context,
    // prepare Bearer Context Release Command and call E1AP notifier
    bearer_context_release_command.ue_index = command.ue_index;
    bearer_context_release_command.cause    = ngap_to_e1ap_cause(command.cause);

    CORO_AWAIT(e1ap_bearer_ctxt_mng->handle_bearer_context_release_command(bearer_context_release_command));
  }

  {
    // Prepare F1AP UE Context Release Command and call F1AP notifier.
    f1ap_ue_context_release_cmd.ue_index = command.ue_index;
    f1ap_ue_context_release_cmd.cause    = ngap_to_f1ap_cause(command.cause);
    if (command.requires_rrc_message) {
      f1ap_ue_context_release_cmd.rrc_pdu = release_context.rrc_pdu.copy();
      f1ap_ue_context_release_cmd.srb_id  = release_context.srb_id;
    }

    CORO_AWAIT_VALUE(f1ap_ue_context_release_result,
                     du_proc.get_f1ap_handler().handle_ue_context_release_command(f1ap_ue_context_release_cmd));
  }

  {
    // Remove UE
    CORO_AWAIT(ue_removal_handler.handle_ue_removal_request(command.ue_index));
    // Note: From this point the UE is removed and only the stored context can be accessed.
  }

  logger.debug("ue={}: \"{}\" finished successfully.", command.ue_index, name());

  CORO_RETURN(release_complete);
}
