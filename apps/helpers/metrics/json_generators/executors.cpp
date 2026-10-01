// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "executors.h"
#include "apps/helpers/metrics/helpers.h"
#include "ocudu/support/executors/metrics/executor_metrics.h"

using namespace ocudu;
using namespace app_helpers;
using namespace json_generators;

namespace ocudu {

void to_json(nlohmann::json& json, const executor_metrics& metrics)
{
  json["name"]                  = metrics.name;
  json["nof_executes"]          = metrics.nof_executes;
  json["nof_defers"]            = metrics.nof_defers;
  json["enqueue_avg"]           = metrics.avg_enqueue_latency_us.count();
  json["enqueue_max"]           = metrics.max_enqueue_latency_us.count();
  json["task_avg"]              = metrics.avg_task_us.count();
  json["task_max"]              = metrics.max_task_us.count();
  json["cpu_load"]              = validate_fp_value(metrics.cpu_load);
  json["nof_vol_ctxt_switch"]   = metrics.total_rusg.vol_ctxt_switch_count;
  json["nof_invol_ctxt_switch"] = metrics.total_rusg.invol_ctxt_switch_count;
}

} // namespace ocudu

nlohmann::json ocudu::app_helpers::json_generators::generate(const executor_metrics& metrics)
{
  nlohmann::json json;

  json["timestamp"]        = get_time_stamp();
  json["executor_metrics"] = metrics;

  return json;
}

std::string ocudu::app_helpers::json_generators::generate_string(const executor_metrics& metrics, int indent)
{
  return generate(metrics).dump(indent);
}
