// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ric_connection_loss_routine.h"

using namespace ocudu;

ric_connection_loss_routine::ric_connection_loss_routine(e2_subscription_manager& subscription_mngr_,
                                                         ocudulog::basic_logger&  logger_) :
  subscription_mngr(subscription_mngr_), logger(logger_)
{
}

void ric_connection_loss_routine::operator()(coro_context<async_task<void>>& ctx)
{
  CORO_BEGIN(ctx);
  logger.info("\"{}\" started.", name());
  subscription_mngr.stop();
  logger.info("\"{}\" finished.", name());
  CORO_RETURN();
}
