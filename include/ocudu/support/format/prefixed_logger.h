// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/ocudulog/ocudulog.h"
#include "ocudu/support/format/fmt_to_c_str.h"

namespace ocudu {

/// This utility class allows adding a prefix string to all log entries generated using the provided methods.
template <typename Prefix>
class prefixed_logger
{
  ocudulog::basic_logger&            logger;
  std::shared_ptr<const std::string> log_label;

public:
  prefixed_logger(const std::string& log_name, Prefix prefix, const char* prefix_separator = "") :
    logger(ocudulog::fetch_basic_logger(log_name, false))
  {
    set_prefix(prefix, prefix_separator);
  }

  void set_prefix(Prefix prefix, const char* prefix_separator = "")
  {
    fmt::memory_buffer buffer;
    fmt::format_to(std::back_inserter(buffer), "{}{}", prefix, prefix_separator);
    log_label = std::make_shared<const std::string>(fmt::to_string(buffer));
  }

  template <typename... Args>
  void log_debug(const char* fmt, Args&&... args) const
  {
    log_helper(logger.debug, fmt, std::forward<Args>(args)...);
  }
  template <typename... Args>
  void log_info(const char* fmt, Args&&... args) const
  {
    log_helper(logger.info, fmt, std::forward<Args>(args)...);
  }
  template <typename... Args>
  void log_warning(const char* fmt, Args&&... args) const
  {
    log_helper(logger.warning, fmt, std::forward<Args>(args)...);
  }
  template <typename... Args>
  void log_error(const char* fmt, Args&&... args) const
  {
    log_helper(logger.error, fmt, std::forward<Args>(args)...);
  }

  template <typename... Args>
  void log(const ocudulog::basic_levels level, const char* fmt, Args&&... args) const
  {
    switch (level) {
      case ocudulog::basic_levels::debug:
        log_debug(fmt, std::forward<Args>(args)...);
        break;
      case ocudulog::basic_levels::info:
        log_info(fmt, std::forward<Args>(args)...);
        break;
      case ocudulog::basic_levels::warning:
        log_warning(fmt, std::forward<Args>(args)...);
        break;
      case ocudulog::basic_levels::error:
        log_error(fmt, std::forward<Args>(args)...);
        break;
      case ocudulog::basic_levels::none:
        // Skip.
        break;
      default:
        log_warning("Unsupported log level: {}", basic_level_to_string(level));
        break;
    }
  }

  template <typename It, typename... Args, std::enable_if_t<!std::is_convertible_v<It, const char*>, int> = 0>
  void log_debug(It it_begin, It it_end, const char* fmt, Args&&... args) const
  {
    log_helper(it_begin, it_end, logger.debug, fmt, std::forward<Args>(args)...);
  }
  template <typename It, typename... Args, std::enable_if_t<!std::is_convertible_v<It, const char*>, int> = 0>
  void log_info(It it_begin, It it_end, const char* fmt, Args&&... args) const
  {
    log_helper(it_begin, it_end, logger.info, fmt, std::forward<Args>(args)...);
  }
  template <typename It, typename... Args, std::enable_if_t<!std::is_convertible_v<It, const char*>, int> = 0>
  void log_warning(It it_begin, It it_end, const char* fmt, Args&&... args) const
  {
    log_helper(it_begin, it_end, logger.warning, fmt, std::forward<Args>(args)...);
  }
  template <typename It, typename... Args, std::enable_if_t<!std::is_convertible_v<It, const char*>, int> = 0>
  void log_error(It it_begin, It it_end, const char* fmt, Args&&... args) const
  {
    log_helper(it_begin, it_end, logger.error, fmt, std::forward<Args>(args)...);
  }

