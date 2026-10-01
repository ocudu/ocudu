// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/e1ap/cu_up/e1ap_cu_up_metrics.h"
#include "ocudu/support/ocudu_assert.h"

namespace ocudu::ocuup {

/// Container to hold TX/RX metrics
class e1ap_cu_up_metrics_collector
{
  e1ap_cu_up_metrics_container metrics = {};
  bool                         enabled = false;

public:
  e1ap_cu_up_metrics_collector(bool enabled_) : enabled(enabled_) {}

  bool is_enabled() const { return enabled; }

  void reset_metrics() { metrics = {}; }

  e1ap_cu_up_metrics_container get_metrics() const
  {
    ocudu_assert(enabled, "Trying to get metrics, but metrics are disabled.");
    if (not enabled) {
      return {};
    }
    return metrics;
  }

  e1ap_cu_up_metrics_container get_metrics_and_reset()
  {
    ocudu_assert(enabled, "Trying to get metrics, but metrics are disabled.");
    if (not enabled) {
      return {};
    }
    e1ap_cu_up_metrics_container ret = get_metrics();
    reset_metrics();
    return ret;
  }

  void add_successful_context_setup()
  {
    if (not enabled) {
      return;
    }
    metrics.nof_successful_bearer_context_setup++;
  }

  void add_successful_context_modification()
  {
    if (not enabled) {
      return;
    }
    metrics.nof_successful_bearer_context_modification++;
  }

  void add_context_release(std::chrono::microseconds proc_dur)
  {
    if (not enabled) {
      return;
    }
    metrics.nof_bearer_context_release++;

    metrics.sum_release_latency += proc_dur;

    unsigned bin_idx = proc_dur.count() / (1000 * e1ap_cu_up_metrics_container::nof_msec_per_bin);

    bin_idx = std::min(bin_idx, e1ap_cu_up_metrics_container::latency_hist_bins - 1);
    metrics.release_latency_hist[bin_idx]++;

    metrics.max_release_latency = std::max(proc_dur, metrics.max_release_latency);
  }
};
} // namespace ocudu::ocuup
