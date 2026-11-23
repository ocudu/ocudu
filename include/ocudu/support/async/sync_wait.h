// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/support/async/coroutine.h"
#include "ocudu/support/async/eager_async_task.h"
#include "ocudu/support/executors/run_loop.h"
#include <optional>
#include <type_traits>

namespace ocudu {

namespace detail {

/// Runs the task created by \c make_task to completion, with \c run_loop_until running the loop until it completes.
template <typename MakeTask, typename RunLoopUntil>
auto sync_wait_impl(MakeTask&& make_task, const RunLoopUntil& run_loop_until)
{
  using task_type   = std::invoke_result_t<MakeTask, task_executor&>;
  using result_type = typename task_type::result_type;

  run_loop  loop;
  task_type task = std::forward<MakeTask>(make_task)(loop);

  if constexpr (std::is_void_v<result_type>) {
    eager_async_task<void> waiter = launch_async([&task](coro_context<eager_async_task<void>>& ctx) {
      CORO_BEGIN(ctx);
      CORO_AWAIT(task);
      CORO_RETURN();
    });
    run_loop_until(loop, [&waiter]() { return waiter.ready(); });
  } else {
    std::optional<result_type> result;
    eager_async_task<void>     waiter = launch_async([&task, &result](coro_context<eager_async_task<void>>& ctx) {
      CORO_BEGIN(ctx);
      CORO_AWAIT_VALUE(result, task);
      CORO_RETURN();
    });
    run_loop_until(loop, [&waiter]() { return waiter.ready(); });
    return std::move(*result);
  }
}

} // namespace detail

/// \brief Blocks the calling thread until an asynchronous task completes, and returns its result.
///
/// The calling thread runs a \c run_loop while it waits. The task must complete on that loop, which is passed to
/// \c make_task as the continuation executor. It must not be called from an executor that the task needs.
/// \param make_task Callable with signature \c async_task<T>(task_executor&).
/// \return The result of the task.
template <typename MakeTask>
auto sync_wait(MakeTask&& make_task)
{
  return detail::sync_wait_impl(std::forward<MakeTask>(make_task),
                                [](run_loop& loop, const auto& pred) { loop.run_until(pred); });
}

/// \brief Blocks the calling thread until an asynchronous task completes, and returns its result, calling an idle
/// handler whenever the loop has no tasks to run.
///
/// The calling thread runs a \c run_loop while it waits, as in \c sync_wait(MakeTask&&). The loop does not block while
/// there are no tasks, so the idle handler should block or sleep for a while.
/// \param make_task Callable with signature \c async_task<T>(task_executor&).
/// \param on_idle   Callable with signature \c void().
/// \return The result of the task.
template <typename MakeTask, typename IdleHandler>
auto sync_wait(MakeTask&& make_task, IdleHandler&& on_idle)
{
  return detail::sync_wait_impl(std::forward<MakeTask>(make_task),
                                [&on_idle](run_loop& loop, const auto& pred) { loop.run_until(pred, on_idle); });
}

} // namespace ocudu
