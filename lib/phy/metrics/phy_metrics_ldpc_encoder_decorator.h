// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/phy/metrics/phy_metrics_notifiers.h"
#include "ocudu/phy/metrics/phy_metrics_reports.h"
#include "ocudu/phy/upper/channel_coding/ldpc/ldpc_encoder.h"
#include "ocudu/phy/upper/channel_coding/ldpc/ldpc_encoder_buffer.h"
#include "ocudu/support/resource_usage/scoped_resource_usage.h"
#include <memory>

namespace ocudu {

/// LDPC encoder metric decorator.
class phy_metrics_ldpc_encoder_decorator : public ldpc_encoder
{
public:
  /// Creates an LDPC encoder decorator from a base LDPC encoder instance and a metric notifier.
  phy_metrics_ldpc_encoder_decorator(std::unique_ptr<ldpc_encoder> base_encoder_,
                                     ldpc_encoder_metric_notifier& notifier_) :
    base_encoder(std::move(base_encoder_)), notifier(notifier_)
  {
    ocudu_assert(base_encoder, "Invalid encoder.");
  }

  // See interface for documentation.
  const ldpc_encoder_buffer& encode(const bit_buffer& input, const configuration& cfg) override
  {
    ldpc_encoder_metrics metrics;

    const auto& buffer = [this, &metrics](const bit_buffer&    bit_buf,
                                          const configuration& config) -> const ldpc_encoder_buffer& {
      // Use scoped resource usage class to measure CPU usage of this block.
      resource_usage_utils::scoped_resource_usage rusage_tracker(metrics.measurements);
      return base_encoder->encode(bit_buf, config);
    }(input, cfg);
    metrics.cb_sz = units::bits(input.size());

    notifier.on_new_metric(metrics);
    return buffer;
  }

private:
  std::unique_ptr<ldpc_encoder> base_encoder;
  ldpc_encoder_metric_notifier& notifier;
};

} // namespace ocudu
