// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "cu_up_f1u_metrics_consumers.h"
#include "apps/helpers/metrics/json_generators/cu_up/f1u.h"
#include "cu_up_f1u_metrics.h"
#include "ocudu/f1u/cu_up/f1u_metrics.h"

using namespace ocudu;

void cu_up_f1u_metrics_consumer_e2::handle_metric(const app_services::metrics_set& metric)
{
  notifier.report_metrics(static_cast<const cu_up_f1u_metrics_impl&>(metric).get_metrics());
}

cu_up_f1u_metrics_consumer_json::cu_up_f1u_metrics_consumer_json(ocudulog::basic_logger& logger_,
                                                                 ocudulog::log_channel&  log_chan_,
                                                                 task_executor&          executor_,
                                                                 unique_timer            timer_,
                                                                 unsigned                report_period_ms_) :
  report_period_ms(report_period_ms_),
  logger(logger_),
  log_chan(log_chan_),
  executor(executor_),
  timer(std::move(timer_))
{
  ocudu_assert(report_period_ms > 10, "CU-UP report period is too fast to work with current JSON consumer");
  ocudu_assert(timer.is_valid(), "Invalid timer passed to metrics controller");

  // Shift the timer a little.
  timer.set(std::chrono::milliseconds(report_period_ms / 10), [this]() { initialize_timer(); });
  timer.run();
}

void cu_up_f1u_metrics_consumer_json::handle_metric(const app_services::metrics_set& metric)
{
  // Implement aggregation.
  const ocuup::f1u_metrics_container& f1u_metric = static_cast<const cu_up_f1u_metrics_impl&>(metric).get_metrics();

  // Tx aggregation.
  const ocuup::f1u_tx_metrics_container& tx_metric = f1u_metric.tx;
  ocuup::f1u_tx_metrics_container&       aggr_tx   = aggr_metrics.tx;
  // TODO: aggregate TX
  (void)tx_metric;
  (void)aggr_tx;

  // Rx aggregation.
  const ocuup::f1u_rx_metrics_container& rx_metric = f1u_metric.rx;
  ocuup::f1u_rx_metrics_container&       aggr_rx   = aggr_metrics.rx;
  // TODO: aggregate RX
  (void)rx_metric;
  (void)aggr_rx;

  aggr_metrics.metrics_period = f1u_metric.metrics_period;

  aggr_metrics.is_empty = false;
}

void cu_up_f1u_metrics_consumer_json::print_metrics()
{
  if (aggr_metrics.is_empty) {
    return;
  }

  log_chan(
      "{}",
      app_helpers::json_generators::generate_string(aggr_metrics.tx, aggr_metrics.rx, aggr_metrics.metrics_period, 2));

  // Clear metrics after printing.
  clear_metrics();
}

void cu_up_f1u_metrics_consumer_json::initialize_timer()
{
  timer.set(std::chrono::milliseconds(report_period_ms), [this]() {
    if (!executor.execute([this]() {
          print_metrics();
          timer.run();
        })) {
      logger.warning("Failed to enqueue task to print CU-UP metrics");
    }
  });
  timer.run();
}

void cu_up_f1u_metrics_consumer_log::handle_metric(const app_services::metrics_set& metric)
{
  // Implement aggregation.
  const ocuup::f1u_metrics_container& f1u_metric = static_cast<const cu_up_f1u_metrics_impl&>(metric).get_metrics();

  fmt::memory_buffer buffer;
  fmt::format_to(std::back_inserter(buffer), "NRUP Metrics: {}", f1u_metric);
  log_chan("{}", to_c_str(buffer));
}
