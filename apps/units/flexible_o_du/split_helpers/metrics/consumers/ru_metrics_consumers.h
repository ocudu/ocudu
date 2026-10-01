// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/span.h"
#include "ocudu/ocudulog/log_channel.h"
#include "ocudu/ofh/ofh_metrics.h"
#include "ocudu/ran/pci.h"

namespace ocudu {

namespace app_services {
class remote_server_metrics_gateway;
} // namespace app_services

struct ru_dummy_metrics;
struct ru_sdr_metrics;
struct ru_metrics;

/// JSON handler for the O-RU metrics.
class ru_metrics_consumer_json
{
public:
  ru_metrics_consumer_json(app_services::remote_server_metrics_gateway& gateway_,
                           span<const pci_t>                            pci_sector_map_,
                           std::chrono::nanoseconds                     symbol_duration_) :
    symbol_duration(symbol_duration_), gateway(gateway_), pci_sector_map(pci_sector_map_)
  {
  }

  // Handles the O-RU metrics.
  void handle_metric(const ru_metrics& metric);

private:
  const std::chrono::nanoseconds               symbol_duration;
  app_services::remote_server_metrics_gateway& gateway;
  span<const pci_t>                            pci_sector_map;
};

/// Logger consumer for the O-RU metrics.
class ru_metrics_consumer_log
{
public:
  ru_metrics_consumer_log(ocudulog::log_channel&   log_chan_,
                          span<const pci_t>        pci_sector_map_,
                          std::chrono::nanoseconds symbol_duration_) :
    symbol_duration(symbol_duration_), log_chan(log_chan_), verbose(true), pci_sector_map(pci_sector_map_)
  {
    ocudu_assert(log_chan.enabled(), "JSON log channel is not enabled");
  }

  // Handles the O-RU metrics.
  void handle_metric(const ru_metrics& metric);

private:
  const std::chrono::nanoseconds symbol_duration;
  ocudulog::log_channel&         log_chan;
  const bool                     verbose;
  span<const pci_t>              pci_sector_map;
};

/// STDOUT handler for the O-RU metrics.
class ru_metrics_handler_stdout
{
  /// Maximum number of metric lines in STDOUT without a header, ie, print header every
  /// MAX_NOF_STDOUT_METRIC_LINES_WITHOUT_HEADER lines.
  static constexpr unsigned MAX_NOF_STDOUT_METRIC_LINES_WITHOUT_HEADER = 10;

public:
  ru_metrics_handler_stdout(span<const pci_t> pci_sector_map_, std::chrono::nanoseconds symbol_duration_) :
    symbol_duration(symbol_duration_), pci_sector_map(pci_sector_map_)
  {
  }

  // Handles the O-RU metrics.
  void handle_metric(const ru_metrics& metric);

  /// Prints the header in the next metric handle.
  void print_header() { nof_lines = MAX_NOF_STDOUT_METRIC_LINES_WITHOUT_HEADER; }

private:
  /// Log SDR RU metrics in STDOUT.
  void log_ru_sdr_metrics_in_stdout(const ru_sdr_metrics& sdr_metrics);

  /// Log dummy RU metrics in STDOUT.
  void log_ru_dummy_metrics_in_stdout(const ru_dummy_metrics& dummy_metrics);

  /// Log OFH RU metrics in STDOUT.
  void log_ru_ofh_metrics_in_stdout(const ofh::metrics& ofh_metrics);

private:
  const std::chrono::nanoseconds symbol_duration;
  unsigned                       nof_lines = MAX_NOF_STDOUT_METRIC_LINES_WITHOUT_HEADER;
  span<const pci_t>              pci_sector_map;
};

} // namespace ocudu
