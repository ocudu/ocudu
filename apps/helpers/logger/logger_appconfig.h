// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ocudulog/logger.h"
#include <string>

namespace ocudu {

/// Configuration of logging functionalities.
struct logger_appconfig {
  /// Path to log file or "stdout" to print to console.
  std::string filename = "stdout";
  /// Default log level for all layers.
  ocudulog::basic_levels all_level = ocudulog::basic_levels::warning;
  /// Generic log level assigned to library components without layer-specific level.
  ocudulog::basic_levels lib_level    = ocudulog::basic_levels::warning;
  ocudulog::basic_levels e2ap_level   = ocudulog::basic_levels::warning;
  ocudulog::basic_levels config_level = ocudulog::basic_levels::none;

  /// Maximum number of bytes to write when dumping hex arrays.
  int hex_max_size = 0;

  /// Flush the logger when the application start and stop banners are written.
  bool flush_app_banner = false;
};

} // namespace ocudu
