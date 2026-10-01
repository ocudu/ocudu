// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/phy/lower/lower_phy_baseband_metrics.h"
#include "ocudu/phy/lower/lower_phy_metrics_notifier.h"
#include "ocudu/support/math/stats.h"
#include <mutex>
#include <optional>

namespace ocudu {

struct ru_sdr_sector_metrics;

/// Lower PHY sector metrics collector.
class lower_phy_sector_metrics_collector : public lower_phy_metrics_notifier
{
public:
  // See interface for documentation.
  void on_new_transmit_metrics(const lower_phy_baseband_metrics& metrics) override;

  // See interface for documentation.
  void on_new_receive_metrics(const lower_phy_baseband_metrics& metrics) override;

  /// Collects the metrics of the lower PHY sector and fills the given structure.
  void collect_metrics(ru_sdr_sector_metrics& metrics);

private:
  std::mutex                       tx_mutex;
  sample_statistics<float>         tx_avg_power;
  sample_statistics<float>         tx_peak_power;
  std::optional<clipping_counters> tx_clipping;
  std::mutex                       rx_mutex;
  sample_statistics<float>         rx_avg_power;
  sample_statistics<float>         rx_peak_power;
  std::optional<clipping_counters> rx_clipping;
};

} // namespace ocudu
