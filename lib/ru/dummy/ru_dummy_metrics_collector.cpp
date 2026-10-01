// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ru_dummy_metrics_collector.h"
#include "ocudu/ru/ru_metrics.h"

using namespace ocudu;

void ru_dummy_metrics_collector::collect_metrics(ru_metrics& metrics)
{
  auto& dummy_metrics = metrics.metrics.emplace<ru_dummy_metrics>();
  dummy_metrics.sectors.resize(sectors.size());
  unsigned i = 0;

  // Collect metrics from each sector.
  for (auto& sector : sectors) {
    ru_dummy_sector_metrics& cell_metrics = dummy_metrics.sectors[i];
    cell_metrics.sector_id                = i++;
    sector->collect_metrics(cell_metrics);
  }
}
