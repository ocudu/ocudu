// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "apps/helpers/metrics/helpers.h"
#include "apps/services/buffer_pool/metrics/buffer_pool_metrics.h"
#include "apps/services/metrics/metrics_consumer.h"
#include "apps/services/remote_control/remote_server_metrics_gateway.h"
#include "external/nlohmann/json.hpp"
#include "ocudu/ocudulog/log_channel.h"

namespace ocudu {
namespace app_services {

/// Log consumer for the buffer pool metrics.
class buffer_pool_metrics_consumer_log : public app_services::metrics_consumer
{
public:
  explicit buffer_pool_metrics_consumer_log(ocudulog::log_channel& log_chan_) : log_chan(log_chan_) {}

  // See interface for documentation.
  void handle_metric(const app_services::metrics_set& metric) override
  {
    size_t central_cache_size = static_cast<const buffer_pool_metrics_impl&>(metric).get_central_cache_size();

    log_chan("Buffer pool: central_cache_size={} segments", central_cache_size);
  }

private:
  ocudulog::log_channel& log_chan;
};

/// JSON consumer for the buffer pool metrics.
class buffer_pool_metrics_consumer_json : public app_services::metrics_consumer
{
public:
  explicit buffer_pool_metrics_consumer_json(app_services::remote_server_metrics_gateway& gateway_) : gateway(gateway_)
  {
  }

  // See interface for documentation.
  void handle_metric(const app_services::metrics_set& metric) override
  {
    nlohmann::json json;

    size_t central_cache_size = static_cast<const buffer_pool_metrics_impl&>(metric).get_central_cache_size();

    json["timestamp"]                      = app_helpers::get_time_stamp();
    auto& json_buffer_pool                 = json["buffer_pool"];
    json_buffer_pool["central_cache_size"] = central_cache_size;

    gateway.send(json.dump(-1));
  }

private:
  app_services::remote_server_metrics_gateway& gateway;
};

} // namespace app_services
} // namespace ocudu
