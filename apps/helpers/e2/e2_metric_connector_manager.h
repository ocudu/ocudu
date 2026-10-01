// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/support/ocudu_assert.h"
#include <cassert>
#include <memory>
#include <vector>

namespace ocudu {

/// Manages the E2 metric connectors of the app.
template <typename ConnectorType, typename NotifierType, typename InterfaceType>
class e2_metric_connector_manager
{
public:
  e2_metric_connector_manager(unsigned nof_cells = 1)
  {
    for (unsigned i = 0, e = nof_cells; i != e; ++i) {
      e2_metric_connectors.push_back(std::make_shared<ConnectorType>());
    }
  }

  ~e2_metric_connector_manager() = default;

  e2_metric_connector_manager(e2_metric_connector_manager&& other) noexcept :
    e2_metric_connectors(std::move(other.e2_metric_connectors))
  {
  }

  NotifierType& get_e2_metric_notifier(unsigned index)
  {
    ocudu_assert(index < e2_metric_connectors.size(), "Invalid index");
    return *(e2_metric_connectors[index]);
  }
  InterfaceType& get_e2_metrics_interface(unsigned index)
  {
    ocudu_assert(index < e2_metric_connectors.size(), "Invalid index");
    return *(e2_metric_connectors[index]);
  }

private:
  std::vector<std::shared_ptr<ConnectorType>> e2_metric_connectors;
};

} // namespace ocudu
