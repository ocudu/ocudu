// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "fmt/args.h"
#include <chrono>

namespace ocudulog {

namespace detail {

/// This structure gives the user a way to log generic information as a context.
//: TODO: legacy struct, will get replaced by the new context framework.
struct log_context {
  /// Generic context value.
  uint64_t value64;
  /// When true, the context value will be printed in the log entry.
  bool enabled;
};

/// Metadata fields carried for each log entry.
struct log_entry_metadata {
  std::chrono::system_clock::time_point               tp;
  log_context                                         context;
  const char*                                         fmtstring;
  fmt::dynamic_format_arg_store<fmt::format_context>* store;
  std::string                                         log_name;
  char                                                log_tag;
  std::shared_ptr<const std::string>                  log_label;
  std::vector<uint8_t>                                hex_dump;
};

} // namespace detail

} // namespace ocudulog
