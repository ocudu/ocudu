// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/support/executors/task_executor.h"
#include "ocudu/support/timers.h"

namespace ocudu {

/// Execute task in the given task executor. If dispatch fails, postpone execution via timers.
///
/// This function is useful for tasks whose dispatch can never fail.
template <typename TaskExecutor, typename CopyableTask>
void execute_until_success(TaskExecutor& exec, timer_manager& timers, CopyableTask&& task)
{
  static_assert(std::is_copy_constructible_v<CopyableTask>, "CopyableTask must be copyable");

  // Try to dispatch right away.
  if (not exec.execute(task)) {
    // If it fails, resort to timers.
    auto timer = timers.create_unique_timer(exec);
    timer.set(timer_duration{0}, [task]() mutable { task(); });
    timer.run();
  }
}

/// Defer task in the given task executor. If dispatch fails, postpone defer.
///
/// This function is useful for tasks whose dispatch can never fail.
template <typename TaskExecutor, typename CopyableTask>
void defer_until_success(TaskExecutor& exec, timer_manager& timers, CopyableTask&& task)
{
  static_assert(std::is_copy_constructible_v<CopyableTask>, "CopyableTask must be copyable");

  // Try to dispatch right away.
  if (not exec.defer(task)) {
    // If it fails, resort to timers.
    auto timer = timers.create_unique_timer(exec);
    timer.set(timer_duration{0}, [task]() mutable { task(); });
    timer.run();
  }
}

} // namespace ocudu
