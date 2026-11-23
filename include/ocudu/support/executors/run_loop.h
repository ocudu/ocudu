// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/support/executors/manual_task_worker.h"

namespace ocudu {

/// \brief Executor whose tasks are run by the thread that creates it, while it waits for a condition.
///
/// Every dispatched task is enqueued, including the ones dispatched from the thread that runs the loop.
class run_loop : public task_executor
{
public:
  explicit run_loop(size_t queue_size = 64) : worker(queue_size, true, true) {}

  // See the task_executor interface for documentation.
  [[nodiscard]] bool execute(unique_task task) override { return worker.execute(std::move(task)); }

  // See the task_executor interface for documentation.
  [[nodiscard]] bool defer(unique_task task) override { return worker.defer(std::move(task)); }

  /// Runs the dispatched tasks, blocking while there are none, until the given predicate returns true.
  template <typename Predicate>
  void run_until(const Predicate& pred)
  {
    while (not pred()) {
      worker.run_next_blocking();
    }
  }

private:
  /// Queue of dispatched tasks.
  manual_task_worker worker;
};

} // namespace ocudu
