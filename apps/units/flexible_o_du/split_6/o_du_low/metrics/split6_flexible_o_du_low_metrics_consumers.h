// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "apps/services/metrics/metrics_consumer.h"
#include "apps/units/flexible_o_du/split_helpers/metrics/consumers/o_du_low_metrics_consumers.h"
#include "apps/units/flexible_o_du/split_helpers/metrics/consumers/ru_metrics_consumers.h"

namespace ocudu {

namespace app_services {
class remote_server_metrics_gateway;
} // namespace app_services

struct ru_metrics;

/// Log consumer for the split 6 flexible O-DU low metrics.
class split6_flexible_o_du_low_metrics_consumer_log : public app_services::metrics_consumer
{
public:
  split6_flexible_o_du_low_metrics_consumer_log(ocudulog::log_channel&   log_chan,
                                                std::vector<pci_t>       pci_sector_map_,
                                                bool                     verbose,
                                                std::chrono::nanoseconds symbol_duration) :
    pci_sector_map(std::move(pci_sector_map_)),
    odu_low_metrics_handler(log_chan, verbose),
    ru_metrics_handler(log_chan, pci_sector_map, symbol_duration)
  {
  }

  // See interface for documentation.
  void handle_metric(const app_services::metrics_set& metric) override;

private:
  const std::vector<pci_t>      pci_sector_map;
  o_du_low_metrics_consumer_log odu_low_metrics_handler;
  ru_metrics_consumer_log       ru_metrics_handler;
};

/// JSON consumer for the flexible O-DU metrics.
class split6_flexible_o_du_low_metrics_consumer_json : public app_services::metrics_consumer
{
public:
  split6_flexible_o_du_low_metrics_consumer_json(app_services::remote_server_metrics_gateway& gateway,
                                                 std::vector<pci_t>                           pci_sector_map_,
                                                 std::chrono::nanoseconds                     symbol_duration) :
    pci_sector_map(std::move(pci_sector_map_)),
    odu_low_metrics_handler(gateway),
    ru_metrics_handler(gateway, pci_sector_map, symbol_duration)

  {
  }

  // See interface for documentation.
  void handle_metric(const app_services::metrics_set& metric) override;

private:
  const std::vector<pci_t>       pci_sector_map;
  o_du_low_metrics_consumer_json odu_low_metrics_handler;
  ru_metrics_consumer_json       ru_metrics_handler;
};

} // namespace ocudu
