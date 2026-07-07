// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "cu_up_pdcp_metrics_consumers.h"
#include "apps/helpers/metrics/json_generators/cu_up/pdcp.h"
#include "apps/helpers/metrics/json_generators/generator_helpers.h"
#include "apps/services/remote_control/remote_server_metrics_gateway.h"
#include "cu_up_pdcp_metrics.h"
#include "ocudu/pdcp/pdcp_metrics.h"

using namespace ocudu;

void cu_up_pdcp_metrics_consumer_e2::handle_metric(const app_services::metrics_set& metric)
{
  notifier.report_metrics(static_cast<const cu_up_pdcp_metrics_impl&>(metric).get_metrics());
}

cu_up_pdcp_metrics_consumer_json::cu_up_pdcp_metrics_consumer_json(
    ocudulog::basic_logger&                      logger_,
    app_services::remote_server_metrics_gateway& gateway_,
    task_executor&                               executor_,
    unique_timer                                 timer_,
    unsigned                                     report_period_ms_) :
  report_period_ms(report_period_ms_), logger(logger_), gateway(gateway_), executor(executor_), timer(std::move(timer_))
{
  ocudu_assert(report_period_ms > 10, "CU-UP report period is too fast to work with current JSON consumer");
  ocudu_assert(timer.is_valid(), "Invalid timer passed to metrics controller");

  // Shift the timer a little.
  timer.set(std::chrono::milliseconds(report_period_ms / 10), [this]() { initialize_timer(); });
  timer.run();
}

void cu_up_pdcp_metrics_consumer_json::handle_metric(const app_services::metrics_set& metric)
{
  // Implement aggregation.
  const pdcp_metrics_container& pdcp_metric = static_cast<const cu_up_pdcp_metrics_impl&>(metric).get_metrics();

  // Tx aggregation.
  const pdcp_tx_metrics_container& tx_metric = pdcp_metric.tx;
  pdcp_tx_metrics_container&       aggr_tx   = aggr_metrics.tx;
  aggr_tx.num_sdus += tx_metric.num_sdus;
  aggr_tx.num_sdu_bytes += tx_metric.num_sdu_bytes;
  aggr_tx.num_pdus += tx_metric.num_pdus;
  aggr_tx.num_pdu_bytes += tx_metric.num_pdu_bytes;
  aggr_tx.num_discard_timeouts += tx_metric.num_discard_timeouts;

  aggr_tx.sum_pdu_latency_ns += tx_metric.sum_pdu_latency_ns;
  aggr_tx.sum_crypto_processing_latency_ns += tx_metric.sum_crypto_processing_latency_ns;

  if (tx_metric.min_pdu_latency_ns) {
    aggr_tx.min_pdu_latency_ns =
        std::min(aggr_tx.min_pdu_latency_ns.value_or(UINT32_MAX), tx_metric.min_pdu_latency_ns.value());
  }
  if (tx_metric.max_pdu_latency_ns) {
    aggr_tx.max_pdu_latency_ns = std::max(aggr_tx.max_pdu_latency_ns.value_or(0), tx_metric.max_pdu_latency_ns.value());
  }

  aggr_metrics.tx_cpu_usage += tx_metric.sum_crypto_processing_latency_ns /
                               (static_cast<double>(pdcp_metric.metrics_period.count()) * 1e6) * 100.0;

  // Rx aggregation.
  const pdcp_rx_metrics_container& rx_metric = pdcp_metric.rx;
  pdcp_rx_metrics_container&       aggr_rx   = aggr_metrics.rx;

  aggr_rx.num_pdus += rx_metric.num_pdus;
  aggr_rx.num_pdu_bytes += rx_metric.num_pdu_bytes;
  aggr_rx.num_data_pdus += rx_metric.num_data_pdus;
  aggr_rx.num_data_pdu_bytes += rx_metric.num_data_pdu_bytes;
  aggr_rx.num_dropped_pdus += rx_metric.num_dropped_pdus;
  aggr_rx.num_sdus += rx_metric.num_sdus;
  aggr_rx.num_sdu_bytes += rx_metric.num_sdu_bytes;
  aggr_rx.num_integrity_verified_pdus += rx_metric.num_integrity_verified_pdus;
  aggr_rx.num_integrity_unverified_pdus += rx_metric.num_integrity_unverified_pdus;
  aggr_rx.num_integrity_failed_pdus += rx_metric.num_integrity_failed_pdus;
  aggr_rx.num_t_reordering_timeouts += rx_metric.num_t_reordering_timeouts;
  aggr_rx.reordering_counter += rx_metric.reordering_counter;

  aggr_rx.reordering_delay_us += rx_metric.reordering_delay_us;
  aggr_rx.sum_sdu_latency_ns += rx_metric.sum_sdu_latency_ns;
  aggr_rx.sum_crypto_processing_latency_ns += rx_metric.sum_crypto_processing_latency_ns;

  if (rx_metric.min_sdu_latency_ns) {
    aggr_rx.min_sdu_latency_ns =
        std::min(aggr_rx.min_sdu_latency_ns.value_or(UINT32_MAX), rx_metric.min_sdu_latency_ns.value());
  }
  if (rx_metric.max_sdu_latency_ns) {
    aggr_rx.max_sdu_latency_ns = std::max(aggr_rx.max_sdu_latency_ns.value_or(0), rx_metric.max_sdu_latency_ns.value());
  }

  aggr_metrics.rx_cpu_usage += rx_metric.sum_crypto_processing_latency_ns /
                               (static_cast<double>(pdcp_metric.metrics_period.count()) * 1e6) * 100.0;

  aggr_metrics.metrics_period = pdcp_metric.metrics_period;

  aggr_metrics.is_empty = false;
}

void cu_up_pdcp_metrics_consumer_json::print_metrics()
{
  if (aggr_metrics.is_empty) {
    return;
  }

  gateway.send(app_helpers::json_generators::generate_string(aggr_metrics.tx,
                                                             aggr_metrics.rx,
                                                             aggr_metrics.tx_cpu_usage,
                                                             aggr_metrics.rx_cpu_usage,
                                                             aggr_metrics.metrics_period,
                                                             DEFAULT_JSON_INDENT));

  // Clear metrics after printing.
  clear_metrics();
}

void cu_up_pdcp_metrics_consumer_json::initialize_timer()
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

void cu_up_pdcp_metrics_consumer_log::handle_metric(const app_services::metrics_set& metric)
{
  // Implement aggregation.
  const pdcp_metrics_container& pdcp_metric = static_cast<const cu_up_pdcp_metrics_impl&>(metric).get_metrics();

  fmt::memory_buffer buffer;
  fmt::format_to(std::back_inserter(buffer), "PDCP Metrics: {}", pdcp_metric);
  log_chan("{}", to_c_str(buffer));
}
