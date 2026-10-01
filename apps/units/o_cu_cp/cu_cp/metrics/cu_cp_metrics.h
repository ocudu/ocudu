// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "apps/services/metrics/metrics_consumer.h"
#include "apps/services/metrics/metrics_properties.h"
#include "apps/services/metrics/metrics_set.h"
#include "ocudu/adt/span.h"
#include "ocudu/adt/unique_function.h"
#include "ocudu/cu_cp/cu_cp_metrics_notifier.h"
#include "ocudu/support/synchronization/stop_event.h"

namespace ocudu {

/// CU-CP metrics properties implementation.
class cu_cp_metrics_properties_impl : public app_services::metrics_properties
{
public:
  std::string_view name() const override { return "CU-CP metrics"; }
};

/// CU-CP metrics implementation.
class cu_cp_metrics_impl : public app_services::metrics_set
{
  cu_cp_metrics_properties_impl properties;
  cu_cp_metrics_report          metrics;

public:
  explicit cu_cp_metrics_impl(const cu_cp_metrics_report& metrics_) : metrics(metrics_) {}

  // See interface for documentation.
  const app_services::metrics_properties& get_properties() const override { return properties; }

  const cu_cp_metrics_report& get_metrics() const { return metrics; }
};

/// Callback for the CU-CP PDCP metrics.
inline auto cu_cp_metrics_callback = [](const app_services::metrics_set&      report,
                                        span<app_services::metrics_consumer*> consumers,
                                        task_executor&                        executor,
                                        ocudulog::basic_logger&               logger,
                                        stop_event_token                      token) {
  const auto& metric = static_cast<const cu_cp_metrics_impl&>(report);

  if (!executor.defer([metric, consumers, stop_token = std::move(token)]() {
        for (auto& consumer : consumers) {
          consumer->handle_metric(metric);
        }
      })) {
    logger.error("Failed to dispatch the metric '{}'", metric.get_properties().name());
  }
};

} // namespace ocudu
