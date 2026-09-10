// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/phy/metrics/phy_metrics_notifiers.h"
#include "ocudu/phy/upper/signal_processors/channel_estimator/port_channel_estimator.h"

namespace ocudu {

/// Port channel estimator metric decorator.
class phy_metrics_port_channel_estimator_decorator : public port_channel_estimator
{
public:
  /// Creates port channel estimator decorator from a base instance and a metric notifier.
  phy_metrics_port_channel_estimator_decorator(std::unique_ptr<port_channel_estimator> base_,
                                               port_channel_estimator_metric_notifier& notifier_) :
    base(std::move(base_)), notifier(notifier_)
  {
    ocudu_assert(base, "Invalid base instance.");
  }

  // See interface for documentation.
  const port_channel_estimator_results& compute(const resource_grid_reader& grid,
                                                unsigned                    port,
                                                const dmrs_symbol_list&     pilots,
                                                const configuration&        cfg) override
  {
    auto                                  tp_before  = std::chrono::steady_clock::now();
    const port_channel_estimator_results& ch_results = base->compute(grid, port, pilots, cfg);
    auto                                  tp_after   = std::chrono::steady_clock::now();

    notifier.on_new_metric({.elapsed = tp_after - tp_before});
    return ch_results;
  }

private:
  std::unique_ptr<port_channel_estimator> base;
  port_channel_estimator_metric_notifier& notifier;
};

} // namespace ocudu
