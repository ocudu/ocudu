// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/phy/metrics/phy_metrics_notifiers.h"
#include "ocudu/phy/upper/channel_processors/pusch/pusch_codeword_buffer.h"
#include "ocudu/phy/upper/channel_processors/pusch/pusch_demodulator.h"
#include "ocudu/phy/upper/channel_processors/pusch/pusch_demodulator_notifier.h"
#include "ocudu/support/resource_usage/scoped_resource_usage.h"

namespace ocudu {

/// PUSCH demodulator metric decorator.
class phy_metrics_pusch_demodulator_decorator : public pusch_demodulator, private pusch_codeword_buffer
{
public:
  /// Creates a PUSCH demodulator decorator from a base instance and a notifier.
  phy_metrics_pusch_demodulator_decorator(std::unique_ptr<pusch_demodulator> base_,
                                          pusch_demodulator_metric_notifier& notifier_) :
    base(std::move(base_)), notifier(notifier_)
  {
    ocudu_assert(base, "Invalid PUSCH demodulator.");
  }

  void demodulate(pusch_codeword_buffer&              codeword_buffer,
                  pusch_demodulator_notifier&         demodulator_notifier,
                  const resource_grid_reader&         grid,
                  const dmrs_pusch_estimator_results& est_results,
                  const configuration&                config) override
  {
    base_buffer             = &codeword_buffer;
    elapsed_on_new_block    = {};
    elapsed_on_end_codeword = {};

    pusch_demodulator_metrics metrics;
    {
      // Use scoped resource usage class to measure CPU usage of this block.
      resource_usage_utils::scoped_resource_usage rusage_tracker(metrics.measurements);
      // Prepare base and save the base buffer.
      base->demodulate(*this, demodulator_notifier, grid, est_results, config);
    }
    metrics.elapsed_buffer = elapsed_on_new_block + elapsed_on_end_codeword;

    // Notify metrics.
    notifier.on_new_metric(metrics);
  }

private:
  // See pusch_codeword_buffer interface for documentation.
  span<log_likelihood_ratio> get_next_block_view(unsigned block_size) override
  {
    ocudu_assert(base_buffer != nullptr, "Invalid base buffer.");

    // Assume this method takes a small amount of time.
    return base_buffer->get_next_block_view(block_size);
  }

  // See pusch_codeword_buffer interface for documentation.
  void on_new_block(span<const log_likelihood_ratio> data, const bit_buffer& scrambling_seq) override
  {
    ocudu_assert(base_buffer != nullptr, "Invalid base buffer.");

    auto tp_before = std::chrono::steady_clock::now();
    base_buffer->on_new_block(data, scrambling_seq);
    auto tp_after = std::chrono::steady_clock::now();

    // Accumulate elapsed time.
    elapsed_on_new_block += tp_after - tp_before;
  }

  // See pusch_codeword_buffer interface for documentation.
  void on_end_codeword() override
  {
    ocudu_assert(base_buffer != nullptr, "Invalid base buffer.");

    auto tp_before = std::chrono::steady_clock::now();
    base_buffer->on_end_codeword();
    auto tp_after = std::chrono::steady_clock::now();

    elapsed_on_end_codeword = tp_after - tp_before;
  }

  std::unique_ptr<pusch_demodulator> base;
  pusch_demodulator_metric_notifier& notifier;
  pusch_codeword_buffer*             base_buffer             = nullptr;
  std::chrono::nanoseconds           elapsed_on_new_block    = {};
  std::chrono::nanoseconds           elapsed_on_end_codeword = {};
};

} // namespace ocudu
