// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/phy/generic_functions/precoding/channel_precoder.h"
#include "ocudu/phy/metrics/phy_metrics_notifiers.h"
#include "ocudu/phy/metrics/phy_metrics_reports.h"
#include "ocudu/support/resource_usage/scoped_resource_usage.h"
#include <memory>

namespace ocudu {

/// Channel precoder metric decorator.
class phy_metrics_channel_precoder_decorator : public channel_precoder
{
public:
  /// Creates a channel precoder decorator from a base instance and a metric notifier.
  phy_metrics_channel_precoder_decorator(std::unique_ptr<channel_precoder> base_precoder_,
                                         channel_precoder_metric_notifier& notifier_) :
    base_precoder(std::move(base_precoder_)), notifier(notifier_)
  {
    ocudu_assert(base_precoder, "Invalid channel precoder.");
  }

  // See interface for documentation.
  void apply_precoding(re_buffer_writer<cbf16_t>&     output,
                       const re_buffer_reader<>&      input,
                       const precoding_weight_matrix& precoding) const override
  {
    channel_precoder_metrics metrics;
    {
      // Use scoped resource usage class to measure CPU usage of this block.
      resource_usage_utils::scoped_resource_usage rusage_tracker(metrics.measurements);
      // Call base precoder.
      base_precoder->apply_precoding(output, input, precoding);
    }
    metrics.method     = channel_precoder_metrics::methods::apply_precoding;
    metrics.nof_re     = output.get_nof_re();
    metrics.nof_layers = precoding.get_nof_layers();
    notifier.on_new_metric(metrics);
  }

  // See interface for documentation.
  void apply_layer_map_and_precoding(re_buffer_writer<cbf16_t>&     output,
                                     span<const ci8_t>              input,
                                     const precoding_weight_matrix& precoding) const override
  {
    channel_precoder_metrics metrics;
    {
      // Use scoped resource usage class to measure CPU usage of this block.
      resource_usage_utils::scoped_resource_usage rusage_tracker(metrics.measurements);
      base_precoder->apply_layer_map_and_precoding(output, input, precoding);
    }
    // Report metrics.
    metrics.method     = channel_precoder_metrics::methods::apply_layer_map_and_precoding;
    metrics.nof_re     = output.get_nof_re();
    metrics.nof_layers = precoding.get_nof_layers();
    metrics.nof_ports  = precoding.get_nof_ports();
    notifier.on_new_metric(metrics);
  }

private:
  std::unique_ptr<channel_precoder> base_precoder;
  channel_precoder_metric_notifier& notifier;
};

} // namespace ocudu
