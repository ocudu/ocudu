// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/phy/lower/lower_phy_configuration.h"
#include "ocudu/radio/radio_configuration.h"
#include "ocudu/ru/ru_error_notifier.h"
#include "ocudu/ru/ru_timing_notifier.h"
#include "ocudu/ru/ru_uplink_plane.h"

namespace ocudu {

/// SDR Radio Unit configuration.
struct ru_sdr_configuration {
  /// Metrics enabled flag.
  bool are_metrics_enabled;
  /// Device driver.
  std::string device_driver;
  /// Radio configuration.
  radio_configuration::radio radio_cfg;
  /// Lower PHY configurations.
  std::vector<lower_phy_configuration> lower_phy_config;
  /// \brief Optional starting time.
  ///
  /// Starts radio operation at the given time. The use of this parameter is for starting radio operation of several
  /// instances at same time.
  std::optional<std::chrono::system_clock::time_point> start_time;
};

/// SDR RU sector dependencies.
struct ru_sdr_sector_dependencies {
  /// Logger.
  ocudulog::basic_logger& logger;
  /// Receive task executor.
  task_executor& rx_task_executor;
  /// Transmit task executor.
  task_executor& tx_task_executor;
  /// Downlink task executor.
  task_executor& dl_task_executor;
  /// Uplink task executor.
  task_executor& ul_task_executor;
  /// PRACH asynchronous task executor.
  task_executor& prach_async_executor;
};

/// SDR Radio Unit dependencies.
struct ru_sdr_dependencies {
  /// Radio executor.
  task_executor& radio_exec;
  /// Radio logger.
  ocudulog::basic_logger& rf_logger;
  /// Radio Unit uplink plane received symbol notifier.
  ru_uplink_plane_rx_symbol_notifier& symbol_notifier;
  /// Radio Unit timing notifier.
  ru_timing_notifier& timing_notifier;
  /// Radio Unit error notifier.
  ru_error_notifier& error_notifier;
  /// SDR RU sector dependencies.
  std::vector<ru_sdr_sector_dependencies> sector_dependencies;
};

} // namespace ocudu
