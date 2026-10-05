// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "radio_baseband_sector_metrics_collector.h"
#include "ocudu/adt/span.h"
#include "ocudu/ru/ru_metrics_collector.h"
#include <vector>

namespace ocudu {

class ru_radio_metrics_collector;

/// Metrics collector implementation for the SDR RU.
class ru_metrics_collector_sdr_impl : public ru_metrics_collector
{
  ru_radio_metrics_collector&                          radio;
  std::vector<radio_baseband_sector_metrics_collector> sector_metrics_collectors;

public:
  explicit ru_metrics_collector_sdr_impl(ru_radio_metrics_collector& radio_, unsigned nof_sectors) :
    radio(radio_), sector_metrics_collectors(nof_sectors)
  {
  }

  /// Sets the list of baseband metrics per sector collectors.
  span<radio_baseband_sector_metrics_collector> get_baseband_metrics_collector() { return sector_metrics_collectors; }

  // See interface for documentation.
  void collect_metrics(ru_metrics& metrics) override;
};

} // namespace ocudu
