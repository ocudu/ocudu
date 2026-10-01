// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/support/async/async_queue.h"
#include "ocudu/support/async/async_task_scheduler.h"
#include "ocudu/support/async/eager_async_task.h"
#include "ocudu/support/async/event_sender_receiver.h"

namespace ocudu {

/// Asynchronous task that sequentially runs other enqueued asynchronous tasks.
class fifo_async_task_scheduler final : public async_task_scheduler
{
public:
  explicit fifo_async_task_scheduler(size_t queue_size) : queue(queue_size) { run(); }

  fifo_async_task_scheduler(const fifo_async_task_scheduler&)            = delete;
  fifo_async_task_scheduler& operator=(const fifo_async_task_scheduler&) = delete;

  bool schedule(async_task<void> t) override { return queue.try_push(std::move(t)); }

  template <typename AsyncTask, typename... Args>
  bool schedule(Args&&... args)
  {
    return queue.try_push(launch_async<AsyncTask>(std::forward<Args>(args)...));
  }

  template <typename AsyncFunc>
  bool schedule(AsyncFunc&& async_func)
  {
    return queue.try_push(launch_async(std::forward<AsyncFunc>(async_func)));
  }

  void clear_pending_tasks() { queue.clear(); }

  eager_async_task<void> request_stop()
  {
    // Enqueue task in case main loop is waiting for new procedure.
    running = false;
    queue.clear();
    queue.try_push(launch_async([](coro_context<async_task<void>>& ctx) {
      CORO_BEGIN(ctx);
      CORO_RETURN();
    }));
    return std::move(loop_task);
  }

  size_t nof_pending_tasks() const { return queue.size(); }

  bool empty() const { return queue.size() == 0; }

  bool is_stopped() const { return loop_task.empty(); }

private:
  void run()
  {
    loop_task = launch_async([this](coro_context<eager_async_task<void>>& ctx) {
      CORO_BEGIN(ctx);

      // Runs until requested to stop.
      while (running) {
        // Wait for new procedure to be enqueued.
        CORO_AWAIT_VALUE(next_task, queue);

        // Await for popped task to complete.
        CORO_AWAIT(std::move(next_task));
      }

      CORO_RETURN();
    });
  }

