// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "app_resource_usage_metrics_consumer.h"
#include "apps/helpers/metrics/json_generators/resource_usage.h"
#include "apps/services/remote_control/remote_server_metrics_gateway.h"
#include "ocudu/support/format/fmt_to_c_str.h"

using namespace ocudu;

void resource_usage_metrics_consumer_json::handle_metric(const app_services::metrics_set& metric)
{
  const resource_usage_metrics& sys_metrics = static_cast<const resource_usage_metrics_impl&>(metric).get_metrics();

  gateway.send(app_helpers::json_generators::generate_string(sys_metrics, 2));
}

void resource_usage_metrics_consumer_log::handle_metric(const app_services::metrics_set& metric)
{
  const resource_usage_metrics& sys_metrics = static_cast<const resource_usage_metrics_impl&>(metric).get_metrics();

  /// Bytes in one Mega Byte.
  static constexpr double BYTES_IN_MB = (1 << 20);

  // Convert used memory to Megabytes.
  double mem_total_mb = static_cast<double>(sys_metrics.memory_stats.memory_usage.value()) / BYTES_IN_MB;

  fmt::memory_buffer buffer;
  fmt::format_to(
      std::back_inserter(buffer),
      "App resource usage: cpu_usage={:.2f}%, mem_total={:.2f} MB, mem_usage={:.2f}%, power_consumption={:.2f} Watts",
      sys_metrics.cpu_stats.cpu_usage_percentage * 100.0,
      mem_total_mb,
      sys_metrics.memory_stats.memory_usage_percentage,
      sys_metrics.power_usage_watts);

  // Flush buffer to the logger.
  log_chan("{}", to_c_str(buffer));
}
