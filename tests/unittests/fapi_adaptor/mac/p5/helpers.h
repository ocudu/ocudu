// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "p5_transaction_outcome_manager.h"
#include "ocudu/fapi/p5/p5_requests_gateway.h"
#include "ocudu/ocudulog/ocudulog.h"
#include "ocudu/support/async/async_task.h"
#include "ocudu/support/async/eager_async_task.h"
#include "ocudu/support/error_handling.h"
#include "ocudu/support/executors/task_worker.h"
#include "ocudu/support/synchronization/sync_event.h"
#include "ocudu/support/timers.h"
#include <atomic>
#include <chrono>
#include <functional>
#include <gtest/gtest.h>
#include <optional>
#include <thread>

namespace ocudu {
namespace unittest {

/// Launches the given task and lets another thread wait until it has finished.
template <typename R>
struct waitable_task_launcher : public eager_async_task<R> {
  explicit waitable_task_launcher(async_task<R>& task_) : task(task_)
  {
    *static_cast<eager_async_task<R>*>(this) =
        launch_async([this, token = event.get_token()](coro_context<eager_async_task<R>>& ctx) {
          CORO_BEGIN(ctx);
          CORO_AWAIT_VALUE(result, task);
          done.store(true, std::memory_order_release);
          CORO_RETURN(*result);
        });
  }

  /// Blocks the caller until the task has finished.
  void wait_for_coroutine_result() { event.wait(); }

  /// Returns true when the task has finished. It can be called from any thread.
  bool finished() const { return done.load(std::memory_order_acquire); }

  /// Result of the task, empty while it has not finished.
  std::optional<R> result;

private:
  async_task<R>&    task;
  std::atomic<bool> done = false;
  sync_event        event;
};

/// \brief Spy of the P5 requests gateway.
///
/// It records the requests that it receives and, for each of them, it completes the matching transaction with the reply
/// that it was given. A request without a reply is left unanswered, which makes its transaction time out.
class p5_requests_gateway_spy : public fapi::p5_requests_gateway
{
public:
  /// Replies to the P5 requests. A \c true value is a successful response and \c false an error one.
  struct replies {
    std::optional<bool> param;
    std::optional<bool> config;
    std::optional<bool> stop;
    std::optional<bool> start;
  };

  p5_requests_gateway_spy(const replies& replies_, fapi_adaptor::p5_transaction_outcome_manager& transaction_manager_) :
    reply(replies_), transaction_manager(transaction_manager_)
  {
  }

  void send_param_request(const fapi::param_request& msg) override
  {
    param_request_sent = true;
    if (reply.param) {
      transaction_manager.param_response_outcome.set(*reply.param);
    }
  }

  void send_config_request(const fapi::config_request& msg) override
  {
    config_request_sent = true;
    if (reply.config) {
      transaction_manager.config_response_outcome.set(*reply.config);
    }
  }

  void send_stop_request(const fapi::stop_request& msg) override
  {
    stop_request_sent = true;
    if (reply.stop) {
      transaction_manager.stop_response_outcome.set(*reply.stop);
    }
  }

  void send_start_request(const fapi::start_request& msg) override
  {
    start_request_sent = true;
    if (reply.start) {
      transaction_manager.start_outcome.set(*reply.start);
    }
  }

  bool has_param_request_been_sent() const { return param_request_sent; }
  bool has_config_request_been_sent() const { return config_request_sent; }
  bool has_stop_request_been_sent() const { return stop_request_sent; }
  bool has_start_request_been_sent() const { return start_request_sent; }

private:
  const replies                                 reply;
  fapi_adaptor::p5_transaction_outcome_manager& transaction_manager;
  std::atomic<bool>                             param_request_sent  = false;
  std::atomic<bool>                             config_request_sent = false;
  std::atomic<bool>                             stop_request_sent   = false;
  std::atomic<bool>                             start_request_sent  = false;
};

/// \brief Base fixture of the tests of the MAC-FAPI P5 procedures.
///
/// It provides the executors, the timers and the transaction manager of a procedure, and runs it in the MAC control
/// executor. The timers are not driven by the clock but by the test, so that a timeout does not take any real time.
class mac_fapi_p5_procedure_fixture : public ::testing::Test
{
  static constexpr auto queue_policy = concurrent_queue_policy::locking_mpsc;
  static constexpr auto wait_policy  = concurrent_queue_wait_policy::condition_variable;
  using task_worker                  = general_task_worker<queue_policy, wait_policy>;
  using task_worker_executor         = general_task_worker_executor<queue_policy, wait_policy>;

protected:
  mac_fapi_p5_procedure_fixture() :
    fapi_worker("fapi_exec_test", 32),
    fapi_executor(fapi_worker),
    mac_worker("mac_exec_test", 32),
    mac_executor(mac_worker),
    transaction_manager(timer_factory{timers, fapi_executor})
  {
  }

