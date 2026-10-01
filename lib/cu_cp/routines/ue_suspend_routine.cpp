// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ue_suspend_routine.h"
#include "ocudu/adt/format.h"
#include "ocudu/ngap/ngap_rrc_inactive_transition.h"
#include "ocudu/ran/cause/ngap_cause.h"
#include "ocudu/support/async/coroutine.h"

using namespace ocudu;
using namespace ocucp;
using namespace asn1::rrc_nr;

ue_suspend_routine::ue_suspend_routine(cu_cp_ue_index_t                       ue_index_,
                                       const rrc_ue_release_context&          release_context_,
                                       e1ap_bearer_context_manager&           e1ap_bearer_ctxt_mng_,
                                       f1ap_ue_context_manager&               f1ap_ue_ctxt_mng_,
                                       cu_cp_ue_context_release_handler&      ue_context_release_handler_,
                                       rrc_du_connection_event_handler&       rrc_du_metrics_handler_,
                                       ngap_control_message_handler&          ng_control_handler_,
                                       cu_cp_ue_context_manipulation_handler& ue_context_handler_,
                                       ocudulog::basic_logger&                logger_) :
  ue_index(ue_index_),
  release_context(release_context_),
  e1ap_bearer_ctxt_mng(e1ap_bearer_ctxt_mng_),
  f1ap_ue_ctxt_mng(f1ap_ue_ctxt_mng_),
  ue_context_release_handler(ue_context_release_handler_),
  rrc_du_metrics_handler(rrc_du_metrics_handler_),
  ng_control_handler(ng_control_handler_),
  ue_context_handler(ue_context_handler_),
  logger(logger_)
{
}

void ue_suspend_routine::operator()(coro_context<async_task<void>>& ctx)
{
  CORO_BEGIN(ctx);

  logger.debug("ue={}: \"{}\" started...", ue_index, name());

  // Prepare BearerContextModificationRequest with bearer context status change.
  bearer_context_modification_request.ue_index                     = ue_index;
  bearer_context_modification_request.bearer_context_status_change = e1ap_bearer_context_status_change::suspend;

  // Call E1AP procedure.
  CORO_AWAIT_VALUE(
      bearer_context_modification_response,
      e1ap_bearer_ctxt_mng.handle_bearer_context_modification_request(bearer_context_modification_request));

  // Handle BearerContextModificationResponse.
  if (!bearer_context_modification_response.success) {
    logger.warning("ue={}: \"{}\" failed to modify bearer at CU-UP", ue_index, name());
    // Request UE release from AMF if Bearer Context Modification failed.
    {
      ue_context_release_command.ue_index = ue_index;
      ue_context_release_command.cause    = ngap_cause_radio_network_t::unspecified;
      CORO_AWAIT(ue_context_release_handler.handle_ue_context_release_command(ue_context_release_command));
      CORO_EARLY_RETURN();
    }
  }

  // Release UE at DU.
  {
    // Prepare UE Context Release Command and call F1.
    du_ue_context_release_command.ue_index = ue_index;
    du_ue_context_release_command.cause    = f1ap_cause_radio_network_t::normal_release;
    du_ue_context_release_command.rrc_pdu  = release_context.rrc_pdu.copy();
    du_ue_context_release_command.srb_id   = release_context.srb_id;

    CORO_AWAIT_VALUE(released_ue_index,
                     f1ap_ue_ctxt_mng.handle_ue_context_release_command(du_ue_context_release_command));

    if (released_ue_index != ue_index) {
      logger.warning("ue={}: \"{}\" failed to release UE context at DU", ue_index, name());
      // Request UE release from AMF if F1AP UE release failed.
      {
        ue_context_release_command.ue_index = ue_index;
        ue_context_release_command.cause    = ngap_cause_radio_network_t::unspecified;
        CORO_AWAIT(ue_context_release_handler.handle_ue_context_release_command(ue_context_release_command));
        CORO_EARLY_RETURN();
      }
    }

    // Notify DU RRC about RRC Inactive transition.
    rrc_du_metrics_handler.handle_rrc_inactive();
  }

  // Start RNA update timer after successful transition to RRC Inactive state.
  {
    ue_context_handler.initialize_rna_update_timer(ue_index);
  }

  // Inform AMF about RRC inactive transition report. If no RRC Inactive Transition Report is requested, the NGAP will
  // return true.
  {
    rrc_inactive_transition_report.ue_index           = ue_index;
    rrc_inactive_transition_report.rrc_state          = ngap_rrc_inactive_transition_report::ngap_rrc_state::inactive;
    rrc_inactive_transition_report.user_location_info = release_context.user_location_info;
    CORO_AWAIT_VALUE(ngap_rrc_inactive_report_sent,
                     ng_control_handler.handle_rrc_inactive_transition_report_required(rrc_inactive_transition_report));
    if (!ngap_rrc_inactive_report_sent) {
      logger.warning("ue={}: \"{}\" failed to send RRCInactiveTransitionReport", ue_index, name());
      // Request UE release from AMF if transmission of RRC Inactive Transition Report failed.
      {
        ue_context_release_command.ue_index = ue_index;
        ue_context_release_command.cause    = ngap_cause_radio_network_t::unspecified;
        CORO_AWAIT(ue_context_release_handler.handle_ue_context_release_command(ue_context_release_command));
        CORO_EARLY_RETURN();
      }
    }
  }

  logger.debug("ue={}: \"{}\" finished successfully", ue_index, name());
  CORO_RETURN();
}
