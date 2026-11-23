// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "ocudu/support/async/async_timer.h"
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

TEST_F(SyncWaitTest, CallsIdleHandlerUntilTaskCompletes)
{
  static constexpr unsigned timeout_ms = 10;

  unique_timer timer;
  unsigned     nof_idle_calls = 0;
  sync_wait(
      [this, &timer](task_executor& exec) {
        timer = timer_factory{timers, exec}.create_timer();
        return launch_async([&timer](coro_context<async_task<void>>& ctx) {
          CORO_BEGIN(ctx);
          CORO_AWAIT(async_wait_for(timer, std::chrono::milliseconds(timeout_ms)));
          CORO_RETURN();
        });
      },
      // The task only completes when the idle handler advances the time.
      [this, &nof_idle_calls]() {
        timers.tick();
        ++nof_idle_calls;
      });
  ASSERT_GE(nof_idle_calls, timeout_ms);
}

TEST_F(SyncWaitTest, ReturnsResultWithIdleHandler)
{
  std::thread::id worker_id = sync_wait([this](task_executor& exec) { return run_on_worker(exec); },
                                        []() { std::this_thread::sleep_for(std::chrono::microseconds(100)); });
  ASSERT_EQ(worker_id, worker.get_id());
}
