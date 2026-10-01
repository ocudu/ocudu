// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "apps/services/buffer_pool/metrics/buffer_pool_metrics_builder.h"
#include "apps/helpers/metrics/metrics_config.h"
#include "apps/helpers/metrics/metrics_helpers.h"
#include "apps/services/buffer_pool/metrics/buffer_pool_metrics_consumer.h"
#include "apps/services/buffer_pool/metrics/buffer_pool_metrics_producer.h"

using namespace ocudu;
using namespace app_services;

metrics_config app_services::build_buffer_pool_metrics_config(metrics_notifier&                  notifier,
                                                              const app_helpers::metrics_config& metrics_cfg,
                                                              remote_server_metrics_gateway*     metrics_gateway)
{
  metrics_config config;

  config.metric_name = buffer_pool_metrics_properties_impl().name();
  config.callback    = buffer_pool_metrics_callback;
  config.producers.emplace_back(std::make_unique<buffer_pool_metrics_producer_impl>(notifier));

  if (metrics_cfg.enable_log_metrics) {
    config.consumers.push_back(
        std::make_unique<buffer_pool_metrics_consumer_log>(app_helpers::fetch_logger_metrics_log_channel()));
  }

  if (metrics_cfg.enable_json_metrics) {
    report_error_if_not(metrics_gateway,
                        "Invalid remote server gateway for sending JSON metrics. Check that remote server is enabled");
    config.consumers.push_back(std::make_unique<buffer_pool_metrics_consumer_json>(*metrics_gateway));
  }

  return config;
}
