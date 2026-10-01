// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/support/compiler.h"
#include "ocudu/support/error_handling.h"
#include "ocudu/support/rtsan.h"

namespace ocudu {
namespace detail {

/// \brief Helper function to format and print assertion messages, first stage.
///
/// \param filename file name where assertion failed.
/// \param line line in which assertion was placed.
/// \param funcname function name where assertion failed.
/// \param condstr assertion condition that failed.
[[gnu::noinline]] inline void
print_and_abort_1(const char* filename, int line, const char* funcname, const char* condstr) noexcept
{
  OCUDU_RTSAN_SCOPED_DISABLER(d);

  if (auto handler = error_report_handler.exchange(nullptr)) {
    handler();
  }

  ::fflush(stdout);
  fmt::print(stderr, "{}:{}: {}: \n", filename, line, funcname);
  if (condstr == nullptr) {
    fmt::print(stderr, "Assertion failed");
  } else {
    fmt::print(stderr, "Assertion `{}' failed", condstr);
  }
}

/// \brief Helper function to format and print assertion messages, second stage.
///
/// \param msg additional assertion message.
template <typename... Args>
[[gnu::noinline, noreturn]] inline void print_and_abort_2(fmt::string_view fmt, Args&&... args) noexcept
{
  OCUDU_RTSAN_SCOPED_DISABLER(d);

  if (fmt.size()) {
    fmt::print(stderr, " - ");
    fmt::print(stderr, fmt, std::forward<Args>(args)...);
  }
  fmt::print(stderr, "\n");

  std::abort();
}

} // namespace detail
} // namespace ocudu

// NOLINTBEGIN

/// Helper macro to log assertion message and terminate program.
#define OCUDU_ASSERT_FAILURE__(condmessage, fmtstr, ...)                                                               \
  (ocudu::detail::print_and_abort_1(__FILE__, __LINE__, __PRETTY_FUNCTION__, condmessage),                             \
   ocudu::detail::print_and_abort_2(fmtstr, ##__VA_ARGS__))

/// \brief Helper macro that asserts condition is true. If false, it logs the remaining macro args, flushes the log,
/// prints the backtrace (if it was activated) and closes the application.
#define OCUDU_ALWAYS_ASSERT__(condition, fmtstr, ...)                                                                  \
  (void)(OCUDU_LIKELY(condition) || (OCUDU_ASSERT_FAILURE__((#condition), fmtstr, ##__VA_ARGS__), 0))

/// Same as "OCUDU_ALWAYS_ASSERT__" but it is only active when "enable_check" flag is defined
#define OCUDU_ALWAYS_ASSERT_IFDEF__(enable_check, condition, fmtstr, ...)                                              \
  (void)((not OCUDU_IS_DEFINED(enable_check)) || (OCUDU_ALWAYS_ASSERT__(condition, fmtstr, ##__VA_ARGS__), 0))

/// \brief Terminates program with an assertion failure. No condition message is provided.
#define ocudu_assertion_failure(fmtstr, ...)                                                                           \
  (void)((not OCUDU_IS_DEFINED(ASSERTS_ENABLED)) || (OCUDU_ASSERT_FAILURE__(nullptr, fmtstr, ##__VA_ARGS__), 0))

/// Specialization of "OCUDU_ALWAYS_ASSERT_IFDEF__" for the ASSERTS_ENABLED flag.
#define ocudu_assert(condition, fmtstr, ...)                                                                           \
  OCUDU_ALWAYS_ASSERT_IFDEF__(ASSERTS_ENABLED, condition, fmtstr, ##__VA_ARGS__)

/// Specialization of "OCUDU_ALWAYS_ASSERT_IFDEF__" for the PARANOID_ASSERTS_ENABLED flag.
#define ocudu_sanity_check(condition, fmtstr, ...)                                                                     \
  OCUDU_ALWAYS_ASSERT_IFDEF__(PARANOID_ASSERTS_ENABLED, condition, fmtstr, ##__VA_ARGS__)

#define ocudu_assume(condition) static_cast<void>((condition) ? void(0) : OCUDU_UNREACHABLE)

// NOLINTEND
