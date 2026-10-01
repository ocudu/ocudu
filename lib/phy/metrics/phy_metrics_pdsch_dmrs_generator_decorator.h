// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/phy/metrics/phy_metrics_notifiers.h"
#include "ocudu/phy/metrics/phy_metrics_reports.h"
#include "ocudu/phy/upper/signal_processors/pdsch/dmrs_pdsch_processor.h"
#include "ocudu/support/resource_usage/scoped_resource_usage.h"
#include <memory>

namespace ocudu {

/// DM-RS for PDSCH generator metric decorator.
class phy_metrics_dmrs_pdsch_processor_decorator : public dmrs_pdsch_processor
{
public:
  /// Creates a DM-RS for PDSCH generator from a base instance and a metric notifier.
  phy_metrics_dmrs_pdsch_processor_decorator(std::unique_ptr<dmrs_pdsch_processor> base_,
                                             pdsch_dmrs_generator_metric_notifier& notifier_) :
    base(std::move(base_)), notifier(notifier_)
  {
    ocudu_assert(base, "Invalid encoder.");
  }

  // See interface for documentation.
  void map(resource_grid_writer& grid, const config_t& config) override
  {
    pdsch_dmrs_generator_metrics metrics;
    {
      // Use scoped resource usage class to measure CPU usage of this block.
      resource_usage_utils::scoped_resource_usage rusage_tracker(metrics.measurements);
      base->map(grid, config);
    }
    notifier.on_new_metric(metrics);
  }

private:
  std::unique_ptr<dmrs_pdsch_processor> base;
  pdsch_dmrs_generator_metric_notifier& notifier;
};

} // namespace ocudu
