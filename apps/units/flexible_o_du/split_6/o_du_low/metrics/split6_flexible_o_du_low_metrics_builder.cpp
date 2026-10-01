// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "split6_flexible_o_du_low_metrics_builder.h"
#include "apps/helpers/metrics/metrics_config.h"
#include "apps/helpers/metrics/metrics_helpers.h"
#include "split6_flexible_o_du_low_app_service_metrics.h"
#include "split6_flexible_o_du_low_metrics_consumers.h"
#include "split6_flexible_o_du_low_metrics_producer.h"

using namespace ocudu;

split6_flexible_o_du_low_metrics_notifier* ocudu::build_split6_flexible_o_du_low_metrics_config(
    std::vector<app_services::metrics_config>&   metrics,
    app_services::metrics_notifier&              notifier,
    app_services::remote_server_metrics_gateway* remote_metrics_gateway,
    const app_helpers::metrics_config&           metrics_cfg,
    const std::vector<pci_t>&                    pci_cell_map,
    std::chrono::nanoseconds                     symbol_duration)

{
  split6_flexible_o_du_low_metrics_notifier* output = nullptr;
  if (!metrics_cfg.enabled()) {
    return output;
  }

  auto& odu_metric = metrics.emplace_back();
  auto  producer   = std::make_unique<split6_o_du_low_metrics_producer_impl>(notifier);
  output           = producer.get();
  odu_metric.producers.push_back(std::move(producer));
  odu_metric.metric_name = split6_flexible_o_du_low_app_service_metrics_properties_impl().name();
  odu_metric.callback    = split6_flexible_o_du_low_metrics_callback;

  if (metrics_cfg.enable_log_metrics) {
    odu_metric.consumers.push_back(std::make_unique<split6_flexible_o_du_low_metrics_consumer_log>(
        app_helpers::fetch_logger_metrics_log_channel(), pci_cell_map, metrics_cfg.enable_verbose, symbol_duration));
  }

  if (metrics_cfg.enable_json_metrics) {
    odu_metric.consumers.push_back(std::make_unique<split6_flexible_o_du_low_metrics_consumer_json>(
        *remote_metrics_gateway, pci_cell_map, symbol_duration));
  }

  return output;
}
