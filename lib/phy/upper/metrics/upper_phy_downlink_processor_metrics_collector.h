// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "aggregators/downlink_processor_metrics_aggregator.h"
#include "ocudu/phy/metrics/phy_metrics_notifiers.h"
#include "ocudu/phy/upper/upper_phy_metrics.h"

namespace ocudu {

/// Upper PHY downlink processing metrics collector.
class upper_phy_downlink_processor_metrics_collector
{
public:
  /// Collects the metrics from the downlink aggregators and writes them into the given metrics argument.
  void collect_metrics(upper_phy_dl_processor_metrics& metrics)
  {
    metrics.min_latency_us = downlink_processor_aggregator.get_min_latency_us();
    metrics.avg_latency_us = downlink_processor_aggregator.get_avg_latency_us();
    metrics.max_latency_us = downlink_processor_aggregator.get_max_latency_us();
    downlink_processor_aggregator.reset();
  }

  /// Gets the downlink processor metric notifier interface.
  downlink_processor_metric_notifier& get_notifier() { return downlink_processor_aggregator; }

private:
  downlink_processor_metrics_aggregator downlink_processor_aggregator;
};

} // namespace ocudu
