// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "upper_phy_metrics_collector_impl.h"

using namespace ocudu;

void upper_phy_metrics_collector_impl::collect_metrics(upper_phy_metrics& metrics)
{
  if (last_timestamp == std::chrono::time_point<std::chrono::steady_clock>()) {
    last_timestamp         = std::chrono::steady_clock::now();
    metrics.metrics_period = {};
    return;
  }

  // Collect LDPC mettrics.
  ldpc_collector.collect_metrics(metrics.ldpc_metrics);

  // Collect PUSCH processing metrics.
  pusch_collector.collect_metrics(metrics.pusch_metrics);

  // Collect PDSCH processing metrics.
  pdsch_collector.collect_metrics(metrics.pdsch_metrics);

  // Collect downlink processing metrics.
  dl_processor_collector.collect_metrics(metrics.dl_processor_metrics);

  auto tp_now            = std::chrono::steady_clock::now();
  metrics.metrics_period = std::chrono::duration_cast<std::chrono::microseconds>(tp_now - last_timestamp);
  last_timestamp         = tp_now;
}