  bool                          running = true;
  async_queue<async_task<void>> queue;
  eager_async_task<void>        loop_task;
  async_task<void>              next_task;
};

/// \brief Launches an asynchronous task on the given task sequencer and returns an async task that is only complete
/// when the former is complete.
///
/// This function is useful to synchronize two procedures running in separate task schedulers.
template <typename Callback,
          typename ReturnType = detail::function_return_t<decltype(&std::decay_t<Callback>::operator())>,
          std::enable_if_t<std::is_same_v<ReturnType, void>, int> = 0>
[[nodiscard]] async_task<bool> when_completed_on_task_sched(fifo_async_task_scheduler& task_sched,
                                                            Callback&&                 task_to_run)
{
  struct task_offloader {
    task_offloader(fifo_async_task_scheduler& task_sched_, Callback&& callback_) :
      task_sched(task_sched_), callback(std::forward<Callback>(callback_))
    {
    }

    void operator()(coro_context<async_task<bool>>& ctx)
    {
      CORO_BEGIN(ctx);

      task_sched.schedule(dispatched_task());

      CORO_AWAIT_VALUE(const bool result, rx);

      CORO_RETURN(result);
    }

  private:
    async_task<void> dispatched_task()
    {
      return launch_async([this, tx = rx.get_sender()](coro_context<async_task<void>>& ctx) mutable {
        CORO_BEGIN(ctx);

        callback();

        tx.set();

        CORO_RETURN();
      });
    }

    fifo_async_task_scheduler& task_sched;
    Callback                   callback;
    event_receiver<void>       rx;
  };

  return launch_async<task_offloader>(task_sched, std::forward<Callback>(task_to_run));
}

template <typename Callback,
          typename ReturnType = detail::function_return_t<decltype(&std::decay_t<Callback>::operator())>,
          std::enable_if_t<not std::is_same_v<ReturnType, void>, int> = 0>
[[nodiscard]] async_task<std::optional<ReturnType>> when_completed_on_task_sched(fifo_async_task_scheduler& task_sched,
                                                                                 Callback&&                 task_to_run)
{
  struct task_offloader {
    task_offloader(fifo_async_task_scheduler& task_sched_, Callback&& callback_) :
      task_sched(task_sched_), callback(std::forward<Callback>(callback_))
    {
    }

    void operator()(coro_context<async_task<std::optional<ReturnType>>>& ctx)
    {
      CORO_BEGIN(ctx);

      task_sched.schedule(dispatched_task());

      CORO_AWAIT_VALUE(std::optional<ReturnType> result, rx);

      CORO_RETURN(result);
    }

  private:
    async_task<void> dispatched_task()
    {
      return launch_async([this, tx = rx.get_sender()](coro_context<async_task<void>>& ctx) mutable {
        CORO_BEGIN(ctx);

        tx.set(callback());

        CORO_RETURN();
      });
    }

    fifo_async_task_scheduler& task_sched;
    Callback                   callback;
    event_receiver<ReturnType> rx;
  };

  return launch_async<task_offloader>(task_sched, std::forward<Callback>(task_to_run));
}

/// \brief Launches a coroutine on the given task sequencer and returns an async task that is only complete when the
/// former is complete.
///
/// This function is useful to synchronize two procedures running in separate task schedulers.
/// \tparam AsyncTask  Type of the coroutine to run in the provided task scheduler.
/// \param task_sched  Task scheduler to which switch the execution too.
/// \param task_to_run Task to run in the task scheduler.
/// \return            Returns and async_task<bool>/async_task<std::optional<R>> that can be awaited on.
template <typename AsyncTask>
[[nodiscard]] auto when_coroutine_completed_on_task_sched(fifo_async_task_scheduler& task_sched,
                                                          AsyncTask&&                task_to_run)
{
  using task_return_type = detail::awaitable_result_t<AsyncTask>;

  if constexpr (std::is_same_v<task_return_type, void>) {
    // Return void case.
    using return_type = async_task<expected<default_success_t>>;

    struct task_offloader {
      task_offloader(fifo_async_task_scheduler& task_sched_, AsyncTask&& routine_) :
        task_sched(task_sched_), task_to_run(std::forward<AsyncTask>(routine_))
      {
      }

      void operator()(coro_context<return_type>& ctx)
      {
        CORO_BEGIN(ctx);
        task_sched.schedule(dispatched_task());
        CORO_AWAIT_VALUE(const bool result, rx);
        if (result) {
          CORO_EARLY_RETURN(default_success_t{});
        }
        CORO_RETURN(make_unexpected(default_error_t{}));
      }

    private:
      async_task<void> dispatched_task()
      {
        return launch_async([this, tx = rx.get_sender()](coro_context<async_task<void>>& ctx) mutable {
          CORO_BEGIN(ctx);
          CORO_AWAIT(task_to_run);
          tx.set();
          CORO_RETURN();
        });
      }

      fifo_async_task_scheduler& task_sched;
      AsyncTask                  task_to_run;
      event_receiver<void>       rx;
    };

    return launch_async<task_offloader>(task_sched, std::forward<AsyncTask>(task_to_run));

  } else {
    // Non-void return case.
    using return_type = async_task<expected<task_return_type>>;

    struct task_offloader {
      task_offloader(fifo_async_task_scheduler& task_sched_, AsyncTask&& routine_) :
        task_sched(task_sched_), task_to_run(std::forward<AsyncTask>(routine_))
      {
      }

      void operator()(coro_context<return_type>& ctx)
      {
        CORO_BEGIN(ctx);
        task_sched.schedule(dispatched_task());
        CORO_AWAIT_VALUE(auto result, rx);
        if (result.has_value()) {
          CORO_EARLY_RETURN(*result);
        }
        CORO_RETURN(make_unexpected(default_error_t{}));
      }

    private:
      async_task<void> dispatched_task()
      {
        return launch_async([this, tx = rx.get_sender()](coro_context<async_task<void>>& ctx) mutable {
          CORO_BEGIN(ctx);
          CORO_AWAIT_VALUE(auto ret, task_to_run);
          tx.set(ret);
          CORO_RETURN();
        });
      }

      fifo_async_task_scheduler&       task_sched;
      AsyncTask                        task_to_run;
      event_receiver<task_return_type> rx;
    };

    return launch_async<task_offloader>(task_sched, std::forward<AsyncTask>(task_to_run));
  }
}

} // namespace ocudu
