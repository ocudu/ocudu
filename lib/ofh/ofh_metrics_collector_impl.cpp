// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "ofh_metrics_collector_impl.h"
#include "ocudu/ofh/ethernet/ethernet_transmitter_metrics_collector.h"
#include "ocudu/ofh/ofh_metrics.h"

using namespace ocudu;
using namespace ofh;

metrics_collector_impl::metrics_collector_impl(receiver_metrics_collector*    rx_metrics_collector_,
                                               transmitter_metrics_collector* tx_metrics_collector_,
                                               unsigned                       sector_id_) :
  sector_id(sector_id_),
  rx_metrics_collector(rx_metrics_collector_),
  tx_metrics_collector(tx_metrics_collector_),
  last_timestamp(std::chrono::steady_clock::now())
{
  is_enabled = (rx_metrics_collector && tx_metrics_collector);
}

void metrics_collector_impl::collect_metrics(sector_metrics& metric)
{
  if (disabled()) {
    return;
  }

  auto tp_now      = std::chrono::steady_clock::now();
  metric.sector_id = sector_id;

  // Collect receiver metrics.
  rx_metrics_collector->collect_metrics(metric.rx_metrics);
  // Collect transmitter metrics.
  tx_metrics_collector->collect_metrics(metric.tx_metrics);

  metric.metrics_period_ms = std::chrono::duration_cast<std::chrono::milliseconds>(tp_now - last_timestamp);
  last_timestamp           = tp_now;
}
