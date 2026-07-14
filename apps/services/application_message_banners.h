// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "external/fmt/include/fmt/core.h"
#include "ocudu/ocudulog/logger.h"
#include "ocudu/support/format/fmt_to_c_str.h"
#include "ocudu/support/versioning/build_info.h"

namespace ocudu {
namespace app_services {

/// \brief Application message banners service.
///
/// Announces in STDOUT when the application starts and stops.
class application_message_banners
{
public:
  /// Announces the application started.
  application_message_banners(std::string_view        app_name,
                              std::string_view        log_filename_,
                              ocudulog::basic_logger& logger_,
                              ocudulog::basic_levels  original_level_,
                              bool                    flush_banner_ = false) :
    log_filename(log_filename_), logger(logger_), original_level(original_level_), flush_banner(flush_banner_)
  {
    fmt::memory_buffer buffer;
    fmt::format_to(std::back_inserter(buffer), "==== {} started ===", app_name);

    log_impl(buffer);

    fmt::println("Type <h> to view help");
  }

  /// Announces the application is stopping.
  ~application_message_banners()
  {
    fmt::memory_buffer buffer;
    fmt::format_to(std::back_inserter(buffer), "Stopping...");

    log_impl(buffer);

    if (!log_filename.empty()) {
      fmt::println("Logfile stored in {}", log_filename);
    }
  }

  /// Announces the application name and version using its build hash.
  static void announce_app_and_version(std::string_view app_name)
  {
    fmt::print("\n--== OCUDU {} (commit {}) ==--\n\n", app_name, get_build_hash());
  }

  /// Logs in the given logger application build parameters.
  static void log_build_info(ocudulog::basic_logger& logger)
  {
    // Log build info
    logger.info("Built in {} mode using {}", get_build_mode(), get_build_info());
  }

private:
  /// Logs the given buffer in the logger and STDOUT.
  void log_impl(fmt::memory_buffer& buffer) const
  {
    logger.set_level(ocudulog::basic_levels::info);
    logger.info("{}", to_c_str(buffer));
    if (flush_banner) {
      ocudulog::flush();
    }
    logger.set_level(original_level);

    fmt::println("{}", to_c_str(buffer));
  }

private:
  std::string                  log_filename;
  ocudulog::basic_logger&      logger;
  const ocudulog::basic_levels original_level;
  const bool                   flush_banner;
};

} // namespace app_services
} // namespace ocudu
