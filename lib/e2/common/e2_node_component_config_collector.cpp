// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/e2/e2_node_component_config_collector.h"
#include "ocudu/support/async/coroutine.h"

using namespace ocudu;

e2_node_component_config_collector::e2_node_component_config_collector(task_executor& e2_exec_,
                                                                       size_t         total_expected_) :
  e2_exec(e2_exec_), expected(total_expected_)
{
  ocudu_assert(total_expected_ > 0, "e2_node_component_config_collector: total_expected must be > 0");
}

async_task<std::vector<e2_node_component_config>> e2_node_component_config_collector::get_configs()
{
  return launch_async([this](coro_context<async_task<std::vector<e2_node_component_config>>>& ctx) {
    CORO_BEGIN(ctx);
    CORO_AWAIT(event);
    CORO_RETURN(event.get());
  });
}

void e2_node_component_config_collector::on_timeout()
{
  // Already on e2_exec thread (called from e2_entity's timer callback).
  if (!event.is_set()) {
    event.set(std::move(collected));
  }
}

void e2_node_component_config_collector::deliver(e2_node_component_interface_type iface_type,
                                                 byte_buffer                      req,
                                                 byte_buffer                      resp,
                                                 e2_node_component_id             component_id)
{
  e2_node_component_config cfg{iface_type, std::move(req), std::move(resp), std::move(component_id)};
  // Dispatch to the E2 executor to avoid data races with the setup coroutine and on_timeout().
  if (!e2_exec.execute([this, cfg = std::move(cfg)]() mutable {
        if (event.is_set()) {
          return; // Already fired (timeout beat us, or called twice).
        }
        collected.push_back(std::move(cfg));
        if (collected.size() >= expected) {
          event.set(std::move(collected));
        }
      })) {
    // Executor rejected the task (shutting down). The timeout path will fire the event
    // with whatever was collected at that point.
  }
}
