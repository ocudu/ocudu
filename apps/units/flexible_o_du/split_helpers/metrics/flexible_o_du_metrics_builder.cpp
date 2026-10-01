// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "flexible_o_du_metrics_builder.h"
#include "apps/helpers/metrics/metrics_config.h"
#include "apps/helpers/metrics/metrics_helpers.h"
#include "apps/units/flexible_o_du/flexible_o_du_commands.h"
#include "flexible_o_du_metrics_consumers.h"
#include "flexible_o_du_metrics_producer.h"

using namespace ocudu;

flexible_o_du_metrics_notifier* ocudu::build_flexible_o_du_metrics_config(
    std::vector<app_services::metrics_config>& metrics,
    std::vector<std::unique_ptr<app_services::toggle_stdout_metrics_app_command::metrics_subcommand>>&
                                                 metrics_subcommands,
    app_services::metrics_notifier&              notifier,
    app_services::remote_server_metrics_gateway* remote_metrics_gateway,
    const app_helpers::metrics_config&           metrics_cfg,
    const std::vector<pci_t>&                    pci_cell_map,
    std::chrono::nanoseconds                     symbol_duration)
{
  flexible_o_du_metrics_notifier* out_value = nullptr;

  auto& odu_metric = metrics.emplace_back();
  auto  producer   = std::make_unique<o_du_metrics_producer_impl>(notifier);
  out_value        = producer.get();
  odu_metric.producers.push_back(std::move(producer));
  odu_metric.metric_name = flexible_o_du_app_service_metrics_properties_impl().name();
  odu_metric.callback    = flexible_o_du_metrics_callback;

  // Create STDOUT consumer.
  auto metrics_stdout = std::make_unique<ru_metrics_consumer_stdout>(pci_cell_map, symbol_duration);
  metrics_subcommands.push_back(std::make_unique<ru_metrics_subcommand_stdout>(*metrics_stdout));
  odu_metric.consumers.push_back(std::move(metrics_stdout));

  if (metrics_cfg.enable_log_metrics) {
    odu_metric.consumers.push_back(std::make_unique<flexible_o_du_metrics_consumer_log>(
        app_helpers::fetch_logger_metrics_log_channel(), pci_cell_map, metrics_cfg.enable_verbose, symbol_duration));
  }

  if (metrics_cfg.enable_json_metrics) {
    report_error_if_not(remote_metrics_gateway,
                        "Invalid remote server gateway for sending JSON metrics. Check that remote server is enabled");
    odu_metric.consumers.push_back(
        std::make_unique<flexible_o_du_metrics_consumer_json>(*remote_metrics_gateway, pci_cell_map, symbol_duration));
  }

  return out_value;
}
