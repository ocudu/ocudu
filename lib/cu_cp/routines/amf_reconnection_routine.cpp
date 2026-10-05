// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "amf_reconnection_routine.h"
#include "ocudu/adt/format.h"
#include "ocudu/ngap/ngap_setup.h"
#include "ocudu/support/async/async_timer.h"
#include "ocudu/support/async/coroutine.h"
#include <algorithm>

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

  logger.info("Reconnecting to AMF in {}...", retry_wait);
  while (true) {
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

    // The AMF may accept the CU-CP later, e.g. once its configuration is fixed. Wait at least the Time to Wait, if the
    // AMF commanded one, see TS 38.413, Section 8.7.1.3.
    retry_wait = std::max<std::chrono::milliseconds>(
        reconnection_retry_time,
        std::get<ngap_ng_setup_failure>(result_msg).time_to_wait.value_or(std::chrono::seconds{0}));
    logger.warning(
        "NG Setup failed. Cause: {}. Retrying in {}...", std::get<ngap_ng_setup_failure>(result_msg).cause, retry_wait);
    if (not setup_failure_reported) {
      // Only the first failure is announced in STDOUT, so that a lasting rejection does not flood it.
      fmt::print("NG Setup failed. Cause: {}. Retrying in the background\n",
                 std::get<ngap_ng_setup_failure>(result_msg).cause);
      setup_failure_reported = true;
    }

    // Tear the N2 TNL association down, so that the NG Setup can be retried over a new one.
    CORO_AWAIT(ngap.handle_amf_disconnection_request());
  }

  {
    std::string plmn_list;
    for (const auto& plmn : ngap.get_ngap_context().get_supported_plmns()) {
      plmn_list += plmn.to_string() + " ";
    }
    logger.debug("Reconnected to AMF. Supported PLMNs: {}", plmn_list);
    fmt::print("Reconnected to AMF. Supported PLMNs: {}\n", plmn_list);
  }

  logger.info("\"{}\" finished successfully", name());

  CORO_RETURN();
}
