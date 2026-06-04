// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ric_connection_removal_routine.h"

using namespace ocudu;

ric_connection_removal_routine::ric_connection_removal_routine(e2_connection_manager&   conn_mng_,
                                                               e2_subscription_manager& subscription_mngr_,
                                                               std::atomic<bool>&       ric_connected_,
                                                               ocudulog::basic_logger&  logger_) :
  conn_mng(conn_mng_), subscription_mngr(subscription_mngr_), ric_connected(ric_connected_), logger(logger_)
{
}

void ric_connection_removal_routine::operator()(coro_context<async_task<void>>& ctx)
{
  CORO_BEGIN(ctx);

  logger.info("\"{}\" started.", name());

  subscription_mngr.stop();

  // TODO: uncomment when RICs support it.
  // Sending E2 Removal Request is skipped: open-source RICs (e.g. OSC, FlexRIC) do not support
  // E2 Node-Initiated Removal and crash upon receiving it.
  // if (ric_connected.load()) {
  //   CORO_AWAIT(conn_mng.handle_e2_node_initiated_removal_request());
  // }

  ric_connected = false;
  CORO_AWAIT(conn_mng.handle_e2_disconnection_request());

  logger.info("\"{}\" finished.", name());

  CORO_RETURN();
}
