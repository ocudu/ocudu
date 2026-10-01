// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "cu_cp_metrics_consumers.h"
#include "apps/helpers/metrics/json_generators/cu_cp/cu_cp_json_helper.h"
#include "apps/helpers/metrics/json_generators/generator_helpers.h"
#include "apps/services/remote_control/remote_server_metrics_gateway.h"
#include "cu_cp_metrics.h"
#include "ocudu/adt/format.h"
#include "ocudu/e2/e2_cu.h"

using namespace ocudu;

void cu_cp_metrics_consumer_json::handle_metric(const app_services::metrics_set& metric)
{
  const cu_cp_metrics_report& cp_metrics = static_cast<const cu_cp_metrics_impl&>(metric).get_metrics();

  // Only log if there is data.
  if (cp_metrics.dus.empty() && cp_metrics.ngaps.empty()) {
    return;
  }

  gateway.send(app_helpers::json_generators::generate_string(cp_metrics, DEFAULT_JSON_INDENT));
}

void cu_cp_metrics_consumer_log::handle_metric(const app_services::metrics_set& metric)
{
  const cu_cp_metrics_report& cp_metrics = static_cast<const cu_cp_metrics_impl&>(metric).get_metrics();

  ngap_consumer.handle_metric(cp_metrics.ngaps, cp_metrics.mobility);
  rrc_consumer.handle_metric(cp_metrics.dus, cp_metrics.mobility);
}

void cu_cp_metrics_consumer_e2::handle_metric(const app_services::metrics_set& metric)
{
  notifier.report_metrics(static_cast<const cu_cp_metrics_impl&>(metric).get_metrics());
}
