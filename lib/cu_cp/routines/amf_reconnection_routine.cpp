// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "amf_reconnection_routine.h"
#include "ocudu/adt/format.h"
#include "ocudu/ngap/ngap_setup.h"
#include "ocudu/support/async/async_timer.h"
#include "ocudu/support/async/coroutine.h"

using namespace ocudu;
using namespace ocucp;

async_task<void> ocudu::ocucp::start_amf_reconnection(ngap_interface&           ngap,
                                                      timer_factory             timers,
                                                      std::chrono::milliseconds reconnection_retry_time)
{
  return launch_async<amf_reconnection_routine>(ngap, timers, reconnection_retry_time);
}

amf_reconnection_routine::amf_reconnection_routine(ngap_interface&           ngap_,
                                                   timer_factory             timers,
                                                   std::chrono::milliseconds reconnection_retry_time_) :
  ngap(ngap_),
  logger(ocudulog::fetch_basic_logger("CU-CP")),
  amf_tnl_connection_retry_timer(timers.create_timer()),
  reconnection_retry_time(reconnection_retry_time_),
  retry_wait(reconnection_retry_time_)
{
}

void amf_reconnection_routine::operator()(coro_context<async_task<void>>& ctx)
{
  CORO_BEGIN(ctx);

  logger.info("\"{}\" started...", name());

  while (true) {
    logger.info("Reconnecting to AMF in {}...", retry_wait);
    CORO_AWAIT(async_wait_for(amf_tnl_connection_retry_timer, retry_wait));
    while (not ngap.handle_amf_tnl_connection_request()) {
      logger.info("TNL connection establishment to AMF failed. Retrying in {}...", reconnection_retry_time);
      CORO_AWAIT(async_wait_for(amf_tnl_connection_retry_timer, reconnection_retry_time));
    }

    // Initiate NG Setup.
    CORO_AWAIT_VALUE(result_msg, ngap.handle_ng_setup_request(/*max_setup_retries*/ 1));
    if (std::holds_alternative<ngap_ng_setup_response>(result_msg)) {
      break;
    }

    // The AMF may accept the CU-CP later. Back off exponentially to spare the AMF, and throttle the warnings so a
    // lasting misconfiguration stays visible without flooding the log.
    ++nof_setup_failures;
    retry_wait =
        reconnection_retry_time *
        (1U << (nof_setup_failures < max_setup_backoff_exponent ? nof_setup_failures : max_setup_backoff_exponent));
    if (nof_setup_failures == 1 or nof_setup_failures % setup_failure_log_period == 0) {
      logger.warning("NG Setup attempt {} failed. Cause: {}. Retrying in {}...",
                     nof_setup_failures,
                     std::get<ngap_ng_setup_failure>(result_msg).cause,
                     retry_wait);
    } else {
      logger.debug("NG Setup attempt {} failed. Cause: {}. Retrying in {}...",
                   nof_setup_failures,
                   std::get<ngap_ng_setup_failure>(result_msg).cause,
                   retry_wait);
    }

    // Tear the N2 TNL association down, so that the NG Setup can be retried over a new one.
    CORO_AWAIT(ngap.handle_amf_disconnection_request());
  }

  if (logger.debug.enabled()) {
    std::string plmn_list;
    for (const auto& plmn : ngap.get_ngap_context().get_supported_plmns()) {
      plmn_list += plmn.to_string() + " ";
    }

    logger.debug("Reconnected to AMF. Supported PLMNs: {}", plmn_list);
  }

  logger.info("\"{}\" finished successfully", name());

  CORO_RETURN();
}
