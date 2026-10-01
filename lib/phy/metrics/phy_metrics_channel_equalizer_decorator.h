// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/phy/metrics/phy_metrics_notifiers.h"
#include "ocudu/phy/metrics/phy_metrics_reports.h"
#include "ocudu/phy/upper/equalization/channel_equalizer.h"
#include "ocudu/support/resource_usage/scoped_resource_usage.h"
#include <memory>

namespace ocudu {

/// Channel precoder metric decorator.
class phy_metrics_channel_equalizer_decorator : public channel_equalizer
{
public:
  /// Creates a channel equalizer decorator from a base instance and a metric notifier.
  phy_metrics_channel_equalizer_decorator(std::unique_ptr<channel_equalizer> base_equalizer_,
                                          channel_equalizer_metric_notifier& notifier_) :
    base_equalizer(std::move(base_equalizer_)), notifier(notifier_)
  {
    ocudu_assert(base_equalizer, "Invalid channel equalizer.");
  }

  // See interface for documentation.
  void equalize(span<cf_t>                       eq_symbols,
                span<float>                      eq_noise_vars,
                const re_buffer_reader<cbf16_t>& ch_symbols,
                const ch_est_list&               ch_estimates,
                span<const float>                noise_var_estimates,
                float                            tx_scaling) override
  {
    channel_equalizer_metrics metrics;
    {
      // Use scoped resource usage class to measure CPU usage of this block.
      resource_usage_utils::scoped_resource_usage rusage_tracker(metrics.measurements);
      base_equalizer->equalize(eq_symbols, eq_noise_vars, ch_symbols, ch_estimates, noise_var_estimates, tx_scaling);
    }
    metrics.nof_re     = ch_estimates.get_nof_re();
    metrics.nof_layers = ch_estimates.get_nof_tx_layers();
    metrics.nof_ports  = ch_estimates.get_nof_rx_ports();
    notifier.on_new_metric(metrics);
  }

  // See interface for documentation.
  bool is_supported(unsigned nof_ports, unsigned nof_layers) override
  {
    return base_equalizer->is_supported(nof_ports, nof_layers);
  }

private:
  std::unique_ptr<channel_equalizer> base_equalizer;
  channel_equalizer_metric_notifier& notifier;
};

} // namespace ocudu
