// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include <atomic>
#include <cstdint>

namespace ocudu {
namespace futex_util {

/// \brief Suspends running thread if state == expected. Otherwise, it returns immediately.
/// \return 0 on success (state != expected or spurious wake-up), -1 on error (errno is set accordingly).
/// \remark Spurious wakeups or EINTR can happen. So callers need to re-check the condition in a loop.
long wait(std::atomic<uint32_t>& state, uint32_t expected);

/// \brief Wakes all threads waiting on the state variable.
/// \return number of awoken threads or -1 on error (errno is set accordingly).
long wake_all(std::atomic<uint32_t>& state);

} // namespace futex_util
} // namespace ocudu
