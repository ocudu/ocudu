// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ng_setup_procedure.h"
#include "../ngap_asn1_validators.h"
#include "ocudu/asn1/asn1_dispatch.h"
#include "ocudu/asn1/ngap/common.h"
#include "ocudu/ngap/ngap_setup.h"
#include "ocudu/support/async/async_timer.h"

using namespace ocudu;
using namespace ocudu::ocucp;
using namespace asn1::ngap;

ng_setup_procedure::ng_setup_procedure(ngap_context_t&           context_,
                                       const ngap_message&       request_,
                                       const unsigned            max_setup_retries_,
                                       ngap_message_notifier&    amf_notif_,
                                       ngap_transaction_manager& ev_mng_,
                                       timer_factory             timers,
                                       ocudulog::basic_logger&   logger_) :
  context(context_),
  request(request_),
  max_setup_retries(max_setup_retries_),
  amf_notifier(amf_notif_),
  ev_mng(ev_mng_),
  logger(logger_),
  ng_setup_wait_timer(timers.create_timer())
{
}

void ng_setup_procedure::operator()(coro_context<async_task<ngap_ng_setup_result>>& ctx)
{
  CORO_BEGIN(ctx);

  logger.info("\"{}\" started...", name());

  while (true) {
    // Subscribe to respective publisher to receive NG SETUP RESPONSE/FAILURE message.
    transaction_sink.subscribe_to(ev_mng.ng_setup_outcome, context.procedure_timeout);

    // Forward message to AMF.
    if (!amf_notifier.on_new_message(request)) {
      logger.warning("Cannot send NGSetupRequest");
      CORO_EARLY_RETURN(ngap_ng_setup_failure{ngap_cause_misc_t::unspecified});
    }

    // Await AMF response.
    CORO_AWAIT(transaction_sink);

    if (not retry_required()) {
      // No more attempts. Exit loop.
      break;
    }

    // Await timer.
    logger.debug("Reinitiating NG setup in {}ms (retry={}/{}). Received NGSetupFailure with Time to Wait IE",
                 time_to_wait.count(),
                 ng_setup_retry_no,
                 max_setup_retries);
    CORO_AWAIT(
        async_wait_for(ng_setup_wait_timer, std::chrono::duration_cast<std::chrono::milliseconds>(time_to_wait)));
  }

  // Forward procedure result to DU manager.
  CORO_RETURN(create_ng_setup_result());
}

bool ng_setup_procedure::retry_required()
{
  if (transaction_sink.successful()) {
    // Success case.
    return false;
  }

  if (transaction_sink.timeout_expired()) {
    // Timeout case.
    logger.warning("\"{}\" timed out after {}ms", name(), context.procedure_timeout.count());
    return false;
  }

  if (!transaction_sink.failed()) {
    // No response received.
    logger.warning("\"{}\" failed. No response received", name());
    return false;
  }

  const asn1::ngap::ng_setup_fail_s& ng_fail = transaction_sink.failure();

  // No point in retrying when the failure is due to misconfiguration.
  if (is_failure_misconfiguration(ng_fail->cause)) {
    logger.warning("\"{}\": Stopping procedure. Cause: misconfiguration between gNB and AMF", name());
    logger.warning("\"{}\" failed. AMF NGAP cause: \"{}\"", name(), asn1_utils::get_cause_str(ng_fail->cause));
    return false;
  }

  if (not ng_fail->time_to_wait_present) {
    // AMF didn't command a waiting time.
    logger.warning("\"{}\": Stopping procedure. Cause: AMF did not set any retry waiting time", name());
    logger.warning("\"{}\" failed. AMF NGAP cause: \"{}\"", name(), asn1_utils::get_cause_str(ng_fail->cause));
    return false;
  }
  if (ng_setup_retry_no++ >= max_setup_retries) {
    // Number of retries exceeded, or there is no time to wait.
    logger.warning("\"{}\": Stopping procedure. Cause: Reached maximum number of NG Setup connection retries ({})",
                   name(),
                   max_setup_retries);
    logger.warning("\"{}\" failed. AMF NGAP cause: \"{}\"", name(), asn1_utils::get_cause_str(ng_fail->cause));
    return false;
  }

  time_to_wait = std::chrono::seconds{ng_fail->time_to_wait.to_number()};
  return true;
}

