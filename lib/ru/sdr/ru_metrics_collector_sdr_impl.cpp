// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ru_metrics_collector_sdr_impl.h"
#include "ru_radio_metrics_collector.h"
#include "ocudu/ru/ru_metrics.h"

using namespace ocudu;

void ru_metrics_collector_sdr_impl::collect_metrics(ru_metrics& metrics)
{
  auto& ru_metrics = metrics.metrics.emplace<ru_sdr_metrics>();

  // Radio metrics.
  radio.collect_metrics(ru_metrics.radio);

  // Lower PHY metrics.
  ru_metrics.cells.resize(sector_metrics_collectors.size());

  unsigned sector_id = 0;
  for (auto& collector : sector_metrics_collectors) {
    ru_sdr_sector_metrics& cell_metric = ru_metrics.cells[sector_id];
    cell_metric.sector_id              = sector_id++;
    collector.collect_metrics(cell_metric);
  }
}
