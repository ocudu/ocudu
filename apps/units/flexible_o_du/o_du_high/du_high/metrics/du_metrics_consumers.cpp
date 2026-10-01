// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "du_metrics_consumers.h"
#include "apps/helpers/metrics/json_generators/du_high/scheduler.h"
#include "apps/helpers/metrics/json_generators/generator_helpers.h"
#include "apps/helpers/metrics/json_generators/o_du_high.h"
#include "apps/services/remote_control/remote_server_metrics_gateway.h"
#include "du_metrics.h"

using namespace ocudu;

void du_metrics_consumer_stdout::handle_metric(const app_services::metrics_set& metric)
{
  const odu::du_metrics_report& report = static_cast<const du_metrics_impl&>(metric).get_report();

  if (!print_metrics.load(std::memory_order_relaxed)) {
    return;
  }

  if (!report.mac) {
    return;
  }

  sched_consumer.handle_metric(report.mac->sched);
}

void du_metrics_consumer_json::handle_metric(const app_services::metrics_set& metric)
{
  const odu::du_metrics_report& report = static_cast<const du_metrics_impl&>(metric).get_report();

  if (!report.mac) {
    return;
  }

  gateway.send(app_helpers::json_generators::generate_string(report, DEFAULT_JSON_INDENT));
  if (report.mac) {
    gateway.send(app_helpers::json_generators::generate_string(report.mac->sched, DEFAULT_JSON_INDENT));
  }
}

void du_metrics_consumer_log::handle_metric(const app_services::metrics_set& metric)
{
  const odu::du_metrics_report& report = static_cast<const du_metrics_impl&>(metric).get_report();

  proc_consumer.handle_metric(report.du_proc);
  if (report.mac) {
    mac_consumer.handle_metric(report.mac->dl);
    sched_consumer.handle_metric(report.mac->sched);
  }
}

void du_metrics_consumer_e2::handle_metric(const app_services::metrics_set& metric)
{
  const odu::du_metrics_report& report = static_cast<const du_metrics_impl&>(metric).get_report();

  if (!report.mac) {
    return;
  }

  sched_consumer.handle_metric(report.mac->sched);
}
