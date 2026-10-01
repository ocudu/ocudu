// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/phy/metrics/phy_metrics_notifiers.h"
#include "ocudu/phy/upper/channel_modulation/evm_calculator.h"
#include "ocudu/phy/upper/unique_rx_buffer.h"
#include "ocudu/support/resource_usage/scoped_resource_usage.h"

namespace ocudu {

/// EVM calculator metric decorator.
class phy_metrics_evm_calculator_decorator : public evm_calculator
{
public:
  /// Creates an EVM decorator from a base instance and a metric notifier.
  phy_metrics_evm_calculator_decorator(std::unique_ptr<evm_calculator>            base_,
                                       common_channel_modulation_metric_notifier& notifier_) :
    base(std::move(base_)), notifier(notifier_)
  {
    ocudu_assert(base, "Invalid base instance.");
  }

  // See interface for documentation.
  float
  calculate(span<const log_likelihood_ratio> soft_bits, span<const cf_t> symbols, modulation_scheme modulation) override
  {
    evm_calculator_metrics metrics;
    float                  ret;
    {
      // Use scoped resource usage class to measure CPU usage of this block.
      resource_usage_utils::scoped_resource_usage rusage_tracker(metrics.measurements);
      ret = base->calculate(soft_bits, symbols, modulation);
    }
    metrics.modulation  = modulation;
    metrics.nof_symbols = static_cast<unsigned>(symbols.size());
    notifier.on_new_metric(metrics);

    return ret;
  }

private:
  std::unique_ptr<evm_calculator>            base;
  common_channel_modulation_metric_notifier& notifier;
};

} // namespace ocudu
