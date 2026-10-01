// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "app_execution_metrics.h"
#include "apps/helpers/metrics/json_generators/executors.h"
#include "apps/services/metrics/metrics_consumer.h"
#include "apps/services/remote_control/remote_server_metrics_gateway.h"
#include "ocudu/ocudulog/log_channel.h"
#include "ocudu/support/format/fmt_to_c_str.h"

namespace ocudu {

/// Log consumer for the application executors metrics.
class app_execution_metrics_consumer_log : public app_services::metrics_consumer
{
public:
  explicit app_execution_metrics_consumer_log(ocudulog::log_channel& log_chan_) : log_chan(log_chan_) {}

  // See interface for documentation.
  void handle_metric(const app_services::metrics_set& metric) override
  {
    static constexpr unsigned str_buffer_size = 256;

    const auto& metrics = static_cast<const executor_metrics_impl&>(metric).get_metrics();
    fmt::basic_memory_buffer<char, str_buffer_size> buffer;

    fmt::format_to(std::back_inserter(buffer),
                   "Executor metrics \"{}\": nof_executes={} nof_defers={} enqueue_avg={}usec enqueue_max={}usec "
                   "task_avg={}usec task_max={}usec cpu_load={:.1f}% {}",
                   metrics.name,
                   metrics.nof_executes,
                   metrics.nof_defers,
                   metrics.avg_enqueue_latency_us.count(),
                   metrics.max_enqueue_latency_us.count(),
                   metrics.avg_task_us.count(),
                   metrics.max_task_us.count(),
                   metrics.cpu_load,
                   metrics.total_rusg);

    // Flush buffer to the logger.
    log_chan("{}", to_c_str(buffer));
  }

private:
  ocudulog::log_channel& log_chan;
};

/// JSON consumer for the application executors metrics.
class app_execution_metrics_consumer_json : public app_services::metrics_consumer
{
public:
  explicit app_execution_metrics_consumer_json(app_services::remote_server_metrics_gateway& gateway_) :
    gateway(gateway_)
  {
  }

  // See interface for documentation.
  void handle_metric(const app_services::metrics_set& metric) override
  {
    const auto& metrics = static_cast<const executor_metrics_impl&>(metric).get_metrics();
    gateway.send(app_helpers::json_generators::generate_string(metrics, 2));
  }

private:
  app_services::remote_server_metrics_gateway& gateway;
};

} // namespace ocudu
