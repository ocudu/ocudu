// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/support/async/fifo_async_task_scheduler.h"
#include "ocudu/support/executors/task_executor.h"
#include "ocudu/support/timers.h"
#include <unordered_map>

namespace ocudu {
namespace ocucp {

/// \brief Async FIFO task scheduler for a single UE managed by the CU-CP.
class ue_task_scheduler
{
public:
  virtual ~ue_task_scheduler() = default;

  /// \brief Schedules an async task for the given UE. The task will run after the remaining tasks in the queue are
  /// completed.
  ///
  /// \param task Task to schedule
  /// \return true if the task was successfully enqueued. False, otherwise.
  virtual bool schedule_async_task(async_task<void> task) = 0;

  /// Create a timer for the UE.
  virtual unique_timer create_timer() = 0;

  virtual timer_factory get_timer_factory() = 0;

  virtual task_executor& get_executor() = 0;
};

} // namespace ocucp
} // namespace ocudu
