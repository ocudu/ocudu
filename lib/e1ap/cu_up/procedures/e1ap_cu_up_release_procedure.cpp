// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "e1ap_cu_up_release_procedure.h"
#include "common/e1ap_common_messages.h"
#include "ocudu/asn1/e1ap/common.h"
#include "ocudu/asn1/e1ap/e1ap.h"
#include "ocudu/asn1/e1ap/e1ap_ies.h"
#include "ocudu/asn1/e1ap/e1ap_pdu_contents.h"

using namespace ocudu;
using namespace ocuup;
using namespace asn1::e1ap;

e1ap_cu_up_release_procedure::e1ap_cu_up_release_procedure(e1ap_cu_up_connection_handler& cu_up_conn_handler_,
                                                           e1ap_message_notifier&         tx_pdu_notifier_,
                                                           e1ap_event_manager&            ev_mng_,
                                                           e1ap_logger&                   logger_) :
  cu_up_conn_handler(cu_up_conn_handler_), cu_notifier(tx_pdu_notifier_), ev_mng(ev_mng_), logger(logger_)
{
}

void e1ap_cu_up_release_procedure::operator()(coro_context<async_task<void>>& ctx)
{
  CORO_BEGIN(ctx);

  logger.log_debug("\"{}\" started...", name());

  // Check if the TNL is still up.
  if (not cu_up_conn_handler.is_connected()) {
    CORO_EARLY_RETURN();
  }

  // Send the E1AP release request and await response.
  transaction = ev_mng.transactions.create_transaction(std::chrono::milliseconds{1000});
  if (not transaction.valid()) {
    // Just shutdown the TNL association and finish procedure.
    logger.log_error("{}: Unable to allocate transaction. Shutting down E1 TNL association...", name());
    CORO_AWAIT(cu_up_conn_handler.handle_tnl_association_removal());
    logger.log_info("{}: E1 TNL association shut down", name());
    CORO_EARLY_RETURN();
  }

  // Send request to CU-CP.
  send_e1ap_release_request();

  // Await CU-CP response.
  CORO_AWAIT(transaction);

  handle_e1ap_release_response();

  // Tear down TNL association and await it.
  logger.log_debug("{}: Shutting down E1 TNL association...", name());
  CORO_AWAIT(cu_up_conn_handler.handle_tnl_association_removal());
  logger.log_info("{}: E1 TNL association shut down", name());

  CORO_RETURN();
}

void e1ap_cu_up_release_procedure::send_e1ap_release_request()
{
  e1ap_message msg;
  msg.pdu.set_init_msg().load_info_obj(ASN1_E1AP_ID_E1_RELEASE);
  asn1::e1ap::e1_release_request_s& release_request = msg.pdu.init_msg().value.e1_release_request();

  release_request->transaction_id            = transaction.id();
  release_request->cause.set_radio_network() = asn1::e1ap::cause_radio_network_opts::normal_release;

  // Send unpacked message to E1-C GW.
  cu_notifier.on_new_message(msg);
}

void e1ap_cu_up_release_procedure::handle_e1ap_release_response()
{
  if (transaction.aborted()) {
    // Timeout or cancellation case.
    logger.log_warning(
        "{}: Forcing shutdown of E1 TNL association. Cause: Timeout reached for reception of the E1 Release Response",
        name());
  } else {
    const asn1::e1ap::successful_outcome_s& success = transaction.response().value();
    if (success.value.type().value != e1ap_elem_procs_o::successful_outcome_c::types_opts::e1_release_resp) {
      logger.log_warning("{}: Received unexpected E1AP PDU type \"{}\"", name(), success.value.type().to_string());

      cause_c cause;
      cause.set_protocol().value = cause_protocol_opts::msg_not_compatible_with_receiver_state;
      cu_notifier.on_new_message(generate_error_indication(transaction.id(), {}, {}, cause));
      return;
    }
    if (success.value.e1_release_resp()->transaction_id != transaction.id()) {
      logger.log_warning("{}: Received E1 Release Response with wrong transaction ID", name());
      return;
    }
    logger.log_debug("\"{}\" finished successfully", name());
  }
}
