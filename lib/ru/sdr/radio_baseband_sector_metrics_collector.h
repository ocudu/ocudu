// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/radio/radio_baseband_metrics.h"
#include "ocudu/radio/radio_baseband_metrics_notifier.h"
#include "ocudu/support/math/stats.h"
#include <mutex>
#include <optional>

namespace ocudu {

struct ru_sdr_sector_metrics;

/// Lower PHY sector metrics collector.
class radio_baseband_sector_metrics_collector : public radio_baseband_metrics_notifier
{
public:
  // See interface for documentation.
  void on_new_transmit_metrics(const radio_baseband_metrics& metrics) override;

  // See interface for documentation.
  void on_new_receive_metrics(const radio_baseband_metrics& metrics) override;

  /// Collects the metrics of the lower PHY sector and fills the given structure.
  void collect_metrics(ru_sdr_sector_metrics& metrics);

private:
  std::mutex                  tx_mutex;
  sample_statistics<float>    tx_avg_power;
  sample_statistics<float>    tx_peak_power;
  sample_statistics<uint64_t> tx_call_duration_ns;
  clipping_counters           tx_clipping;
  std::mutex                  rx_mutex;
  sample_statistics<float>    rx_avg_power;
  sample_statistics<float>    rx_peak_power;
  clipping_counters           rx_clipping;
  sample_statistics<uint64_t> rx_call_duration_ns;
};

} // namespace ocudu
