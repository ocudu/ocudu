// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "apps/services/metrics/metrics_consumer.h"
#include "apps/services/metrics/metrics_properties.h"
#include "apps/services/metrics/metrics_set.h"
#include "flexible_o_du_metrics.h"
#include "ocudu/adt/span.h"
#include "ocudu/ocudulog/logger.h"
#include "ocudu/support/executors/task_executor.h"
#include "ocudu/support/synchronization/stop_event.h"

namespace ocudu {

/// Flexible O-RAN DU metrics properties implementation.
class flexible_o_du_app_service_metrics_properties_impl : public app_services::metrics_properties
{
public:
  std::string_view name() const override { return "Flexible O-DU metrics"; }
};

/// Application service metrics for the flexible O-DU.
class flexible_o_du_app_service_metrics_impl : public app_services::metrics_set
{
  flexible_o_du_app_service_metrics_properties_impl properties;
  flexible_o_du_metrics                             metrics;

public:
  explicit flexible_o_du_app_service_metrics_impl(const flexible_o_du_metrics& metrics_) : metrics(metrics_) {}

  // See interface for documentation.
  const app_services::metrics_properties& get_properties() const override { return properties; }

  // Returns the flexible O-DU metrics.
  const flexible_o_du_metrics& get_metrics() const { return metrics; }
};

/// Callback for the flexible O-DU metrics.
inline auto flexible_o_du_metrics_callback = [](const app_services::metrics_set&      report,
                                                span<app_services::metrics_consumer*> consumers,
                                                task_executor&                        executor,
                                                ocudulog::basic_logger&               logger,
                                                stop_event_token                      token) {
  const auto& metric = static_cast<const flexible_o_du_app_service_metrics_impl&>(report);

  if (!executor.defer([metric, consumers, stop_token = std::move(token)]() {
        for (auto& consumer : consumers) {
          consumer->handle_metric(metric);
        }
      })) {
    logger.error("Failed to dispatch the metric '{}'", metric.get_properties().name());
  }
};

} // namespace ocudu