  template <typename It, typename... Args>
  void log(const ocudulog::basic_levels level, It it_begin, It it_end, const char* fmt, Args&&... args) const
  {
    switch (level) {
      case ocudulog::basic_levels::debug:
        log_debug(it_begin, it_end, fmt, std::forward<Args>(args)...);
        break;
      case ocudulog::basic_levels::info:
        log_info(it_begin, it_end, fmt, std::forward<Args>(args)...);
        break;
      case ocudulog::basic_levels::warning:
        log_warning(it_begin, it_end, fmt, std::forward<Args>(args)...);
        break;
      case ocudulog::basic_levels::error:
        log_error(it_begin, it_end, fmt, std::forward<Args>(args)...);
        break;
      case ocudulog::basic_levels::none:
        // Skip.
        break;
      default:
        log_warning("Unsupported log level: {}", basic_level_to_string(level));
        break;
    }
  }

  template <typename... Args>
  void log_debug(uint8_t* msg, size_t len, const char* fmt, Args&&... args) const
  {
    log_helper(msg, len, logger.debug, fmt, std::forward<Args>(args)...);
  }
  template <typename... Args>
  void log_debug(const uint8_t* msg, size_t len, const char* fmt, Args&&... args) const
  {
    log_helper(msg, len, logger.debug, fmt, std::forward<Args>(args)...);
  }
  template <typename... Args>
  void log_info(uint8_t* msg, size_t len, const char* fmt, Args&&... args) const
  {
    log_helper(msg, len, logger.info, fmt, std::forward<Args>(args)...);
  }
  template <typename... Args>
  void log_info(const uint8_t* msg, size_t len, const char* fmt, Args&&... args) const
  {
    log_helper(msg, len, logger.info, fmt, std::forward<Args>(args)...);
  }
  template <typename... Args>
  void log_warning(uint8_t* msg, size_t len, const char* fmt, Args&&... args) const
  {
    log_helper(msg, len, logger.warning, fmt, std::forward<Args>(args)...);
  }
  template <typename... Args>
  void log_warning(const uint8_t* msg, size_t len, const char* fmt, Args&&... args) const
  {
    log_helper(msg, len, logger.warning, fmt, std::forward<Args>(args)...);
  }
  template <typename... Args>
  void log_error(uint8_t* msg, size_t len, const char* fmt, Args&&... args) const
  {
    log_helper(msg, len, logger.error, fmt, std::forward<Args>(args)...);
  }
  template <typename... Args>
  void log_error(const uint8_t* msg, size_t len, const char* fmt, Args&&... args) const
  {
    log_helper(msg, len, logger.error, fmt, std::forward<Args>(args)...);
  }

  template <typename... Args>
  void log(const ocudulog::basic_levels level, uint8_t* msg, size_t len, const char* fmt, Args&&... args) const
  {
    switch (level) {
      case ocudulog::basic_levels::debug:
        log_debug(msg, len, fmt, std::forward<Args>(args)...);
        break;
      case ocudulog::basic_levels::info:
        log_info(msg, len, fmt, std::forward<Args>(args)...);
        break;
      case ocudulog::basic_levels::warning:
        log_warning(msg, len, fmt, std::forward<Args>(args)...);
        break;
      case ocudulog::basic_levels::error:
        log_error(msg, len, fmt, std::forward<Args>(args)...);
        break;
      case ocudulog::basic_levels::none:
        // Skip.
        break;
      default:
        log_warning("Unsupported log level: {}", basic_level_to_string(level));
        break;
    }
  }

  ocudulog::basic_logger& get_basic_logger() { return logger; }

private:
  template <typename... Args>
  void log_helper(ocudulog::log_channel& channel, const char* fmt, Args&&... args) const
  {
    channel(log_label, fmt, std::forward<Args>(args)...);
  }

  template <typename It, typename... Args>
  void log_helper(It it_begin, It it_end, ocudulog::log_channel& channel, const char* fmt, Args&&... args) const
  {
    channel(log_label, it_begin, it_end, fmt, std::forward<Args>(args)...);
  }

  template <typename... Args>
  void log_helper(const uint8_t* msg, size_t len, ocudulog::log_channel& channel, const char* fmt, Args&&... args) const
  {
    channel(log_label, msg, len, fmt, std::forward<Args>(args)...);
  }
};

} // namespace ocudu
