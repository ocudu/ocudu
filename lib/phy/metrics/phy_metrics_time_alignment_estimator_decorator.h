// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/phy/metrics/phy_metrics_notifiers.h"
#include "ocudu/phy/support/time_alignment_estimator/time_alignment_estimator.h"
#include "ocudu/phy/upper/unique_rx_buffer.h"
#include "ocudu/support/resource_usage/scoped_resource_usage.h"

namespace ocudu {

/// Time alignment estimator metric decorator.
class phy_metrics_time_alignment_estimator_decorator : public time_alignment_estimator
{
public:
  /// Creates a time alignment estimator decorator from a base instance and a metric notifier.
  phy_metrics_time_alignment_estimator_decorator(std::unique_ptr<time_alignment_estimator> base_,
                                                 time_alignment_estimator_metric_notifier& notifier_) :
    base(std::move(base_)), notifier(notifier_)
  {
    ocudu_assert(base, "Invalid base instance.");
  }

  // See interface for documentation.
  time_alignment_measurement estimate(span<const cf_t>                       symbols,
                                      const bounded_bitset<max_nof_symbols>& mask,
                                      subcarrier_spacing                     scs,
                                      double                                 max_ta) override
  {
    time_alignment_estimator_metrics metrics;
    time_alignment_measurement       ret;
    {
      // Use scoped resource usage class to measure CPU usage of this block.
      resource_usage_utils::scoped_resource_usage rusage_tracker(metrics.measurements);
      ret = base->estimate(symbols, mask, scs, max_ta);
    }
    metrics.nof_re = static_cast<unsigned>(symbols.size());
    notifier.on_new_metric(metrics);

    return ret;
  }

  // See interface for documentation.
  time_alignment_measurement estimate(const re_buffer_reader<cf_t>&          symbols,
                                      const bounded_bitset<max_nof_symbols>& mask,
                                      subcarrier_spacing                     scs,
                                      double                                 max_ta) override
  {
    time_alignment_estimator_metrics metrics;
    time_alignment_measurement       ret;
    {
      // Use scoped resource usage class to measure CPU usage of this block.
      resource_usage_utils::scoped_resource_usage rusage_tracker(metrics.measurements);
      ret = base->estimate(symbols, mask, scs, max_ta);
    }
    metrics.nof_re = static_cast<unsigned>(symbols.get_nof_re());
    notifier.on_new_metric(metrics);

    return ret;
  }

  // See interface for documentation.
  time_alignment_measurement
  estimate(span<const cf_t> symbols, unsigned stride, subcarrier_spacing scs, double max_ta) override
  {
    time_alignment_estimator_metrics metrics;
    time_alignment_measurement       ret;
    {
      // Use scoped resource usage class to measure CPU usage of this block.
      resource_usage_utils::scoped_resource_usage rusage_tracker(metrics.measurements);
      ret = base->estimate(symbols, stride, scs, max_ta);
    }
    metrics.nof_re = static_cast<unsigned>(symbols.size());
    notifier.on_new_metric(metrics);

    return ret;
  }

  // See interface for documentation.
  time_alignment_measurement
  estimate(const re_buffer_reader<cf_t>& symbols, unsigned stride, subcarrier_spacing scs, double max_ta) override
  {
    time_alignment_estimator_metrics metrics;
    time_alignment_measurement       ret;
    {
      // Use scoped resource usage class to measure CPU usage of this block.
      resource_usage_utils::scoped_resource_usage rusage_tracker(metrics.measurements);
      ret = base->estimate(symbols, stride, scs, max_ta);
    }
    metrics.nof_re = static_cast<unsigned>(symbols.get_nof_re());
    notifier.on_new_metric(metrics);

    return ret;
  }

private:
  std::unique_ptr<time_alignment_estimator> base;
  time_alignment_estimator_metric_notifier& notifier;
};

} // namespace ocudu
