// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "ocudu/support/executors/manual_task_worker.h"
#include <gtest/gtest.h>

using namespace ocudu;

static constexpr size_t queue_size = 2;

TEST(manual_task_worker_test, stop_requested_on_full_queue_runs_accepted_tasks_before_stopping)
{
  manual_task_worker worker{queue_size};
  unsigned           nof_runs = 0;
  for (unsigned i = 0; i != queue_size; ++i) {
    ASSERT_TRUE(worker.defer([&nof_runs]() { ++nof_runs; }));
  }

  worker.request_stop();
  ASSERT_FALSE(worker.is_stopped());

  for (unsigned i = 0; i != queue_size; ++i) {
    ASSERT_TRUE(worker.run_next_blocking());
  }
  ASSERT_EQ(nof_runs, queue_size);
  ASSERT_FALSE(worker.run_next_blocking());
  ASSERT_TRUE(worker.is_stopped());
}

TEST(manual_task_worker_test, run_pending_tasks_stops_after_draining_when_stop_requested_on_full_queue)
{
  manual_task_worker worker{queue_size};
  unsigned           nof_runs = 0;
  for (unsigned i = 0; i != queue_size; ++i) {
    ASSERT_TRUE(worker.defer([&nof_runs]() { ++nof_runs; }));
  }

  worker.request_stop();
  ASSERT_TRUE(worker.run_pending_tasks());
  ASSERT_EQ(nof_runs, queue_size);
  ASSERT_TRUE(worker.is_stopped());
}

TEST(manual_task_worker_test, stop_requested_on_free_queue_runs_earlier_tasks_before_stopping)
{
  manual_task_worker worker{queue_size};
  unsigned           nof_runs = 0;
  ASSERT_TRUE(worker.defer([&nof_runs]() { ++nof_runs; }));

  worker.request_stop();
  ASSERT_TRUE(worker.run_next_blocking());
  ASSERT_EQ(nof_runs, 1);
  ASSERT_TRUE(worker.run_next_blocking());
  ASSERT_TRUE(worker.is_stopped());
  ASSERT_FALSE(worker.run_next_blocking());
}
