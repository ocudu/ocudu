// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ocudulog/logger.h"
#include "ocudu/ofh/ofh_sector_config.h"

namespace ocudu {

class ru_error_notifier;
class ru_timing_notifier;
class ru_uplink_plane_rx_symbol_notifier;
class task_executor;

/// Radio Unit configuration for the Open Fronthaul implementation.
struct ru_ofh_configuration {
  /// Individual Open Fronthaul sector configurations.
  std::vector<ofh::sector_configuration> sector_configs;
  /// GPS Alpha - Valid value range: [0, 1.2288e7].
  unsigned gps_Alpha;
  /// GPS Beta - Valid value range: [-32768, 32767].
  int gps_Beta;
  /// Busy waiting enabled flag.
  bool enable_busy_waiting = false;
};

/// Radio Unit dependencies for the Open Fronthaul implementation.
struct ru_ofh_dependencies {
  /// Logger.
  ocudulog::basic_logger* logger = nullptr;
  /// Radio Unit timing notifier.
  ru_timing_notifier* timing_notifier = nullptr;
  /// Radio Unit error notifier.
  ru_error_notifier* error_notifier = nullptr;
  /// Radio Unit received symbol notifier.
  ru_uplink_plane_rx_symbol_notifier* rx_symbol_notifier = nullptr;
  /// Realtime timing task executor.
  task_executor* rt_timing_executor = nullptr;

  /// Individual Open Fronthaul sector dependencies.
  std::vector<ofh::sector_dependencies> sector_dependencies;
};

/// Returns true if the given Open Fronthaul configuration is valid, otherwise false.
bool is_valid_ru_ofh_config(const ru_ofh_configuration& config);

} // namespace ocudu
