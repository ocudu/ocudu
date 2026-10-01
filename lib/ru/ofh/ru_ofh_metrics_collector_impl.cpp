// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ru_ofh_metrics_collector_impl.h"
#include "ocudu/ru/ru_metrics.h"

using namespace ocudu;

void ru_ofh_metrics_collector_impl::collect_metrics(ru_metrics& metrics)
{
  auto& ofh_metrics = metrics.metrics.emplace<ofh::metrics>();

  // Add timestamp to the metric.
  ofh_metrics.timestamp = std::chrono::system_clock::now();

  // Timing metrics.
  timing_collector->collect_metrics(ofh_metrics.timing);

  // Resize with the number of sectors.
  ofh_metrics.sectors.resize(sector_metrics_collectors.size());

  // Collect the metrics from each sector.
  for (unsigned i = 0, e = sector_metrics_collectors.size(); i != e; ++i) {
    sector_metrics_collectors[i]->collect_metrics(ofh_metrics.sectors[i]);
  }
}
