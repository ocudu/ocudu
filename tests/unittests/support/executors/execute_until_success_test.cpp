// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "ocudu/support/executors/execute_until_success.h"
#include "ocudu/support/executors/manual_task_worker.h"
#include <gtest/gtest.h>

using namespace ocudu;

namespace {

/// Executor that rejects a given number of dispatches before forwarding them to the wrapped executor.
class failing_executor final : public task_executor
{
public:
  failing_executor(task_executor& exec_, unsigned nof_failures_) : exec(exec_), nof_failures(nof_failures_) {}

  [[nodiscard]] bool execute(unique_task task) override
  {
    if (nof_failures > 0) {
      --nof_failures;
      return false;
    }
    return exec.execute(std::move(task));
  }

  [[nodiscard]] bool defer(unique_task task) override
  {
    if (nof_failures > 0) {
      --nof_failures;
      return false;
    }
    return exec.defer(std::move(task));
  }

private:
  task_executor& exec;
  unsigned       nof_failures;
};

} // namespace

class execute_until_success_test : public ::testing::TestWithParam<unsigned>
{
protected:
  void run_ticks(unsigned nof_ticks)
  {
    for (unsigned i = 0; i != nof_ticks; ++i) {
      timers.tick();
      worker.run_pending_tasks();
    }
  }

  timer_manager      timers{4};
  manual_task_worker worker{16};
  failing_executor   exec{worker, GetParam()};
};

TEST_P(execute_until_success_test, execute_runs_task_once_despite_dispatch_failures)
{
  unsigned count = 0;
  execute_until_success(exec, timers, [&count]() { ++count; });
  run_ticks(GetParam() + 2);
  ASSERT_EQ(count, 1);
}

TEST_P(execute_until_success_test, defer_runs_task_once_despite_dispatch_failures)
{
  unsigned count = 0;
  defer_until_success(exec, timers, [&count]() { ++count; });
  run_ticks(GetParam() + 2);
  ASSERT_EQ(count, 1);
}

INSTANTIATE_TEST_SUITE_P(execute_until_success_test, execute_until_success_test, ::testing::Values(0, 1, 2, 5));
