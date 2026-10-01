// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "app_resource_usage_config.h"
#include "apps/services/metrics/metrics_config.h"
#include "ocudu/support/resource_usage/power_consumption.h"
#include "ocudu/support/resource_usage/resource_usage_utils.h"

namespace ocudu {

namespace app_helpers {
struct metrics_config;
} // namespace app_helpers

namespace app_services {

class metrics_notifier;
class remote_server_metrics_gateway;

/// This class can be periodically polled for the new app-level resource usage metrics.
class app_resource_usage
{
public:
  explicit app_resource_usage(std::unique_ptr<resource_usage_utils::power_consumption_reader> power_reader_);

  /// Returns new metrics measured from the last time this method has been called.
  resource_usage_metrics get_new_metrics();

private:
  /// Updates power consumption parameters in the given metrics.
  void update_power_consumption_metric(resource_usage_metrics& metrics);

  /// Updates CPU usage parameters in the given metrics.
  void update_cpu_usage_metric(const resource_usage_utils::cpu_snapshot& current_cpu_snapshot,
                               resource_usage_metrics&                   metrics);

  /// The last taken snapshot of CPU usage.
  expected<resource_usage_utils::cpu_snapshot, int> last_cpu_snapshot;
  /// Power consumption reader.
  std::unique_ptr<resource_usage_utils::power_consumption_reader> power_reader;
};

/// Aggregates application resource usage service with the related metrics.
struct app_resource_usage_service {
  std::unique_ptr<app_resource_usage> service;
  std::vector<metrics_config>         metrics;
};

/// Builds application resource usage service and related metrics.
app_resource_usage_service build_app_resource_usage_service(metrics_notifier&                metrics_notifier,
                                                            const app_resource_usage_config& config,
                                                            ocudulog::basic_logger&          logger,
                                                            remote_server_metrics_gateway*   metrics_gateway);

} // namespace app_services
} // namespace ocudu
