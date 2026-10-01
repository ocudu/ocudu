// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ocudulog/logger.h"
#include "ocudu/support/cli11_utils.h"

namespace ocudu {
namespace app_helpers {

/// Function to capture the log level.
inline auto capture_log_level_function = [](ocudulog::basic_levels& level) {
  return [&level](const std::string& value) {
    auto val = ocudulog::str_to_basic_level(value);
    level    = (val) ? val.value() : ocudulog::basic_levels::none;
  };
};

/// Helper function to add log options to CLI11.
inline CLI::Option*
add_log_option(CLI::App& app, ocudulog::basic_levels& level, const std::string& name, const std::string& descriptrion)
{
  /// Function to check that the log level is correct.
  auto check_log_level = [](const std::string& value) -> std::string {
    if (ocudulog::str_to_basic_level(value).has_value()) {
      return {};
    }

    return fmt::format("Log level '{}' not supported. Accepted values [none,info,debug,warning,error]", value);
  };

  return add_option_function<std::string>(app, name, capture_log_level_function(level), descriptrion)
      ->default_str(ocudulog::basic_level_to_string(level))
      ->check(check_log_level);
}

} // namespace app_helpers
} // namespace ocudu
