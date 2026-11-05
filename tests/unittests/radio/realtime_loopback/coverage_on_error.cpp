// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#ifdef ENABLE_GCOV

#include "ocudu/support/error_handling.h"

/// Writes the coverage counters, provided by the coverage runtime.
extern "C" void __gcov_dump();

namespace {

/// \brief Registers an error handler that writes the coverage counters.
///
/// Fatal errors and assertions terminate the death test processes before the coverage counters are written, which
/// would report the lines leading to them as not covered.
const bool coverage_on_error_registered = []() {
  ocudu::set_error_handler([]() { __gcov_dump(); });
  return true;
}();

} // namespace

#endif // ENABLE_GCOV
