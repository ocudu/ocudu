// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "ocudu/support/executors/inline_task_executor.h"
#include "ocudu/support/executors/manual_task_worker.h"
#include "ocudu/support/executors/task_worker.h"
#include "ocudu/support/io/io_broker_factory.h"
#include "ocudu/support/io/io_timer_source.h"
#include "ocudu/support/timers.h"
#include <gtest/gtest.h>
#include <optional>

using namespace ocudu;

class io_timer_source_test : public ::testing::Test
{
public:
  manual_task_worker         worker{16};
  timer_manager              timers{16};
  std::unique_ptr<io_broker> broker = create_io_broker(io_broker_type::epoll);

  ~io_timer_source_test() override { stop(); }

  void start() { source.emplace(timers, *broker, worker, std::chrono::milliseconds{1}); }

  void stop()
  {
    // We need to stop from a different thread than the one ticking.
    std::atomic<bool> closed{false};
    task_worker       stopper_helper{"STOP_WORKER", 128};
    stopper_helper.push_task_blocking([this, &closed]() {
      source.reset();
      closed = true;
    });

    while (not closed) {
      worker.run_pending_tasks();
      std::this_thread::sleep_for(std::chrono::milliseconds{1});
    }
  }

  std::optional<io_timer_source> source;
};

TEST_F(io_timer_source_test, timer_gets_ticked_when_source_starts)
{
  std::chrono::milliseconds run_duration{100};
  std::chrono::milliseconds timer_period{5};

  unique_timer          t = timers.create_unique_timer(worker);
  std::atomic<unsigned> count{0};
  t.set(timer_period, [&count, &t]() {
    count++;
    t.run();
  });
  t.run();

  // The io timer source is not yet running.
  std::this_thread::sleep_for(std::chrono::milliseconds{10});
  worker.run_pending_tasks();
  ASSERT_EQ(count, 0);

  start();
  for (unsigned i = 0; i != run_duration.count(); i++) {
    std::this_thread::sleep_for(std::chrono::milliseconds{1});
    worker.run_pending_tasks();
  }
  ASSERT_GT(count, 1);
  fmt::print("Tick count: expected={} actual={}\n", run_duration / timer_period, count.load());

  // Stop io timer source.
  stop();
  worker.run_pending_tasks(); // ensures no pending tasks before we reset count

  count = 0;
  std::this_thread::sleep_for(std::chrono::milliseconds{10});
  worker.run_pending_tasks();
  ASSERT_EQ(count, 0);
}
