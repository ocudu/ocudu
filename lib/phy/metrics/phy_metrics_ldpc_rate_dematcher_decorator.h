// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/phy/metrics/phy_metrics_notifiers.h"
#include "ocudu/phy/metrics/phy_metrics_reports.h"
#include "ocudu/phy/upper/channel_coding/ldpc/ldpc_decoder.h"
#include "ocudu/support/resource_usage/scoped_resource_usage.h"
#include <memory>

namespace ocudu {

/// LDPC rate dematcher metric decorator.
class phy_metrics_ldpc_rate_dematcher_decorator : public ldpc_rate_dematcher
{
public:
  /// Creates an LDPC rate dematcher from a base instance and a metric notifier.
  phy_metrics_ldpc_rate_dematcher_decorator(std::unique_ptr<ldpc_rate_dematcher> base_rate_dematcher_,
                                            ldpc_rate_dematcher_metric_notifier& notifier_) :
    base(std::move(base_rate_dematcher_)), notifier(notifier_)
  {
    ocudu_assert(base, "Invalid decoder.");
  }

  // See interface for documentation.
  void rate_dematch(span<log_likelihood_ratio>       output,
                    span<const log_likelihood_ratio> input,
                    bool                             new_data,
                    const codeblock_metadata&        cfg) override
  {
    ldpc_rate_dematcher_metrics metrics;
    {
      // Use scoped resource usage class to measure CPU usage of this block.
      resource_usage_utils::scoped_resource_usage rusage_tracker(metrics.measurements);
      base->rate_dematch(output, input, new_data, cfg);
    }
    metrics.input_size = units::bits(input.size());
    notifier.on_new_metric(metrics);
  }

private:
  std::unique_ptr<ldpc_rate_dematcher> base;
  ldpc_rate_dematcher_metric_notifier& notifier;
};

} // namespace ocudu
