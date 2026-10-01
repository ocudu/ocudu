// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "app_execution_metrics_builder.h"
#include "app_execution_metrics_consumer.h"
#include "app_execution_metrics_producer.h"
#include "apps/helpers/metrics/metrics_config.h"
#include "apps/helpers/metrics/metrics_helpers.h"

using namespace ocudu;

executor_metrics_notifier&
ocudu::build_app_execution_metrics_config(app_services::metrics_config&                exec_metric_cfg,
                                          app_services::metrics_notifier&              notifier,
                                          const app_helpers::metrics_config&           metrics_cfg,
                                          app_services::remote_server_metrics_gateway* metrics_gateway)

{
  auto                       producer  = std::make_unique<app_execution_metrics_producer_impl>(notifier);
  executor_metrics_notifier& out_value = *producer;

  exec_metric_cfg.callback    = producer->get_callback();
  exec_metric_cfg.metric_name = executor_metrics_properties_impl().name();
  exec_metric_cfg.producers.push_back(std::move(producer));

  if (metrics_cfg.enable_log_metrics) {
    exec_metric_cfg.consumers.push_back(
        std::make_unique<app_execution_metrics_consumer_log>(app_helpers::fetch_logger_metrics_log_channel()));
  }

  if (metrics_cfg.enable_json_metrics) {
    report_error_if_not(metrics_gateway,
                        "Invalid remote server gateway for sending JSON metrics. Check that remote server is enabled");
    exec_metric_cfg.consumers.push_back(std::make_unique<app_execution_metrics_consumer_json>(*metrics_gateway));
  }

  return out_value;
}
