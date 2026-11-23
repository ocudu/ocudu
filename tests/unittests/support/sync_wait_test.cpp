// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "ocudu/support/async/execute_on_blocking.h"
#include "ocudu/support/async/sync_wait.h"
#include "ocudu/support/executors/task_worker.h"
#include "ocudu/support/timers.h"
#include <gtest/gtest.h>
#include <thread>

using namespace ocudu;

namespace {

class SyncWaitTest : public ::testing::Test
{
protected:
  /// Creates a task that runs on the worker thread and completes on the given continuation executor.
  async_task<std::thread::id> run_on_worker(task_executor& continuation_exec)
  {
    return launch_async([this, &continuation_exec, worker_id = std::thread::id{}](
                            coro_context<async_task<std::thread::id>>& ctx) mutable {
      CORO_BEGIN(ctx);
      CORO_AWAIT(defer_on_blocking(worker_exec, timers));
      worker_id = std::this_thread::get_id();
      CORO_AWAIT(defer_on_blocking(continuation_exec, timers));
      CORO_RETURN(worker_id);
    });
  }

  timer_manager        timers;
  task_worker          worker{"worker", 16};
  task_worker_executor worker_exec = make_task_executor(worker);
};

} // namespace

TEST_F(SyncWaitTest, ReturnsResultOfTaskThatRunsOnAnotherThread)
{
  std::thread::id worker_id = sync_wait([this](task_executor& exec) { return run_on_worker(exec); });
  ASSERT_EQ(worker_id, worker.get_id());
  ASSERT_NE(worker_id, std::this_thread::get_id());
}

TEST_F(SyncWaitTest, WaitsForVoidTask)
{
  std::thread::id worker_id;
  sync_wait([this, &worker_id](task_executor& exec) {
    return launch_async([this, &exec, &worker_id](coro_context<async_task<void>>& ctx) {
      CORO_BEGIN(ctx);
      CORO_AWAIT_VALUE(worker_id, run_on_worker(exec));
      CORO_RETURN();
    });
  });
  ASSERT_EQ(worker_id, worker.get_id());
}

TEST_F(SyncWaitTest, CompletesTaskThatDoesNotSuspend)
{
  int value = sync_wait([](task_executor& exec) {
    return launch_async([](coro_context<async_task<int>>& ctx) {
      CORO_BEGIN(ctx);
      CORO_RETURN(5);
    });
  });
  ASSERT_EQ(value, 5);
}
