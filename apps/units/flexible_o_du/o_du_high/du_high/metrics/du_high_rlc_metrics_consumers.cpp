// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "du_high_rlc_metrics_consumers.h"
#include "apps/helpers/metrics/json_generators/du_high/rlc.h"
#include "apps/helpers/metrics/json_generators/generator_helpers.h"
#include "apps/services/remote_control/remote_server_metrics_gateway.h"
#include "du_high_rlc_metrics.h"
#include "ocudu/support/format/fmt_to_c_str.h"

using namespace ocudu;

void rlc_metrics_consumer_json::handle_metric(const app_services::metrics_set& metric)
{
  const rlc_metrics& drb = static_cast<const rlc_metrics_impl&>(metric).get_metrics();

  gateway.send(app_helpers::json_generators::generate_string(drb, DEFAULT_JSON_INDENT));
}

void rlc_metrics_consumer_log::handle_metric(const app_services::metrics_set& metric)
{
  const rlc_metrics& drb = static_cast<const rlc_metrics_impl&>(metric).get_metrics();

  fmt::memory_buffer buffer;
  fmt::format_to(std::back_inserter(buffer), "RLC Metrics:");
  fmt::format_to(std::back_inserter(buffer), " du={}", static_cast<uint32_t>(drb.du_index));
  fmt::format_to(std::back_inserter(buffer), " ue={}", static_cast<uint32_t>(drb.ue_index));
  fmt::format_to(std::back_inserter(buffer), " rb={}", drb.rb_id);
  fmt::format_to(std::back_inserter(buffer), " TX=[");
  format_rlc_tx_metrics(buffer, drb.metrics_period, drb.tx);
  fmt::format_to(std::back_inserter(buffer), "] RX=[");
  format_rlc_rx_metrics(buffer, drb.metrics_period, drb.rx);
  fmt::format_to(std::back_inserter(buffer), "]");
  log_chan("{}", to_c_str(buffer));
}

void rlc_metrics_consumer_e2::handle_metric(const app_services::metrics_set& metric)
{
  notifier.report_metrics(static_cast<const rlc_metrics_impl&>(metric).get_metrics());
}
