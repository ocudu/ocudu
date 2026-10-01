// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "ocudu/support/executors/executor_decoration_factory.h"
#include "ocudu/support/executors/sync_task_executor.h"
#include "ocudu/support/executors/task_worker.h"
#include <gtest/gtest.h>

using namespace ocudu;

TEST(sync_task_worker, task_is_synced)
{
  task_worker w{"WORKER", 1};
  auto        exec      = make_task_executor(w);
  auto        sync_exec = make_sync_executor(exec);

  std::atomic<unsigned> counter{0};
  for (unsigned i = 0; i != 10; ++i) {
    ASSERT_TRUE(sync_exec->execute([&counter]() { ++counter; }));
    ASSERT_EQ(counter, i + 1);
  }

  w.stop();
}

TEST(sync_task_worker, multiple_producers_sync_their_tasks)
{
  task_worker w{"WORKER", 10};
  auto        exec      = make_task_executor(w);
  auto        sync_exec = make_sync_executor(exec);

  std::atomic<unsigned> counter{0};
  auto                  f = [&counter, &sync_exec]() {
    for (unsigned i = 0; i != 10; ++i) {
      unsigned prev_count = ++counter;
      unsigned result;
      ASSERT_TRUE(sync_exec->execute([prev_count, &result]() { result = prev_count + 1; }));
      ASSERT_EQ(result, prev_count + 1);
    }
  };

  std::thread t1(f);
  std::thread t2(f);
  t1.join();
  t2.join();

  w.stop();
}

TEST(sync_task_worker, decorate_with_sync_cfg_instantiates_sync_executor)
{
  task_worker w{"WORKER", 1};
  auto        exec      = decorate_executor(make_task_executor_ptr(w),
                                execution_decoration_config{.sync = execution_decoration_config::sync_option{}});
  auto        sync_exec = make_sync_executor(exec);

  std::atomic<unsigned> counter{0};
  for (unsigned i = 0; i != 10; ++i) {
    ASSERT_TRUE(sync_exec->execute([&counter]() { ++counter; }));
    ASSERT_EQ(counter, i + 1);
  }

  w.stop();
}
