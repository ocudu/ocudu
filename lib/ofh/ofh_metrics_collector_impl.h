// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/ofh/ethernet/ethernet_receiver_metrics_collector.h"
#include "ocudu/ofh/ethernet/ethernet_transmitter_metrics_collector.h"
#include "ocudu/ofh/ofh_sector_metrics_collector.h"
#include "ocudu/ofh/receiver/ofh_receiver_metrics_collector.h"
#include "ocudu/ofh/transmitter/ofh_transmitter_metrics_collector.h"
#include <chrono>

namespace ocudu {
namespace ofh {

/// Open Fronthaul sector metrics collector implementation.
class metrics_collector_impl : public metrics_collector
{
  bool                           is_enabled = false;
  const unsigned                 sector_id;
  receiver_metrics_collector*    rx_metrics_collector;
  transmitter_metrics_collector* tx_metrics_collector;

  /// The time point when the metrics where collected last time.
  std::chrono::steady_clock::time_point last_timestamp = {};

public:
  metrics_collector_impl(receiver_metrics_collector*    rx_metrics_collector_,
                         transmitter_metrics_collector* tx_metrics_collector_,
                         unsigned                       sector_id_);

  // See interface for documentation.
  void collect_metrics(sector_metrics& metric) override;

  /// Returns true if the metrics are disabled, otherwise true.
  bool disabled() const { return !is_enabled; }
};

} // namespace ofh
} // namespace ocudu
