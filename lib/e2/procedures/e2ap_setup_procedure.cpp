// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "e2ap_setup_procedure.h"
#include "../common/e2ap_asn1_utils.h"
#include "ocudu/asn1/e2ap/e2ap.h"
#include "ocudu/support/async/async_event_source.h"

using namespace ocudu;
using namespace asn1::e2ap;

e2ap_setup_procedure::e2ap_setup_procedure(const e2_setup_request_message& request_,
                                           e2_message_notifier&            notif_,
                                           e2_event_manager&               ev_mng_,
                                           ocudulog::basic_logger&         logger_,
                                           async_event_source<bool>&       cancel_event_) :
  request(request_), notifier(notif_), ev_mng(ev_mng_), logger(logger_), cancel_event(cancel_event_)
{
}

void e2ap_setup_procedure::operator()(coro_context<async_task<e2_setup_response_message>>& ctx)
{
  CORO_BEGIN(ctx);

  while (true) {
    transaction_sink.subscribe_to(ev_mng.e2_setup_outcome, std::chrono::seconds{5});

    // Send request to RIC interface.
    send_e2_setup_request();

    // Await RIC response.
    CORO_AWAIT(transaction_sink);

    if (not retry_required()) {
      // No more attempts. Exit loop.
      break;
    }

    // Wait time_to_wait before retrying. Fires early with false if cancel_event is set externally.
    wait_observer.subscribe_to(cancel_event,
                               std::chrono::duration_cast<std::chrono::milliseconds>(time_to_wait),
                               true /* value set when timer fires */);
    CORO_AWAIT_VALUE(timer_expired, wait_observer);
    if (not timer_expired) {
      break;
    }
  }

  // Forward procedure result to DU manager.
  CORO_RETURN(create_e2_setup_result());
}

void e2ap_setup_procedure::send_e2_setup_request()
{
  e2_message msg = {};
  msg.pdu.set_init_msg();
  msg.pdu.init_msg().load_info_obj(ASN1_E2AP_ID_E2SETUP);
  msg.pdu.init_msg().value.e2setup_request() = request.request;
  auto& setup_req                            = msg.pdu.init_msg().value.e2setup_request();
  setup_req->transaction_id                  = transaction_id++;
  notifier.on_new_message(msg);
}

bool e2ap_setup_procedure::retry_required()
{
  if (not transaction_sink.failed()) {
    return false;
  }

  const auto& fail = transaction_sink.failure();
  if (not fail->time_to_wait_present) {
    logger.warning("\"{}\" failed. RIC E2AP cause: \"{}\". RIC did not set a retry waiting time.",
                   name(),
                   get_cause_str(fail->cause));
    fmt::print("\"{}\" failed. RIC E2AP cause: \"{}\"\n", name(), get_cause_str(fail->cause));
    return false;
  }
  if (e2_setup_retry_no++ >= request.max_setup_retries) {
    logger.warning("\"{}\" failed. RIC E2AP cause: \"{}\". Reached maximum number of retries ({}).",
                   name(),
                   get_cause_str(fail->cause),
                   request.max_setup_retries);
    fmt::print("\"{}\" failed. RIC E2AP cause: \"{}\"\n", name(), get_cause_str(fail->cause));
    return false;
  }
  time_to_wait = std::chrono::seconds{fail->time_to_wait.to_number()};
  logger.info("\"{}\" failed. RIC E2AP cause: \"{}\". Reinitiating in {}s ({}/{}).",
              name(),
              get_cause_str(fail->cause),
              time_to_wait.count(),
              e2_setup_retry_no,
              request.max_setup_retries);
  fmt::print("\"{}\" failed. RIC E2AP cause: \"{}\". Reinitiating in {}s ({}/{}).\n",
             name(),
             get_cause_str(fail->cause),
             time_to_wait.count(),
             e2_setup_retry_no,
             request.max_setup_retries);
  return true;
}

e2_setup_response_message e2ap_setup_procedure::create_e2_setup_result()
{
  e2_setup_response_message res{};

  if (transaction_sink.successful()) {
    res.success  = true;
    res.response = transaction_sink.response();
    logger.info("E2 Setup procedure successful.");
  } else {
    res.success = false;
    logger.error("E2 Setup procedure failed.");
  }
  return res;
}
