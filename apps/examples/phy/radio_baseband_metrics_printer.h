// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/radio/radio_baseband_metrics.h"
#include "ocudu/radio/radio_baseband_metrics_notifier.h"
#include "ocudu/support/math/math_utils.h"
#include "ocudu/support/math/stats.h"
#include <mutex>

namespace ocudu {

/// Implements a generic lower physical layer metrics printer.
class radio_baseband_metrics_printer : public radio_baseband_metrics_notifier
{
public:
  // See interface for documentation.
  void on_new_transmit_metrics(const radio_baseband_metrics& metrics) override
  {
    std::lock_guard<std::mutex> lock(tx_mutex);
    tx_avg_power.update(metrics.avg_power);
    tx_peak_power.update(metrics.peak_power);
    if (metrics.clipping.nof_processed_samples != 0) {
      if (tx_clipping.nof_processed_samples == 0) {
        tx_clipping = metrics.clipping;
      } else {
        tx_clipping.nof_clipped_samples += metrics.clipping.nof_clipped_samples;
        tx_clipping.nof_processed_samples += metrics.clipping.nof_processed_samples;
      }
    }
  }

  // See interface for documentation.
  void on_new_receive_metrics(const radio_baseband_metrics& metrics) override
  {
    std::lock_guard<std::mutex> lock(rx_mutex);
    rx_avg_power.update(metrics.avg_power);
    rx_peak_power.update(metrics.peak_power);
    if (metrics.clipping.nof_processed_samples != 0) {
      if (rx_clipping.nof_processed_samples == 0) {
        rx_clipping = metrics.clipping;
      } else {
        rx_clipping.nof_clipped_samples += metrics.clipping.nof_clipped_samples;
        rx_clipping.nof_processed_samples += metrics.clipping.nof_processed_samples;
      }
    }
  }

  static void print_header()
  {
    fmt::println("| ------------------- TX ------------------ | ------------------- RX ------------------ |");
    fmt::println("| Avg. power | Peak power | PAPR | Clipping | Avg. power | Peak power | PAPR | Clipping |");
  }

  void print_metrics()
  {
    float  tx_avg_power_dB;
    float  tx_peak_power_dB;
    float  tx_papr_dB;
    double tx_clipping_prob = std::numeric_limits<double>::quiet_NaN();
    {
      std::lock_guard<std::mutex> lock(tx_mutex);
      tx_avg_power_dB  = convert_power_to_dB(tx_avg_power.get_mean());
      tx_peak_power_dB = convert_power_to_dB(tx_peak_power.get_max());
      tx_papr_dB       = convert_power_to_dB(tx_peak_power.get_max() / tx_avg_power.get_mean());
      if (tx_clipping.nof_processed_samples != 0) {
        double num       = tx_clipping.nof_clipped_samples;
        double den       = tx_clipping.nof_processed_samples;
        tx_clipping_prob = num / den;
      }
      tx_avg_power.reset();
      tx_peak_power.reset();
      tx_clipping = {};
    }

    float  rx_avg_power_dB;
    float  rx_peak_power_dB;
    float  rx_papr_dB;
    double rx_clipping_prob = std::numeric_limits<double>::quiet_NaN();
    {
      std::lock_guard<std::mutex> lock(rx_mutex);
      rx_avg_power_dB  = convert_power_to_dB(rx_avg_power.get_mean());
      rx_peak_power_dB = convert_power_to_dB(rx_peak_power.get_max());
      rx_papr_dB       = convert_power_to_dB(rx_peak_power.get_max() / rx_avg_power.get_mean());
      if (rx_clipping.nof_processed_samples != 0) {
        double num       = rx_clipping.nof_clipped_samples;
        double den       = rx_clipping.nof_processed_samples;
        rx_clipping_prob = num / den;
      }
      rx_avg_power.reset();
      rx_peak_power.reset();
      rx_clipping = {};
    }

    fmt::println("| {:>10.1f} | {:>10.1f} | {:>4.1f} | {:>8.1e} | {:>10.1f} | {:>10.1f} | {:>4.1f} | {:>8.1e} |",
                 tx_avg_power_dB,
                 tx_peak_power_dB,
                 tx_papr_dB,
                 tx_clipping_prob,
                 rx_avg_power_dB,
                 rx_peak_power_dB,
                 rx_papr_dB,
                 rx_clipping_prob);
  }

private:
  std::mutex               tx_mutex;
  sample_statistics<float> tx_avg_power;
  sample_statistics<float> tx_peak_power;
  clipping_counters        tx_clipping;
  std::mutex               rx_mutex;
  sample_statistics<float> rx_avg_power;
  sample_statistics<float> rx_peak_power;
  clipping_counters        rx_clipping;
};

} // namespace ocudu
