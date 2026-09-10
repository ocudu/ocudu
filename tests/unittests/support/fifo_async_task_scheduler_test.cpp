// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "ocudu/support/async/fifo_async_task_scheduler.h"
#include "ocudu/support/test_utils.h"
#include <chrono>

using namespace ocudu;

void test_async_loop_empty_tasks()
{
  fifo_async_task_scheduler loop{128};
  size_t                    nof_tasks = 10000;

  auto tp = std::chrono::steady_clock::now();

  // Action: Run a lot of empty async tasks.
  for (size_t i = 0; i < nof_tasks; ++i) {
    loop.schedule([](coro_context<async_task<void>>& ctx) {
      CORO_BEGIN(ctx);
      CORO_RETURN();
    });
  }

  // Status: The stack shouldn't overflow with so many immediately resumable tasks.

  auto tp2       = std::chrono::steady_clock::now();
  auto diff_usec = std::chrono::duration_cast<std::chrono::microseconds>(tp2 - tp);

  fmt::print("Computes {} empty tasks in {} msec.\n", nof_tasks, diff_usec.count() / (float)1000);
}

int main()
{
  test_async_loop_empty_tasks();
  return 0;
}
