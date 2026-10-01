// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/e2/e2_node_component_config_provider.h"
#include "ocudu/support/async/manual_event.h"
#include "ocudu/support/executors/task_executor.h"
#include <vector>

namespace ocudu {

/// Aggregates node-component-config pairs from one or more protocol setup procedures.
///
/// - Owns the manual_event used internally to signal completion.
/// - Counts expected pairs (across all protocol types); fires the event once all arrive.
/// - If on_timeout() is called first, fires with whatever partial set was collected.
/// - All methods that touch internal state must be called from the E2 executor thread.
class e2_node_component_config_collector : public e2_node_component_config_provider
{
public:
  /// \param e2_exec_        E2 task executor: deliver() dispatches to this before touching state.
  /// \param total_expected_ Total number of req/resp pairs expected (across all protocols).
  e2_node_component_config_collector(task_executor& e2_exec_, size_t total_expected_);

  async_task<std::vector<e2_node_component_config>> get_configs() override;
  void                                              on_timeout() override;

  /// Called by protocol adapters (from any thread); dispatches to e2_exec before storing.
  /// \param iface_type    Protocol interface that completed setup.
  /// \param req           Packed setup-request PDU bytes.
  /// \param resp          Packed setup-response PDU bytes.
  /// \param component_id  Typed protocol-specific identifier; defaults to monostate (not set).
  void deliver(e2_node_component_interface_type iface_type,
               byte_buffer                      req,
               byte_buffer                      resp,
               e2_node_component_id             component_id = {});

private:
  task_executor& e2_exec;
  size_t         expected;

  // Only accessed on e2_exec thread.
  std::vector<e2_node_component_config>               collected;
  manual_event<std::vector<e2_node_component_config>> event;
};

} // namespace ocudu
