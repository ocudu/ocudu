// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/e2/e2_node_component_config.h"
#include "ocudu/support/async/async_task.h"
#include <vector>

namespace ocudu {

/// Interface exposed to e2_impl: provides an awaitable task and a timeout hook.
/// The aggregator implements this; e2_impl holds a non-owning reference to it.
class e2_node_component_config_provider
{
public:
  virtual ~e2_node_component_config_provider() = default;

  /// \brief Returns an async task that resolves once all expected node-component-config pairs
  ///        have been collected, or on_timeout() is called (whichever comes first).
  virtual async_task<std::vector<e2_node_component_config>> get_configs() = 0;

  /// Called by e2_entity's timeout timer (on the E2 executor) when the deadline fires
  /// before all expected pairs have arrived.  Fires the event with whatever was collected.
  virtual void on_timeout() = 0;
};

} // namespace ocudu
