// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "apps/services/metrics/metrics_consumer.h"
#include "apps/services/metrics/metrics_properties.h"
#include "apps/services/metrics/metrics_set.h"
#include "ocudu/adt/span.h"
#include "ocudu/du/du_high/du_metrics_report.h"
#include "ocudu/ocudulog/logger.h"
#include "ocudu/support/executors/task_executor.h"
#include "ocudu/support/synchronization/stop_event.h"
#include <utility>

namespace ocudu {

/// DU metrics properties implementation.
class du_metrics_properties_impl : public app_services::metrics_properties
{
public:
  std::string_view name() const override { return "DU metrics"; }
};

/// MAC metrics implementation.
class du_metrics_impl : public app_services::metrics_set
{
  du_metrics_properties_impl properties;
  odu::du_metrics_report     report;

public:
  explicit du_metrics_impl(odu::du_metrics_report report_) : report(std::move(report_)) {}

  // See interface for documentation.
  const app_services::metrics_properties& get_properties() const override { return properties; }

  // Returns the DU metrics report.
  const odu::du_metrics_report& get_report() const { return report; }
};

/// Callback for the DU metrics.
inline auto du_metrics_callback = [](const app_services::metrics_set&      report,
                                     span<app_services::metrics_consumer*> consumers,
                                     task_executor&                        executor,
                                     ocudulog::basic_logger&               logger,
                                     stop_event_token                      token) {
  const auto& metric = static_cast<const du_metrics_impl&>(report);

  if (!executor.defer([metric, consumers, stop_token = std::move(token)]() {
        for (auto& consumer : consumers) {
          consumer->handle_metric(metric);
        }
      })) {
    logger.error("Failed to dispatch the metric '{}'", metric.get_properties().name());
  }
};

} // namespace ocudu
