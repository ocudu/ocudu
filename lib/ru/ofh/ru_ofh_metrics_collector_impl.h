// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ofh/ofh_sector_metrics_collector.h"
#include "ocudu/ofh/timing/ofh_timing_metrics_collector.h"
#include "ocudu/ru/ru_metrics_collector.h"
#include <vector>

namespace ocudu {

/// Open Fronthaul Radio Unit metrics collector implementation.
class ru_ofh_metrics_collector_impl : public ru_metrics_collector
{
  ofh::timing_metrics_collector*       timing_collector;
  std::vector<ofh::metrics_collector*> sector_metrics_collectors;

public:
  explicit ru_ofh_metrics_collector_impl(ofh::timing_metrics_collector& timing_collector_) :
    timing_collector(&timing_collector_)
  {
  }

  ru_ofh_metrics_collector_impl(ofh::timing_metrics_collector&       timing_collector_,
                                std::vector<ofh::metrics_collector*> sector_metrics_collectors_) :
    timing_collector(&timing_collector_), sector_metrics_collectors(std::move(sector_metrics_collectors_))
  {
  }

  // See interface for documentation.
  void collect_metrics(ru_metrics& metrics) override;

  /// Returns true if the metrics collector is disabled, otherwise true.
  bool disabled() const { return sector_metrics_collectors.empty(); }
};

} // namespace ocudu
