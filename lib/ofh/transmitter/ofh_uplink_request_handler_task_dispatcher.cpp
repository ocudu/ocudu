// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "ofh_uplink_request_handler_task_dispatcher.h"
#include "ocudu/adt/format.h"
#include "ocudu/phy/support/resource_grid_context.h"
#include "ocudu/phy/support/shared_resource_grid.h"
#include "ocudu/support/rtsan.h"

using namespace ocudu;
using namespace ofh;

void uplink_request_handler_task_dispatcher::start()
{
  stop_manager.reset();
}

void uplink_request_handler_task_dispatcher::stop()
{
  stop_manager.stop();
}

void uplink_request_handler_task_dispatcher::handle_prach_occasion(const prach_buffer_context& context,
                                                                   shared_prach_buffer         buffer)
{
  // Do not process if stop was requested.
  auto token = stop_manager.get_token();
  if (OCUDU_UNLIKELY(token.is_stop_requested())) {
    return;
  }

  if (!executor.defer([context,
                       prach_buff = std::move(buffer),
                       this,
                       tk = std::move(token)]() mutable noexcept OCUDU_RTSAN_NONBLOCKING {
        uplink_handler.handle_prach_occasion(context, std::move(prach_buff));
      })) {
    metrics_collector.increment_dispatch_failures();
    logger.warning(
        "Sector#{}: failed to handle PRACH in the uplink request handler for slot '{}'", sector_id, context.slot);
  }
}

void uplink_request_handler_task_dispatcher::handle_new_uplink_slot(const resource_grid_context& context,
                                                                    const shared_resource_grid&  grid)
{
  // Do not process if stop was requested.
  auto token = stop_manager.get_token();
  if (OCUDU_UNLIKELY(token.is_stop_requested())) {
    return;
  }

  if (!executor.defer([context, rg = grid.copy(), this, tk = std::move(token)]() noexcept OCUDU_RTSAN_NONBLOCKING {
        uplink_handler.handle_new_uplink_slot(context, rg);
      })) {
    metrics_collector.increment_dispatch_failures();
    logger.warning(
        "Sector#{}: failed to handle uplink slot in the uplink request handler for slot '{}'", sector_id, context.slot);
  }
}
