// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "apps/services/metrics/metrics_consumer.h"
#include "consumers/du_procedure_metrics_consumers.h"
#include "consumers/mac_metrics_consumers.h"
#include "consumers/scheduler_metrics_consumers.h"
#include "ocudu/ocudulog/log_channel.h"

namespace ocudu {

namespace app_services {
class remote_server_metrics_gateway;
} // namespace app_services

/// Consumer for the STDOUT DU metrics.
class du_metrics_consumer_stdout : public app_services::metrics_consumer
{
public:
  // See interface for documentation.
  void handle_metric(const app_services::metrics_set& metric) override;

  /// This can be called from another execution context to turn on the actual plotting.
  void enable() { print_metrics.store(true, std::memory_order_relaxed); }

  /// This can be called from another execution context to turn off the actual plotting.
  void disable() { print_metrics.store(false, std::memory_order_relaxed); }
  /// Prints the header in the next metric handle.
  void print_header() { sched_consumer.print_header(); }

private:
  scheduler_cell_metrics_consumer_stdout sched_consumer;
  std::atomic<bool>                      print_metrics = {false};
};

/// Consumer for the json DU metrics.
class du_metrics_consumer_json : public app_services::metrics_consumer
{
public:
  explicit du_metrics_consumer_json(app_services::remote_server_metrics_gateway& gateway_) : gateway(gateway_) {}

  // See interface for documentation.
  void handle_metric(const app_services::metrics_set& metric) override;

private:
  app_services::remote_server_metrics_gateway& gateway;
};

/// Consumer for the logger DU metrics.
class du_metrics_consumer_log : public app_services::metrics_consumer
{
public:
  explicit du_metrics_consumer_log(ocudulog::log_channel& log_chan) :
    mac_consumer(log_chan), sched_consumer(log_chan), proc_consumer(log_chan)
  {
  }

  // See interface for documentation.
  void handle_metric(const app_services::metrics_set& metric) override;

private:
  mac_metrics_consumer_log            mac_consumer;
  scheduler_cell_metrics_consumer_log sched_consumer;
  du_procedure_metrics_consumer_log   proc_consumer;
};

/// Consumer for the E2 DU metrics.
class du_metrics_consumer_e2 : public app_services::metrics_consumer
{
public:
  explicit du_metrics_consumer_e2(scheduler_metrics_notifier& notifier) : sched_consumer(notifier) {}

  // See interface for documentation.
  void handle_metric(const app_services::metrics_set& metric) override;

private:
  scheduler_cell_metrics_consumer_e2 sched_consumer;
};

} // namespace ocudu
