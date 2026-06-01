// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "amf_reconnection_routine.h"
#include "ocudu/ngap/ngap_setup.h"
#include "ocudu/support/async/async_timer.h"
#include "ocudu/support/async/coroutine.h"

using namespace ocudu;
using namespace ocucp;

async_task<bool> ocudu::ocucp::start_amf_reconnection(ngap_interface&           ngap,
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
  reconnection_retry_time(reconnection_retry_time_)
{
}

void amf_reconnection_routine::operator()(coro_context<async_task<bool>>& ctx)
{
  CORO_BEGIN(ctx);

  logger.info("\"{}\" started...", name());

  logger.info("TNL connection establishment to AMF failed. Retrying in {}...", reconnection_retry_time);
  CORO_AWAIT(async_wait_for(amf_tnl_connection_retry_timer, reconnection_retry_time));
  while (not ngap.handle_amf_tnl_connection_request()) {
    logger.info("TNL connection establishment to AMF failed. Retrying in {}...", reconnection_retry_time);
    CORO_AWAIT(async_wait_for(amf_tnl_connection_retry_timer, reconnection_retry_time));
  }

  // Initiate NG Setup.
  CORO_AWAIT_VALUE(result_msg, ngap.handle_ng_setup_request(/*max_setup_retries*/ 1));

  success = std::holds_alternative<ngap_ng_setup_response>(result_msg);

  if (success) {
    if (logger.debug.enabled()) {
      std::string plmn_list;
      for (const auto& plmn : ngap.get_ngap_context().get_supported_plmns()) {
        plmn_list += plmn.to_string() + " ";
      }

      logger.debug("Reconnected to AMF. Supported PLMNs: {}", plmn_list);
    }

    logger.info("\"{}\" finished successfully", name());

  } else {
    logger.info("\"{}\" failed", name());

    CORO_EARLY_RETURN(false);
  }

  CORO_RETURN(success);
}