  ~mac_fapi_p5_procedure_fixture() override
  {
    if (!proc_launcher) {
      return;
    }
    // A procedure that has not finished, because the test ended early, still runs in the workers. Its transactions are
    // made to time out so that it finishes before the members it uses are destroyed.
    while (!proc_launcher->finished()) {
      timers.tick();
      std::this_thread::yield();
    }
    wait_procedure_to_finish();
  }

  /// Timeout of the transactions of the procedures.
  static constexpr std::chrono::milliseconds timeout{100};

  /// Creates the gateway spy that answers the requests as the given replies describe.
  void create_gateway(const p5_requests_gateway_spy::replies& replies)
  {
    gateway_spy = std::make_unique<p5_requests_gateway_spy>(replies, transaction_manager);
  }

  /// Returns the dependencies of a procedure, which are the same for all of them.
  template <typename ProcedureDependencies>
  ProcedureDependencies make_dependencies()
  {
    return ProcedureDependencies{.logger              = ocudulog::fetch_basic_logger("TEST"),
                                 .p5_gateway          = *gateway_spy,
                                 .transaction_manager = transaction_manager,
                                 .mac_ctrl_executor   = mac_executor,
                                 .fapi_ctrl_executor  = fapi_executor,
                                 .timers              = timers};
  }

  /// Launches the task that the given function creates in the MAC control executor, and returns once it has started.
  void launch_procedure(std::function<async_task<bool>()> create_procedure)
  {
    // The destructor of the event blocks until the deferred task, which holds its token, has run.
    sync_event started;

    bool launched = mac_executor.defer([this, token = started.get_token(), create = std::move(create_procedure)]() {
      proc = create();
      proc_launcher.emplace(proc);
    });
    report_fatal_error_if_not(launched, "The procedure could not be launched in the MAC control executor");
  }

  /// Waits until the procedure has finished.
  void wait_procedure_to_finish()
  {
    proc_launcher->wait_for_coroutine_result();
    mac_worker.wait_pending_tasks();
    fapi_worker.wait_pending_tasks();
  }

  /// \brief Makes the transaction of the given request time out and waits until the procedure has finished.
  ///
  /// The transaction timer is started before its request is sent, so it is running once the request is out. The test
  /// then advances the timers up to the tick before the timeout, checks that the procedure is still waiting, and
  /// advances them by the last tick.
  ::testing::AssertionResult wait_procedure_to_time_out_after(const std::function<bool()>& is_request_sent)
  {
    static constexpr std::chrono::seconds max_wait_time{5};

    auto deadline = std::chrono::steady_clock::now() + max_wait_time;
    while (!is_request_sent()) {
      if (std::chrono::steady_clock::now() > deadline) {
        return ::testing::AssertionFailure() << "The request has not been sent";
      }
      std::this_thread::yield();
    }

    tick_timers(timeout.count() - 1);
    fapi_worker.wait_pending_tasks();
    mac_worker.wait_pending_tasks();
    if (proc_launcher->finished()) {
      return ::testing::AssertionFailure() << "The procedure finished before the transaction timed out";
    }

    tick_timers(1);
    wait_procedure_to_finish();
    return ::testing::AssertionSuccess();
  }

  /// Returns true when the procedure has finished with the given result.
  bool procedure_finished_with(bool expected_result) const
  {
    return proc_launcher && proc_launcher->finished() && *proc_launcher->result == expected_result;
  }

  /// Advances the timers by the given number of ticks.
  void tick_timers(unsigned nof_ticks)
  {
    for (unsigned i = 0; i != nof_ticks; ++i) {
      timers.tick();
    }
  }

  timer_manager                                timers;
  task_worker                                  fapi_worker;
  task_worker_executor                         fapi_executor;
  task_worker                                  mac_worker;
  task_worker_executor                         mac_executor;
  fapi_adaptor::p5_transaction_outcome_manager transaction_manager;
  std::unique_ptr<p5_requests_gateway_spy>     gateway_spy;
  async_task<bool>                             proc;
  std::optional<waitable_task_launcher<bool>>  proc_launcher;
};

} // namespace unittest
} // namespace ocudu
