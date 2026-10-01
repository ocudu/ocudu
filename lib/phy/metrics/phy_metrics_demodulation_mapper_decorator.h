// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/phy/metrics/phy_metrics_notifiers.h"
#include "ocudu/phy/upper/channel_modulation/modulation_mapper.h"
#include "ocudu/phy/upper/unique_rx_buffer.h"
#include "ocudu/support/resource_usage/scoped_resource_usage.h"

namespace ocudu {

/// Demodulation mapper metric decorator.
class phy_metrics_demodulation_mapper_decorator : public demodulation_mapper
{
public:
  /// Creates a demodulation mapper decorator from a base instance and a metric notifier.
  phy_metrics_demodulation_mapper_decorator(std::unique_ptr<demodulation_mapper>       base_,
                                            common_channel_modulation_metric_notifier& notifier_) :
    base(std::move(base_)), notifier(notifier_)
  {
    ocudu_assert(base, "Invalid base instance.");
  }

  // See interface for documentation.
  void demodulate_soft(span<log_likelihood_ratio> llrs,
                       span<const cf_t>           symbols,
                       span<const float>          noise_vars,
                       modulation_scheme          mod) override
  {
    demodulation_mapper_metrics metrics;
    {
      // Use scoped resource usage class to measure CPU usage of this block.
      resource_usage_utils::scoped_resource_usage rusage_tracker(metrics.measurements);
      base->demodulate_soft(llrs, symbols, noise_vars, mod);
    }
    metrics.modulation  = mod;
    metrics.nof_symbols = static_cast<unsigned>(symbols.size());
    notifier.on_new_metric(metrics);
  }

private:
  std::unique_ptr<demodulation_mapper>       base;
  common_channel_modulation_metric_notifier& notifier;
};

} // namespace ocudu
