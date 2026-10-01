// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/rlc_mode.h"
#include "ocudu/rlc/rlc_rx_metrics.h"
#include "ocudu/support/ocudu_assert.h"

namespace ocudu {

class rlc_rx_metrics_container
{
  rlc_rx_metrics metrics = {};
  bool           enabled = false;

public:
  rlc_rx_metrics_container(bool enabled_) : enabled(enabled_) {}

  void metrics_set_mode(rlc_mode mode)
  {
    if (not enabled) {
      return;
    }
    switch (mode) {
      case rlc_mode::tm:
        metrics.mode_specific = rlc_tm_rx_metrics{};
        break;
      case rlc_mode::um_bidir:
      case rlc_mode::um_unidir_dl:
      case rlc_mode::um_unidir_ul:
        metrics.mode_specific = rlc_um_rx_metrics{};
        break;
      case rlc_mode::am:
        metrics.mode_specific = rlc_am_rx_metrics{};
        break;
      default:
        break;
    }
  }

  void metrics_add_sdus(uint32_t num_sdus_, size_t num_sdu_bytes_)
  {
    if (not enabled) {
      return;
    }
    metrics.num_sdus += num_sdus_;
    metrics.num_sdu_bytes += num_sdu_bytes_;
  }

  void metrics_add_pdus(uint32_t num_pdus_, size_t num_pdu_bytes_)
  {
    if (not enabled) {
      return;
    }
    metrics.num_pdus += num_pdus_;
    metrics.num_pdu_bytes += num_pdu_bytes_;
  }

  void metrics_add_lost_pdus(uint32_t num_pdus_)
  {
    if (not enabled) {
      return;
    }
    metrics.num_lost_pdus += num_pdus_;
  }

  void metrics_add_malformed_pdus(uint32_t num_pdus_)
  {
    if (not enabled) {
      return;
    }
    metrics.num_malformed_pdus += num_pdus_;
  }

  void metrics_add_sdu_latency(uint32_t sdu_latency_us_)
  {
    if (not enabled) {
      return;
    }
    metrics.sdu_latency_us += sdu_latency_us_;
  }

  /// RLC AM specific metrics
  void metrics_add_ctrl_pdus(uint32_t num_ctrl_, uint32_t num_ctrl_pdu_bytes_)
  {
    if (not enabled) {
      return;
    }
    ocudu_assert(std::holds_alternative<rlc_am_rx_metrics>(metrics.mode_specific), "Wrong mode for AM metrics.");
    auto& am = std::get<rlc_am_rx_metrics>(metrics.mode_specific);
    am.num_ctrl_pdus += num_ctrl_;
    am.num_ctrl_pdu_bytes += num_ctrl_pdu_bytes_;
  }

  rlc_rx_metrics get_metrics()
  {
    ocudu_assert(enabled, "Trying to get metrics, but metrics are disabled.");
    if (not enabled) {
      return {};
    }
    return metrics.get();
  }

  rlc_rx_metrics get_and_reset_metrics()
  {
    ocudu_assert(enabled, "Trying to get and reset metrics, but metrics are disabled.");
    if (not enabled) {
      return {};
    }
    rlc_rx_metrics ret = metrics.get();
    reset_metrics();
    return ret;
  }

  void reset_metrics()
  {
    ocudu_assert(enabled, "Trying to reset metrics, but metrics are disabled.");
    if (not enabled) {
      return;
    }
    metrics.reset();
  }
};
} // namespace ocudu
