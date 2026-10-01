// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/phy/generic_functions/transform_precoding/transform_precoder.h"
#include "ocudu/phy/metrics/phy_metrics_notifiers.h"
#include "ocudu/phy/upper/unique_rx_buffer.h"
#include "ocudu/support/resource_usage/scoped_resource_usage.h"

namespace ocudu {

/// Transform precoder metric decorator.
class phy_metrics_transform_precoder_decorator : public transform_precoder
{
public:
  /// Creates a transform precoder decorator from a base instance and a metric notifier.
  phy_metrics_transform_precoder_decorator(std::unique_ptr<transform_precoder> base_,
                                           transform_precoder_metric_notifier& notifier_) :
    base(std::move(base_)), notifier(notifier_)
  {
    ocudu_assert(base, "Invalid base instance.");
  }

  // See interface for documentation.
  void deprecode_ofdm_symbol(span<cf_t> out, span<const cf_t> in) override
  {
    transform_precoder_metrics metrics;
    {
      // Use scoped resource usage class to measure CPU usage of this block.
      resource_usage_utils::scoped_resource_usage rusage_tracker(metrics.measurements);
      base->deprecode_ofdm_symbol(out, in);
    }
    metrics.nof_re = static_cast<unsigned>(out.size());
    notifier.on_new_metric(metrics);
  }

  // See interface for documentation.
  void deprecode_ofdm_symbol_noise(span<float> out, span<const float> in) override
  {
    base->deprecode_ofdm_symbol_noise(out, in);
  }

private:
  std::unique_ptr<transform_precoder> base;
  transform_precoder_metric_notifier& notifier;
};

} // namespace ocudu
