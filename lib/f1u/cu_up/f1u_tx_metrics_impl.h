// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/f1u/cu_up/f1u_tx_metrics.h"
#include "ocudu/support/ocudu_assert.h"

namespace ocudu::ocuup {

class f1u_tx_metrics
{
  f1u_tx_metrics_container metrics = {};
  bool                     enabled = false;

public:
  f1u_tx_metrics(bool enabled_) : enabled(enabled_) {}

  bool is_enabled() const { return enabled; }

  void add_sdus(uint32_t num_sdus_, size_t num_sdu_bytes_)
  {
    if (not enabled) {
      return;
    }
    metrics.num_sdus += num_sdus_;
    metrics.num_sdu_bytes += num_sdu_bytes_;
  }

  void add_dropped_sdus(uint32_t num_sdus)
  {
    if (not enabled) {
      return;
    }
    metrics.num_dropped_sdus += num_sdus;
  }

  void add_sdu_discards(uint32_t num_sdus)
  {
    if (not enabled) {
      return;
    }
    metrics.num_sdu_discards += num_sdus;
  }

  void add_pdus(uint32_t num_pdus_)
  {
    if (not enabled) {
      return;
    }
    metrics.num_pdus += num_pdus_;
  }

  f1u_tx_metrics_container get_metrics()
  {
    ocudu_assert(enabled, "Trying to get metrics, but metrics are disabled.");
    if (not enabled) {
      return {};
    }
    f1u_tx_metrics_container ret = metrics.copy();
    return ret;
  }

  f1u_tx_metrics_container get_metrics_and_reset()
  {
    ocudu_assert(enabled, "Trying to get metrics, but metrics are disabled.");
    if (not enabled) {
      return {};
    }
    f1u_tx_metrics_container ret = get_metrics();
    reset_metrics();
    return ret;
  }

  void add_crypto_processing_latency(uint32_t crypto_processing_latency)
  {
    if (not enabled) {
      return;
    }
  }

  void reset_metrics()
  {
    ocudu_assert(enabled, "Trying to reset metrics, but metrics are disabled.");
    if (not enabled) {
      return;
    }
    // Ugly way to reset all metrics but not the counter.
    unsigned counter = metrics.counter;
    metrics          = {};
    metrics.counter  = counter;
  }
};
} // namespace ocudu::ocuup
