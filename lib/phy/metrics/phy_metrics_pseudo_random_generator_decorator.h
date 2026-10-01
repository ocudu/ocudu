// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/phy/metrics/phy_metrics_notifiers.h"
#include "ocudu/phy/upper/sequence_generators/pseudo_random_generator.h"
#include "ocudu/support/resource_usage/scoped_resource_usage.h"

namespace ocudu {

/// Pseudo-random sequence generator metric decorator.
class phy_metrics_pseudo_random_generator_decorator : public pseudo_random_generator
{
public:
  phy_metrics_pseudo_random_generator_decorator(std::unique_ptr<pseudo_random_generator>          base_generator_,
                                                pseudo_random_sequence_generator_metric_notifier& notifier_) :
    base_generator(std::move(base_generator_)), notifier(notifier_)
  {
    ocudu_assert(base_generator, "Invaluid base generator.");
  }

  // See interface for documentation.
  void init(unsigned c_init) override
  {
    pseudo_random_sequence_generator_metrics metrics;
    {
      // Use scoped resource usage class to measure CPU usage of this block.
      resource_usage_utils::scoped_resource_usage rusage_tracker(metrics.measurements);
      base_generator->init(c_init);
    }
    // Report metrics.
    metrics.method   = pseudo_random_sequence_generator_metrics::methods::init;
    metrics.nof_bits = 0;
    notifier.on_new_metric(metrics);
  }

  // See interface for documentation.
  void init(const state_s& state) override { base_generator->init(state); }

  // See interface for documentation.
  state_s get_state() const override { return base_generator->get_state(); }

  // See interface for documentation.
  void advance(unsigned count) override
  {
    pseudo_random_sequence_generator_metrics metrics;
    {
      // Use scoped resource usage class to measure CPU usage of this block.
      resource_usage_utils::scoped_resource_usage rusage_tracker(metrics.measurements);
      base_generator->advance(count);
    }
    // Report metrics.
    metrics.method   = pseudo_random_sequence_generator_metrics::methods::advance;
    metrics.nof_bits = count;
    notifier.on_new_metric(metrics);
  }

  // See interface for documentation.
  void apply_xor(bit_buffer& out, const bit_buffer& in) override
  {
    pseudo_random_sequence_generator_metrics metrics;
    {
      // Use scoped resource usage class to measure CPU usage of this block.
      resource_usage_utils::scoped_resource_usage rusage_tracker(metrics.measurements);
      base_generator->apply_xor(out, in);
    }
    // Report metrics.
    metrics.method   = pseudo_random_sequence_generator_metrics::methods::apply_xor_packed;
    metrics.nof_bits = in.size();
    notifier.on_new_metric(metrics);
  }

  // See interface for documentation.
  void apply_xor(span<uint8_t> out, span<const uint8_t> in) override
  {
    pseudo_random_sequence_generator_metrics metrics;
    {
      // Use scoped resource usage class to measure CPU usage of this block.
      resource_usage_utils::scoped_resource_usage rusage_tracker(metrics.measurements);
      base_generator->apply_xor(out, in);
    }
    // Report metrics.
    metrics.method   = pseudo_random_sequence_generator_metrics::methods::apply_xor_unpacked;
    metrics.nof_bits = in.size();
    notifier.on_new_metric(metrics);
  }

  // See interface for documentation.
  void apply_xor(span<log_likelihood_ratio> out, span<const log_likelihood_ratio> in) override
  {
    pseudo_random_sequence_generator_metrics metrics;
    {
      // Use scoped resource usage class to measure CPU usage of this block.
      resource_usage_utils::scoped_resource_usage rusage_tracker(metrics.measurements);
      base_generator->apply_xor(out, in);
    }
    // Report metrics.
    metrics.method   = pseudo_random_sequence_generator_metrics::methods::apply_xor_soft_bit;
    metrics.nof_bits = in.size();
    notifier.on_new_metric(metrics);
  }

  // See interface for documentation.
  void generate(bit_buffer& data) override
  {
    pseudo_random_sequence_generator_metrics metrics;
    {
      // Use scoped resource usage class to measure CPU usage of this block.
      resource_usage_utils::scoped_resource_usage rusage_tracker(metrics.measurements);
      base_generator->generate(data);
    }
    // Report metrics.
    metrics.method   = pseudo_random_sequence_generator_metrics::methods::generate_bit_packed;
    metrics.nof_bits = data.size();
    notifier.on_new_metric(metrics);
  }

  // See interface for documentation.
  void generate(span<float> buffer, float value) override
  {
    pseudo_random_sequence_generator_metrics metrics;
    {
      // Use scoped resource usage class to measure CPU usage of this block.
      resource_usage_utils::scoped_resource_usage rusage_tracker(metrics.measurements);
      base_generator->generate(buffer, value);
    }
    // Report metrics.
    metrics.method   = pseudo_random_sequence_generator_metrics::methods::generate_float;
    metrics.nof_bits = buffer.size();
    notifier.on_new_metric(metrics);
  }

private:
  std::unique_ptr<pseudo_random_generator>          base_generator;
  pseudo_random_sequence_generator_metric_notifier& notifier;
};

} // namespace ocudu
