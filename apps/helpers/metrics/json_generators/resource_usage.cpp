// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "resource_usage.h"
#include "apps/helpers/metrics/helpers.h"
#include "ocudu/support/resource_usage/resource_usage_metrics.h"

using namespace ocudu;
using namespace app_helpers;
using namespace json_generators;

/// Bytes in one Mega Byte.
static constexpr double BYTES_IN_MB = (1 << 20);

namespace ocudu {

void to_json(nlohmann::json& json, const resource_usage_metrics& metrics)
{
  // Convert used memory to Megabytes.
  double mem_total_mb = validate_fp_value(static_cast<double>(metrics.memory_stats.memory_usage.value()) / BYTES_IN_MB);

  json["cpu_usage_percent"]       = validate_fp_value(metrics.cpu_stats.cpu_usage_percentage * 100.0);
  json["mem_total_mb"]            = mem_total_mb;
  json["mem_usage_percent"]       = validate_fp_value(metrics.memory_stats.memory_usage_percentage);
  json["power_consumption_watts"] = validate_fp_value(metrics.power_usage_watts);
}

} // namespace ocudu

nlohmann::json ocudu::app_helpers::json_generators::generate(const resource_usage_metrics& metrics)
{
  nlohmann::json json;

  json["timestamp"]          = get_time_stamp();
  json["app_resource_usage"] = metrics;

  return json;
}

std::string ocudu::app_helpers::json_generators::generate_string(const resource_usage_metrics& metrics, int indent)
{
  return generate(metrics).dump(indent);
}
