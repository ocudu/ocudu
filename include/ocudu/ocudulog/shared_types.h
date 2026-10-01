// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include <functional>
#include <string>

namespace ocudulog {

/// Generic error handler callback.
using error_handler = std::function<void(const std::string&)>;

/// Backend priority levels.
enum class backend_priority {
  /// Default priority of the operating system.
  normal,
  /// Thread will be given a high priority.
  high,
  /// Thread will be given a very high priority.
  very_high
};

/// syslog log local types
enum class syslog_local_type {
  local0,
  local1,
  local2,
  local3,
  local4,
  local5,
  local6,
  local7,
};

} // namespace ocudulog
