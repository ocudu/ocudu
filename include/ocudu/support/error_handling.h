// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/support/rtsan.h"
#include "fmt/base.h"
#include <atomic>
#include <cstdio>
#include <cstdlib>

namespace ocudu {

/// Callback type for the error handler.
using ocudu_error_handler = void (*)();
/// Error handler instance.
inline std::atomic<ocudu_error_handler> error_report_handler = nullptr;

/// Sets a new error handler to be used whenever a serious (non-recoverable) error is encountered.
inline void set_error_handler(ocudu_error_handler handler)
{
  error_report_handler = handler;
}

/// \brief Command to terminate application with an error message.
///
/// Attribute noinline is used to signal to the compiler that this path should rarely occur and therefore doesn't need
/// to get optimized.
template <typename... Args>
[[gnu::noinline, noreturn]] inline bool ocudu_terminate(const char* reason_fmt, Args&&... args) noexcept
{
  OCUDU_RTSAN_SCOPED_DISABLER(d);

  if (auto handler = error_report_handler.exchange(nullptr)) {
    handler();
  }
  ::fflush(stdout);
  fmt::print(stderr, "OCUDU FATAL ERROR: ");
  fmt::println(stderr, reason_fmt, std::forward<Args>(args)...);

  std::abort();
}

/// \brief Reports an error and closes the application gracefully.
///
/// This function is intended to be used for error conditions that may be triggered by the user or through invalid
/// configurations.
template <typename... Args>
[[gnu::noinline, noreturn]] inline void report_error(const char* reason_fmt, Args&&... args) noexcept
{
  OCUDU_RTSAN_SCOPED_DISABLER(d);

  if (auto handler = error_report_handler.exchange(nullptr)) {
    handler();
  }
  ::fflush(stdout);
  fmt::print(stderr, "OCUDU ERROR: ");
  fmt::println(stderr, reason_fmt, std::forward<Args>(args)...);

  std::quick_exit(1);
}

/// \brief Reports a fatal error and handles the application shutdown.
///
/// This function is intended to be used for error conditions that are neither caught by the compiler nor possible to
/// be handled by the application at runtime.
template <typename... Args>
[[gnu::noinline, noreturn]] inline void report_fatal_error(const char* reason_fmt, Args&&... args) noexcept
{
  ocudu_terminate(reason_fmt, std::forward<Args>(args)...);
}

/// \brief Verifies if condition is true. If not, report a fatal error and closes the application.
#define report_fatal_error_if_not(condition, fmtstr, ...)                                                              \
  (void)((condition) || (report_fatal_error(fmtstr, ##__VA_ARGS__), 0))

/// \brief Verifies if condition is true. If not, report an error and close the application.
#define report_error_if_not(condition, fmtstr, ...) (void)((condition) || (report_error(fmtstr, ##__VA_ARGS__), 0))

} // namespace ocudu
