// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/phy/metrics/phy_metrics_notifiers.h"
#include "ocudu/phy/metrics/phy_metrics_reports.h"
#include "ocudu/phy/upper/channel_coding/crc_calculator.h"
#include "ocudu/support/resource_usage/scoped_resource_usage.h"
#include <memory>

namespace ocudu {

/// CRC calculator metric decorator.
class phy_metrics_crc_calculator_decorator : public crc_calculator
{
public:
  /// Creates an CRC calculator decorator from a base instance and a metric notifier.
  phy_metrics_crc_calculator_decorator(std::unique_ptr<crc_calculator> base_calculator_,
                                       crc_calculator_metric_notifier& notifier_) :
    base_calculator(std::move(base_calculator_)), notifier(notifier_)
  {
    ocudu_assert(base_calculator, "Invalid CRC calculator.");
  }

  // See interface for documentation.
  crc_calculator_checksum_t calculate_byte(span<const uint8_t> data) const override
  {
    crc_calculator_metrics    metrics;
    crc_calculator_checksum_t ret;
    {
      // Use scoped resource usage class to measure CPU usage of this block.
      resource_usage_utils::scoped_resource_usage rusage_tracker(metrics.measurements);
      ret = base_calculator->calculate_byte(data);
    }
    // Report metrics.
    metrics.poly     = base_calculator->get_generator_poly();
    metrics.nof_bits = units::bytes(data.size()).to_bits();
    notifier.on_new_metric(metrics);

    return ret;
  }

  // See interface for documentation.
  crc_calculator_checksum_t calculate_bit(span<const uint8_t> data) const override
  {
    crc_calculator_metrics    metrics;
    crc_calculator_checksum_t ret;
    {
      // Use scoped resource usage class to measure CPU usage of this block.
      resource_usage_utils::scoped_resource_usage rusage_tracker(metrics.measurements);
      ret = base_calculator->calculate_bit(data);
    }
    // Report metrics.
    metrics.poly     = base_calculator->get_generator_poly();
    metrics.nof_bits = units::bits(data.size());
    notifier.on_new_metric(metrics);

    return ret;
  }

  // See interface for documentation.
  crc_calculator_checksum_t calculate(const bit_buffer& data) const override
  {
    crc_calculator_metrics    metrics;
    crc_calculator_checksum_t ret;
    {
      // Use scoped resource usage class to measure CPU usage of this block.
      resource_usage_utils::scoped_resource_usage rusage_tracker(metrics.measurements);
      ret = base_calculator->calculate(data);
    }
    // Report metrics.
    metrics.poly     = base_calculator->get_generator_poly();
    metrics.nof_bits = units::bits(data.size());
    notifier.on_new_metric(metrics);

    return ret;
  }

  // See interface for documentation.
  crc_generator_poly get_generator_poly() const override { return base_calculator->get_generator_poly(); }

private:
  std::unique_ptr<crc_calculator> base_calculator;
  crc_calculator_metric_notifier& notifier;
};

} // namespace ocudu
