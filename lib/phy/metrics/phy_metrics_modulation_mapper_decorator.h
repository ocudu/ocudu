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

/// Modulation mapper metric decorator.
class phy_metrics_modulation_mapper_decorator : public modulation_mapper
{
public:
  /// Creates a modulation mapper decorator from a base instance and a metric notifier.
  phy_metrics_modulation_mapper_decorator(std::unique_ptr<modulation_mapper>         base_,
                                          common_channel_modulation_metric_notifier& notifier_) :
    base(std::move(base_)), notifier(notifier_)
  {
    ocudu_assert(base, "Invalid base instance.");
  }

  // See interface for documentation.
  void modulate(span<cf_t> symbols, const bit_buffer& input, modulation_scheme scheme) override
  {
    channel_modulation_metrics metrics;
    {
      // Use scoped resource usage class to measure CPU usage of this block.
      resource_usage_utils::scoped_resource_usage rusage_tracker(metrics.measurements);
      base->modulate(symbols, input, scheme);
    }
    metrics.modulation  = scheme;
    metrics.nof_symbols = static_cast<unsigned>(symbols.size());
    notifier.on_new_metric(metrics);
  }

  // See interface for documentation.
  float modulate(span<ci8_t> symbols, const bit_buffer& input, modulation_scheme scheme) override
  {
    channel_modulation_metrics metrics;
    float                      ret;
    {
      // Use scoped resource usage class to measure CPU usage of this block.
      resource_usage_utils::scoped_resource_usage rusage_tracker(metrics.measurements);
      ret = base->modulate(symbols, input, scheme);
    }
    metrics.modulation  = scheme;
    metrics.nof_symbols = static_cast<unsigned>(symbols.size());
    notifier.on_new_metric(metrics);

    return ret;
  }

private:
  std::unique_ptr<modulation_mapper>         base;
  common_channel_modulation_metric_notifier& notifier;
};

} // namespace ocudu
