// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "radio_baseband_sector_metrics_collector.h"
#include "ocudu/ru/sdr/ru_sdr_metrics.h"
#include "ocudu/support/math/math_utils.h"

using namespace ocudu;

void radio_baseband_sector_metrics_collector::on_new_transmit_metrics(const radio_baseband_metrics& metrics)
{
  // Skip processing metrics if the number of processed samples is zero.
  if (metrics.clipping.nof_processed_samples == 0) {
    return;
  }

  std::lock_guard lock(tx_mutex);
  tx_avg_power.update(metrics.avg_power);
  tx_peak_power.update(metrics.peak_power);
  if (tx_clipping.nof_processed_samples == 0) {
    tx_clipping = metrics.clipping;
  } else {
    tx_clipping.nof_clipped_samples += metrics.clipping.nof_clipped_samples;
    tx_clipping.nof_processed_samples += metrics.clipping.nof_processed_samples;
  }
  tx_call_duration_ns.update(metrics.call_duration_ns);
}

void radio_baseband_sector_metrics_collector::on_new_receive_metrics(const radio_baseband_metrics& metrics)
{
  // Skip processing metrics if the number of processed samples is zero.
  if (metrics.clipping.nof_processed_samples == 0) {
    return;
  }

  std::lock_guard lock(rx_mutex);
  rx_avg_power.update(metrics.avg_power);
  rx_peak_power.update(metrics.peak_power);
  if (rx_clipping.nof_processed_samples == 0) {
    rx_clipping = metrics.clipping;
  } else {
    rx_clipping.nof_clipped_samples += metrics.clipping.nof_clipped_samples;
    rx_clipping.nof_processed_samples += metrics.clipping.nof_processed_samples;
  }
  rx_call_duration_ns.update(metrics.call_duration_ns);
}

void radio_baseband_sector_metrics_collector::collect_metrics(ru_sdr_sector_metrics& metrics)
{
  {
    std::lock_guard lock(tx_mutex);
    metrics.tx_avg_power_dB  = convert_power_to_dB(tx_avg_power.get_mean());
    metrics.tx_peak_power_dB = convert_power_to_dB(tx_peak_power.get_max());
    metrics.tx_papr_dB       = convert_power_to_dB(tx_peak_power.get_max() / tx_avg_power.get_mean());
    if (tx_clipping.nof_processed_samples != 0) {
      double num               = tx_clipping.nof_clipped_samples;
      double den               = tx_clipping.nof_processed_samples;
      metrics.tx_clipping_prob = num / den;
    }
    double total_call_duration_ns = tx_call_duration_ns.get_mean() * tx_call_duration_ns.get_nof_observations();
    if (std::isnormal(total_call_duration_ns)) {
      metrics.tx_net_call_rate_MHz = (tx_clipping.nof_processed_samples / total_call_duration_ns) * 1e3;
    }
    tx_avg_power.reset();
    tx_peak_power.reset();
    tx_clipping = {};
    tx_call_duration_ns.reset();
  }

  {
    std::lock_guard lock(rx_mutex);
    metrics.rx_avg_power_dB  = convert_power_to_dB(rx_avg_power.get_mean());
    metrics.rx_peak_power_dB = convert_power_to_dB(rx_peak_power.get_max());
    metrics.rx_papr_dB       = convert_power_to_dB(rx_peak_power.get_max() / rx_avg_power.get_mean());
    if (rx_clipping.nof_processed_samples != 0) {
      double num               = rx_clipping.nof_clipped_samples;
      double den               = rx_clipping.nof_processed_samples;
      metrics.rx_clipping_prob = num / den;
    }
    double total_call_duration_ns = rx_call_duration_ns.get_mean() * rx_call_duration_ns.get_nof_observations();
    if (std::isnormal(total_call_duration_ns)) {
      metrics.rx_net_call_rate_MHz = (rx_clipping.nof_processed_samples / total_call_duration_ns) * 1e3;
    }
    rx_avg_power.reset();
    rx_peak_power.reset();
    rx_clipping = {};
    rx_call_duration_ns.reset();
  }
}
