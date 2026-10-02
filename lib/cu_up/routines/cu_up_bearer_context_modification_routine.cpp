// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "cu_up_bearer_context_modification_routine.h"
#include "../cu_up_manager_helpers.h"

using namespace ocudu;
using namespace ocuup;

cu_up_bearer_context_modification_routine ::cu_up_bearer_context_modification_routine(
    ue_context&                                     ue_ctxt_,
    const e1ap_bearer_context_modification_request& msg_) :
  ue_ctxt(ue_ctxt_), msg(msg_), logger(ocudulog::fetch_basic_logger("CU-UP"))
{
}

void cu_up_bearer_context_modification_routine::operator()(
    coro_context<async_task<e1ap_bearer_context_modification_response>>& ctx)
{
  CORO_BEGIN(ctx);

  ue_ctxt.get_logger().log_debug("Handling BearerContextModificationRequest");

  response.ue_index = ue_ctxt.get_index();
  response.success  = false;

  if (msg.bearer_context_status_change.has_value()) {
    // Handle bearer context suspension request.
    if (*msg.bearer_context_status_change == e1ap_bearer_context_status_change::suspend) {
      if (ue_ctxt.is_suspended()) {
        ue_ctxt.get_logger().log_warning("Suspend requested for bearer context that is already suspended");
        response.cause = cause_protocol_t::msg_not_compatible_with_receiver_state;
        CORO_EARLY_RETURN(response);
      }
      ue_ctxt.get_logger().log_debug("Processing bearer context status suspend indication");
      ue_ctxt.begin_pdcp_buffering();
      ue_ctxt.notify_pdcp_pdu_processing_stopped();
      CORO_AWAIT(ue_ctxt.await_rx_crypto_tasks());
      CORO_AWAIT(ue_ctxt.await_tx_crypto_tasks());
      ue_ctxt.suspend();
      ue_ctxt.restart_pdcp_pdu_processing();
      ue_ctxt.get_logger().log_debug("Processing bearer context status suspend indication finished");
    } else if (*msg.bearer_context_status_change == e1ap_bearer_context_status_change::resume) {
      if (not ue_ctxt.is_suspended()) {
        ue_ctxt.get_logger().log_warning("Resume requested for bearer context that is already active");
        response.cause = cause_protocol_t::msg_not_compatible_with_receiver_state;
        CORO_EARLY_RETURN(response);
      }
      ue_ctxt.get_logger().log_debug("Processing bearer context status resume indication");
      ue_ctxt.resume();
      ue_ctxt.get_logger().log_debug("Processing bearer context status resume indication finished");
    } else {
      // Unhandled Bearer Context Status change.
      ue_ctxt.get_logger().log_warning("Unhandled bearer context status change. Change is neither resume or suspend");
      CORO_EARLY_RETURN(response);
    }
  }

  if (msg.security_info.has_value()) {
    fill_sec_as_config(security_info, msg.security_info.value());

    // Await pending crypto processing to be finished, so that keys can be safely replaced. Any
    // SDUs/PDUs that arrive at the PDCP between now and the end of this function will be buffered.
    // We flush the buffer at the end of the function, so that if there is a re-establishment, the flush is
    // done after any state change and after PDCP retransmissions.
    ue_ctxt.get_logger().log_debug("Bearer Context Modification Request with security info received");
    ue_ctxt.begin_pdcp_buffering();
    ue_ctxt.notify_pdcp_pdu_processing_stopped();
    CORO_AWAIT(ue_ctxt.await_rx_crypto_tasks());
    CORO_AWAIT(ue_ctxt.await_tx_crypto_tasks());
    ue_ctxt.get_logger().log_debug("Bearer Context Modification Request changed security keys");

    // SDU/PDU processing is finished, safely update the keys now.
    ue_ctxt.set_security_config(security_info);
    ue_ctxt.restart_pdcp_pdu_processing();
  }

  if (not msg.ng_ran_bearer_context_mod_request.has_value()) {
    ue_ctxt.get_logger().log_debug("Bearer Context Modification Request does not setup/modify any NR PDU sessions");
    if (msg.security_info.has_value() and not ue_ctxt.is_suspended()) {
      ue_ctxt.end_pdcp_buffering();
    }
    response.success = true;
    CORO_EARLY_RETURN(response);
  }

  // Traverse list of PDU sessions to be setup/modified.
  for (const auto& pdu_session_item : msg.ng_ran_bearer_context_mod_request.value().pdu_session_res_to_setup_mod_list) {
    ue_ctxt.get_logger().log_debug("Setup/Modification of {}", pdu_session_item.pdu_session_id);
    // The BEARER CONTEXT MODIFICATION REQUEST carries no Direct Forwarding Path Availability IE.
    pdu_session_setup_result session_result = ue_ctxt.setup_pdu_session(pdu_session_item, false);
    process_successful_pdu_resource_setup_mod_outcome(response.pdu_session_resource_setup_list, session_result);
    // Update final result.
    response.success &= session_result.success;
  }

  // Traverse list of PDU sessions to be modified.
  for (const auto& pdu_session_item : msg.ng_ran_bearer_context_mod_request.value().pdu_session_res_to_modify_list) {
    ue_ctxt.get_logger().log_debug("Modifying {}", pdu_session_item.pdu_session_id);
    pdu_session_modification_result session_result = ue_ctxt.modify_pdu_session(
        pdu_session_item, msg.new_ul_tnl_info_required.has_value() && msg.new_ul_tnl_info_required.value());
    process_successful_pdu_resource_modification_outcome(response.pdu_session_resource_modified_list,
                                                         response.pdu_session_resource_failed_to_modify_list,
                                                         session_result,
                                                         logger);
    ue_ctxt.get_logger().log_debug("Modification {}", session_result.success ? "successful" : "failed");

    // Update final result.
    response.success &= session_result.success;
  }

  // Traverse list of PDU sessions to be removed.
  for (const auto& pdu_session_item : msg.ng_ran_bearer_context_mod_request.value().pdu_session_res_to_rem_list) {
    ue_ctxt.get_logger().log_info("Removing {}", pdu_session_item);
    ue_ctxt.remove_pdu_session(pdu_session_item);
    // There is no IE to confirm successful removal.
  }

  // End PDCP buffering if it was required due to key changes.
  if (msg.security_info.has_value() and not ue_ctxt.is_suspended()) {
    ue_ctxt.end_pdcp_buffering();
  }

  // Create response.
  response.success = true;
  CORO_RETURN(response);
}
