// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "apps/services/app_resource_usage/metrics/app_resource_usage_metrics.h"
#include "ocudu/ocudulog/log_channel.h"

namespace ocudu {

namespace app_services {
class remote_server_metrics_gateway;
} // namespace app_services

/// Log consumer for the resource usage metrics.
class resource_usage_metrics_consumer_log : public app_services::metrics_consumer
{
public:
  explicit resource_usage_metrics_consumer_log(ocudulog::log_channel& log_chan_) : log_chan(log_chan_) {}

  // See interface for documentation.
  void handle_metric(const app_services::metrics_set& metric) override;

private:
  ocudulog::log_channel& log_chan;
};

/// JSON consumer for the resource usage metrics.
class resource_usage_metrics_consumer_json : public app_services::metrics_consumer
{
public:
  explicit resource_usage_metrics_consumer_json(app_services::remote_server_metrics_gateway& gateway_) :
    gateway(gateway_)
  {
  }

  // See interface for documentation.
  void handle_metric(const app_services::metrics_set& metric) override;

private:
  app_services::remote_server_metrics_gateway& gateway;
};

} // namespace ocudu
