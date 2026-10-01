// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "ocudu/support/async/async_task.h"
#include "ocudu/support/async/async_test_utils.h"
#include "ocudu/support/async/manual_event.h"
#include "ocudu/support/async/when_all.h"
#include <gtest/gtest.h>

using namespace ocudu;

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

namespace ocudu::detail {

template <typename R>
struct when_all_caller {
  when_all_awaitable<R> awaitable;
  bool                  completed = false;
  std::vector<R>        results;

  explicit when_all_caller(std::vector<async_task<R>> tasks) : awaitable(when_all(std::move(tasks)))
  {
    t = launch_async([this](coro_context<eager_async_task<void>>& ctx) {
      CORO_BEGIN(ctx);
      CORO_AWAIT_VALUE(results, awaitable);
      completed = true;
      CORO_RETURN();
    });
  }

private:
  eager_async_task<void> t;
};

struct when_all_void_caller {
  when_all_awaitable<void> awaitable;
  bool                     completed = false;

  explicit when_all_void_caller(std::vector<async_task<void>> tasks) : awaitable(when_all(std::move(tasks)))
  {
    t = launch_async([this](coro_context<eager_async_task<void>>& ctx) {
      CORO_BEGIN(ctx);
      CORO_AWAIT(awaitable);
      completed = true;
      CORO_RETURN();
    });
  }

private:
  eager_async_task<void> t;
};

} // namespace ocudu::detail

// ---------------------------------------------------------------------------
// Tests
// ---------------------------------------------------------------------------

TEST(when_all_test, empty_task_list_completes_immediately)
{
  detail::when_all_caller<int> caller{{}};
  ASSERT_TRUE(caller.completed);
  ASSERT_TRUE(caller.results.empty());
}

TEST(when_all_test, single_task_completes_on_event)
{
  wait_manual_event_tester<int> ev{42};
  std::vector<async_task<int>>  tasks;
  tasks.push_back(ev.launch());
  detail::when_all_caller<int> caller{std::move(tasks)};

  ASSERT_FALSE(caller.completed);

  ev.ready_ev.set();
  ASSERT_TRUE(caller.completed);
  ASSERT_EQ(1u, caller.results.size());
  ASSERT_EQ(42, caller.results[0]);
}

TEST(when_all_test, caller_waits_for_all_tasks)
{
  wait_manual_event_tester<int> ev0{1}, ev1{2}, ev2{3};
  std::vector<async_task<int>>  tasks;
  tasks.push_back(ev0.launch());
  tasks.push_back(ev1.launch());
  tasks.push_back(ev2.launch());
  detail::when_all_caller<int> caller{std::move(tasks)};

  ASSERT_FALSE(caller.completed);
  ev0.ready_ev.set();
  ASSERT_FALSE(caller.completed);
  ev2.ready_ev.set();
  ASSERT_FALSE(caller.completed);
  ev1.ready_ev.set();
  ASSERT_TRUE(caller.completed);
}

TEST(when_all_test, results_are_in_original_task_order)
{
  wait_manual_event_tester<int> ev0{10}, ev1{20}, ev2{30};
  std::vector<async_task<int>>  tasks;
  tasks.push_back(ev0.launch());
  tasks.push_back(ev1.launch());
  tasks.push_back(ev2.launch());
  detail::when_all_caller<int> caller{std::move(tasks)};

  // Complete in reverse order.
  ev2.ready_ev.set();
  ev1.ready_ev.set();
  ev0.ready_ev.set();

  ASSERT_TRUE(caller.completed);
  ASSERT_EQ(3u, caller.results.size());
  ASSERT_EQ(10, caller.results[0]);
  ASSERT_EQ(20, caller.results[1]);
  ASSERT_EQ(30, caller.results[2]);
}

TEST(when_all_test, synchronous_tasks_complete_without_suspension)
{
  std::vector<async_task<int>> tasks;
  tasks.push_back(launch_no_op_task(1));
  tasks.push_back(launch_no_op_task(2));
  tasks.push_back(launch_no_op_task(3));
  detail::when_all_caller<int> caller{std::move(tasks)};

  ASSERT_TRUE(caller.completed);
  ASSERT_EQ(3u, caller.results.size());
  ASSERT_EQ(1, caller.results[0]);
  ASSERT_EQ(2, caller.results[1]);
  ASSERT_EQ(3, caller.results[2]);
}

TEST(when_all_test, void_tasks_complete_when_all_finish)
{
  wait_manual_event_tester<void> ev0, ev1;
  std::vector<async_task<void>>  tasks;
  tasks.push_back(ev0.launch());
  tasks.push_back(ev1.launch());
  detail::when_all_void_caller caller{std::move(tasks)};

  ASSERT_FALSE(caller.completed);
  ev0.ready_ev.set();
  ASSERT_FALSE(caller.completed);
  ev1.ready_ev.set();
  ASSERT_TRUE(caller.completed);
}
