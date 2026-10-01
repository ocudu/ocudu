// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/adt/blocking_queue.h"
#include "ocudu/support/executors/task_executor.h"

namespace ocudu {

/// \brief Contrarily to other type of workers, this worker runs in the same thread where run() is called.
/// run() is blocking.
class blocking_task_worker final : public task_executor
{
public:
  blocking_task_worker(size_t q_size) : pending_tasks(q_size) {}

  [[nodiscard]] bool execute(unique_task task) override
  {
    pending_tasks.push_blocking(std::move(task));
    return true;
  }

  [[nodiscard]] bool defer(unique_task task) override
  {
    (void)execute(std::move(task));
    return true;
  }

  void request_stop()
  {
    (void)execute([this]() {
      if (not pending_tasks.is_stopped()) {
        pending_tasks.stop();
      }
    });
  }

  void run()
  {
    while (true) {
      bool        success;
      unique_task t = pending_tasks.pop_blocking(&success);
      if (not success) {
        break;
      }
      t();
    }
  }

private:
  blocking_queue<unique_task> pending_tasks;
};

} // namespace ocudu
