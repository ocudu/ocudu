// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../../../include/ocudu/radio/radio_baseband_metrics_notifier.h"
#include "ocudu/ocudulog/ocudulog.h"
#include "ocudu/radio/radio_baseband_metrics.h"
#include "ocudu/radio/radio_factory.h"
#include "ocudu/support/math/math_utils.h"
#include "ocudu/support/math/stats.h"
#include "fmt/std.h"

namespace ocudu {

class radio_baseband_metrics_notifier_spy : public radio_baseband_metrics_notifier
{
public:
  explicit radio_baseband_metrics_notifier_spy(ocudulog::basic_levels log_level_) :
    logger(ocudulog::fetch_basic_logger("Radio notification"))
  {
    ocudulog::init();
    logger.set_level(log_level_);
  }

  void on_new_transmit_metrics(const radio_baseband_metrics& metrics) override
  {
    tx_average_power.update(metrics.avg_power);
    tx_papr.update(metrics.peak_power / metrics.avg_power);
    if (metrics.clipping.nof_processed_samples != 0) {
      tx_nof_clipped_samples   = metrics.clipping.nof_clipped_samples;
      tx_nof_processed_samples = metrics.clipping.nof_processed_samples;
    }
  }

  void on_new_receive_metrics(const radio_baseband_metrics& metrics) override {}

  void print()
  {
    std::optional<double> tx_clipping_probability;
    if (tx_nof_processed_samples != 0) {
      tx_clipping_probability =
          static_cast<double>(tx_nof_clipped_samples) / static_cast<double>(tx_nof_processed_samples);
    }
    std::optional<double> rx_clipping_probability;
    if (rx_nof_processed_samples != 0) {
      rx_clipping_probability =
          static_cast<double>(rx_nof_clipped_samples) / static_cast<double>(rx_nof_processed_samples);
    }

    fmt::println("[TX{}] Avg.power={:+.1f}dBFS PAPR={:+.1f}dBFS clipping={:.1e}",
                 convert_power_to_dB(tx_average_power.get_mean()),
                 convert_power_to_dB(tx_papr.get_mean()),
                 tx_clipping_probability);
    fmt::println("[RX{}] Avg.power={:+.1f}dBFS PAPR={:+.1f}dBFS clipping={:.1e}",
                 convert_power_to_dB(rx_average_power.get_mean()),
                 convert_power_to_dB(rx_papr.get_mean()),
                 rx_clipping_probability);
  }

private:
  ocudulog::basic_logger&  logger;
  sample_statistics<float> tx_average_power;
  sample_statistics<float> tx_papr;
  uint64_t                 tx_nof_clipped_samples;
  uint64_t                 tx_nof_processed_samples;
  sample_statistics<float> rx_average_power;
  sample_statistics<float> rx_papr;
  uint64_t                 rx_nof_clipped_samples;
  uint64_t                 rx_nof_processed_samples;
};

} // namespace ocudu
