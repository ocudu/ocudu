// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "apps/services/app_resource_usage/app_resource_usage.h"
#include "apps/helpers/metrics/metrics_config.h"
#include "apps/helpers/metrics/metrics_helpers.h"
#include "apps/services/app_resource_usage/metrics/app_resource_usage_metrics_consumer.h"
#include "apps/services/app_resource_usage/metrics/app_resource_usage_metrics_producer.h"

using namespace ocudu;
using namespace app_services;
using namespace resource_usage_utils;

app_resource_usage::app_resource_usage(std::unique_ptr<power_consumption_reader> power_reader_) :
  power_reader(std::move(power_reader_))
{
  ocudu_assert(power_reader, "Power consumption reader must be non-null");

  auto cpu_snapshot = cpu_usage_now(rusage_measurement_type::PROCESS);
  if (!cpu_snapshot) {
    ocudulog::fetch_basic_logger("METRICS").warning(
        "Application resource usage service failed to query current resource usage, errno={}", cpu_snapshot.error());
  } else {
    last_cpu_snapshot = cpu_snapshot.value();
  }
}

resource_usage_metrics app_resource_usage::get_new_metrics()
{
  auto current_cpu_snapshot = cpu_usage_now(rusage_measurement_type::PROCESS);
  if (!current_cpu_snapshot) {
    ocudulog::fetch_basic_logger("METRICS").warning(
        "Application resource usage service failed to query current resource usage, errno={}",
        current_cpu_snapshot.error());
    return {};
  }

  if (!last_cpu_snapshot) {
    last_cpu_snapshot = current_cpu_snapshot.value();
    return {};
  }

  resource_usage_metrics new_metrics;
  // Calculate CPU metrics.
  update_cpu_usage_metric(current_cpu_snapshot.value(), new_metrics);
  // Calculate power consumption in Watts.
  update_power_consumption_metric(new_metrics);

  return new_metrics;
}

void app_resource_usage::update_cpu_usage_metric(const cpu_snapshot&     current_cpu_snapshot,
                                                 resource_usage_metrics& metrics)
{
  resource_usage_utils::measurements measurements;

  measurements.duration = std::chrono::duration_cast<resource_usage_utils::rusage_meas_duration>(
      current_cpu_snapshot.tp - last_cpu_snapshot->tp);
  measurements.user_time   = (current_cpu_snapshot.user_time - last_cpu_snapshot->user_time);
  measurements.system_time = (current_cpu_snapshot.system_time - last_cpu_snapshot->system_time);
  measurements.current_rss = current_cpu_snapshot.current_rss;

  // Save current snapshot.
  last_cpu_snapshot = current_cpu_snapshot;
  // Update metrics.
  metrics = res_usage_measurements_to_metrics(
      measurements, std::chrono::duration_cast<std::chrono::microseconds>(measurements.duration));
}

void app_resource_usage::update_power_consumption_metric(resource_usage_metrics& metrics)
{
  metrics.power_usage_watts = power_reader->read_power_watts().value_or(0.0);
}

app_resource_usage_service
app_services::build_app_resource_usage_service(metrics_notifier&                metrics_notifier,
                                               const app_resource_usage_config& config,
                                               ocudulog::basic_logger&          logger,
                                               remote_server_metrics_gateway*   metrics_gateway)
{
  app_resource_usage_service app_res_usage;
  if (!config.enable_app_usage) {
    return app_res_usage;
  }
  app_res_usage.service = std::make_unique<app_resource_usage>(build_power_consumption_reader(logger));

  metrics_config& app_res_usage_metrics = app_res_usage.metrics.emplace_back();
  app_res_usage_metrics.metric_name     = resource_usage_metrics_properties_impl().name();
  app_res_usage_metrics.callback        = rusage_metrics_callback;
  app_res_usage_metrics.producers.emplace_back(
      std::make_unique<resource_usage_metrics_producer_impl>(metrics_notifier, *app_res_usage.service));

  if (config.metrics_consumers_cfg.enable_log_metrics) {
    app_res_usage_metrics.consumers.push_back(
        std::make_unique<resource_usage_metrics_consumer_log>(app_helpers::fetch_logger_metrics_log_channel()));
  }

  if (config.metrics_consumers_cfg.enable_json_metrics) {
    report_error_if_not(metrics_gateway,
                        "Invalid remote server gateway for sending JSON metrics. Check that remote server is enabled");
    app_res_usage_metrics.consumers.push_back(std::make_unique<resource_usage_metrics_consumer_json>(*metrics_gateway));
  }

  return app_res_usage;
}