ngap_ng_setup_result ng_setup_procedure::create_ng_setup_result()
{
  if (!transaction_sink.successful()) {
    if (transaction_sink.failed()) {
      return create_ngap_ng_setup_failure(transaction_sink.failure());
    }
    return ngap_ng_setup_failure{ngap_cause_misc_t::unspecified};
  }

  // Validate response content.
  auto msgerr = validate_ng_setup_response(transaction_sink.response());
  if (not msgerr.has_value()) {
    logger.warning("\"{}\" failed. Cause: {}", name(), msgerr.error().second);
    return ngap_ng_setup_failure{ngap_cause_misc_t::unspecified};
  }

  ngap_ng_setup_response response = create_ngap_ng_setup_response(transaction_sink.response());

  context.served_guami_list     = response.served_guami_list;
  context.relative_amf_capacity = response.relative_amf_capacity;
  context.plmn_support_list     = response.plmn_support_list;
  context.amf_name              = response.amf_name;
  context.unavailable_guami_list.clear();
  context.overload.reset();

  // Capture packed request bytes.
  {
    byte_buffer   packed{byte_buffer::fallback_allocation_tag{}};
    asn1::bit_ref bref(packed);
    if (asn1::pack(bref, request.pdu) == asn1::OCUDUASN_SUCCESS) {
      response.packed_ng_setup_request = std::move(packed);
    } else {
      logger.debug("\"{}\": failed to pack NGSetupRequest", name());
    }
  }

  // Capture packed response bytes.
  {
    ngap_pdu_c resp_pdu;
    resp_pdu.set_successful_outcome().load_info_obj(ASN1_NGAP_ID_NG_SETUP);
    resp_pdu.successful_outcome().value.ng_setup_resp() = transaction_sink.response();
    byte_buffer   packed{byte_buffer::fallback_allocation_tag{}};
    asn1::bit_ref bref(packed);
    if (asn1::pack(bref, resp_pdu) == asn1::OCUDUASN_SUCCESS) {
      response.packed_ng_setup_response = std::move(packed);
    } else {
      logger.debug("\"{}\": failed to pack NGSetupResponse", name());
    }
  }

  logger.info("\"{}\" finished successfully", name());

  return response;
}

bool ng_setup_procedure::is_failure_misconfiguration(const cause_c& cause)
{
  switch (cause.type()) {
    case cause_c::types_opts::radio_network:
      return false;
    case cause_c::types_opts::transport:
      return false;
    case cause_c::types_opts::nas:
      return false;
    case cause_c::types_opts::protocol:
      return false;
    case cause_c::types_opts::misc:
      return cause.misc() == asn1::ngap::cause_misc_opts::unknown_plmn_or_sn_pn;
    default:
      break;
  }
  return false;
}

ngap_ng_setup_response
ng_setup_procedure::create_ngap_ng_setup_response(const asn1::ngap::ng_setup_resp_s& asn1_response)
{
  ngap_ng_setup_response response;

  // Fill AMF name
  response.amf_name = asn1_response->amf_name.to_string();

  // Fill served GUAMI list.
  response.served_guami_list = asn1_to_served_guami_list(asn1_response->served_guami_list);

  // Fill relative AMF capacity.
  response.relative_amf_capacity = asn1_response->relative_amf_capacity;

  // Fill PLMN support list.
  response.plmn_support_list = asn1_to_plmn_support_list(asn1_response->plmn_support_list);

  // TODO: Add missing optional values.

  return response;
}

/// \brief Creates the common type \c ngap_ng_setup_failure struct.
/// \param[in] asn1_fail The ASN.1 type NGSetupFailure.
/// \return The common type \c ngap_ng_setup_failure struct.
ngap_ng_setup_failure ng_setup_procedure::create_ngap_ng_setup_failure(const asn1::ngap::ng_setup_fail_s& asn1_fail)
{
  ngap_ng_setup_failure fail;
  fail.cause = asn1_to_cause(asn1_fail->cause);
  if (asn1_fail->time_to_wait_present) {
    fail.time_to_wait = std::chrono::seconds{asn1_fail->time_to_wait.to_number()};
  }

  return fail;
}
