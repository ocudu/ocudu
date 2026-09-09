// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "xn_setup_procedure.h"
#include "../xnap_asn1_utils.h"
#include "xn_setup_asn1_helpers.h"
#include "xn_setup_procedure_asn1_helpers.h"
#include "ocudu/support/async/async_timer.h"

using namespace ocudu;
using namespace ocudu::ocucp;
using namespace asn1::xnap;

xn_setup_procedure::xn_setup_procedure(
    const xnap_configuration&                                                                    xnap_cfg_,
    span<const cu_cp_served_cell_info>                                                           served_cells_,
    std::optional<xnap_context>&                                                                 peer_ctxt_,
    xnap_tx_pdu_notifier_with_logging&                                                           tx_notifier_,
    protocol_transaction_event_source<asn1::xnap::xn_setup_resp_s, asn1::xnap::xn_setup_fail_s>& xn_setup_outcome_,
    timer_factory                                                                                timers_,
    ocudulog::basic_logger&                                                                      logger_) :
  xnap_cfg(xnap_cfg_),
  served_cells(served_cells_),
  peer_ctxt(peer_ctxt_),
  tx_notifier(tx_notifier_),
  xn_setup_outcome(xn_setup_outcome_),
  logger(logger_),
  xn_setup_wait_timer(timers_.create_timer())
{
}

void xn_setup_procedure::operator()(coro_context<async_task<bool>>& ctx)
{
  CORO_BEGIN(ctx);

  logger.info("\"{}\" started...", name());

  // Prepare XN Setup message.
  xn_setup_req = generate_asn1_xn_setup_request(xnap_cfg, served_cells);

  while (true) {
    // Subscribe to respective publisher to receive XN SETUP RESPONSE/FAILURE message.
    transaction_sink.subscribe_to(xn_setup_outcome, xnap_cfg.procedure_timeout);

    // Forward message to Xn-C.
    if (!tx_notifier.on_new_message(xn_setup_req)) {
      logger.warning("Cannot send XNSetupRequest");
      CORO_EARLY_RETURN(false);
    }

    // Await XN Setup response.
    CORO_AWAIT(transaction_sink);

    if (not retry_required()) {
      // No more attempts. Exit loop.
      break;
    }

    // Await timer.
    logger.debug("Reinitiating XN setup in {}s. Cause: Received XNSetupFailure with Time to Wait IE",
                 time_to_wait.count());
    CORO_AWAIT(
        async_wait_for(xn_setup_wait_timer, std::chrono::duration_cast<std::chrono::milliseconds>(time_to_wait)));
  }

  if (!transaction_sink.successful()) {
    logger.warning("\"{}\" failed. Cause: No successful outcome received", name());
    CORO_EARLY_RETURN(false);
  }

  // Handle successful outcome. Validate received message and store peer context information.
  received_xn_setup_resp = transaction_sink.response();

  validation_error = validate_xn_setup_request_response(received_xn_setup_resp);
  if (not validation_error.has_value()) {
    logger.warning("\"{}\" failed.Cause: Received XN Setup Response is invalid", name());
    CORO_EARLY_RETURN(false);
  }

  // Store peer context information.
  peer_ctxt = create_peer_xnap_context(received_xn_setup_resp);

  logger.info("\"{}\" finished successfully", name());

  CORO_RETURN(true);
}

bool xn_setup_procedure::retry_required()
{
  if (transaction_sink.successful()) {
    // Success case.
    return false;
  }

  if (transaction_sink.timeout_expired()) {
    // Timeout case.
    logger.warning("\"{}\" timed out after {}ms", name(), xnap_cfg.procedure_timeout.count());
    return false;
  }

  if (!transaction_sink.failed()) {
    // No response received.
    logger.warning("\"{}\" failed. No response received", name());
    fmt::print("\"{}\" failed. No response received\n", name());
    return false;
  }

  const asn1::xnap::xn_setup_fail_s& xn_fail = transaction_sink.failure();

  if (not xn_fail->time_to_wait_present) {
    // Xn-C peer didn't command a waiting time.
    logger.warning("\"{}\": Stopping procedure. Cause: Xn-C peer did not set any retry waiting time", name());
    logger.warning("\"{}\" failed. Xn-C peer Cause: \"{}\"", name(), asn1_utils::get_cause_str(xn_fail->cause));
    fmt::print("\"{}\" failed. Xn-C peer Cause: \"{}\"\n", name(), asn1_utils::get_cause_str(xn_fail->cause));
    return false;
  }

  time_to_wait = std::chrono::seconds{xn_fail->time_to_wait.to_number()};
  return true;
}
