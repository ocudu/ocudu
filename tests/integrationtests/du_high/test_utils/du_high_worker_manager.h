// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/du/du_high/du_high_executor_mapper.h"
#include "ocudu/support/executors/manual_task_worker.h"
#include "ocudu/support/executors/task_worker_pool.h"
#include "ocudu/support/synchronization/sync_event.h"
#include <array>

namespace ocudu {

struct du_high_worker_manager {
  static const uint32_t task_worker_queue_size = 10000;

  du_high_worker_manager();
  ~du_high_worker_manager();
  void stop();
  void flush_pending_dl_pdus();
  void flush_pending_control_tasks();

  manual_task_worker        test_worker{task_worker_queue_size};
  priority_task_worker_pool worker_pool{
      "POOL",
      3,
      std::array<concurrent_queue_params, 2>{
          concurrent_queue_params{concurrent_queue_policy::lockfree_mpmc, task_worker_queue_size},
          concurrent_queue_params{concurrent_queue_policy::lockfree_mpmc, task_worker_queue_size}}};
  priority_task_worker_pool_executor            high_prio_exec{enqueue_priority::max, worker_pool};
  priority_task_worker_pool_executor            low_prio_exec{enqueue_priority::max - 1, worker_pool};
  std::unique_ptr<odu::du_high_executor_mapper> exec_mapper;
  std::unique_ptr<task_executor>                time_exec;

private:
  sync_event ev;
};

} // namespace ocudu
