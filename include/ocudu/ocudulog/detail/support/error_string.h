// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include <string>

namespace ocudulog {

namespace detail {

/// This is a lightweight error class that encapsulates a string for error
/// reporting.
class error_string
{
  std::string error;

public:
  error_string() = default;

  /*implicit*/ error_string(std::string error_) : error(std::move(error_)) {}
  /*implicit*/ error_string(const char* error_) : error(error_) {}

  /// Returns the error string.
  const std::string& get_error() const { return error; }

  explicit operator bool() const { return !error.empty(); }
};

} // namespace detail

} // namespace ocudulog
