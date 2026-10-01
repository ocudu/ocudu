// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ue_task_scheduler_impl.h"
#include "ocudu/ran/cu_cp_types.h"

using namespace ocudu;
using namespace ocucp;

/// Maximum number of pending procedures per UE.
static constexpr size_t number_of_pending_ue_procedures = 64;

unique_timer ue_task_scheduler_impl::create_timer()
{
  ocudu_assert(parent != nullptr, "UE task scheduler not set");
  return parent->make_unique_timer();
}

timer_factory ue_task_scheduler_impl::get_timer_factory()
{
  return timer_factory{parent->timers, parent->exec};
}

task_executor& ue_task_scheduler_impl::get_executor()
{
  return parent->exec;
}

void ue_task_scheduler_impl::stop()
{
  if (parent != nullptr) {
    parent->rem_ue_task_loop(ue_sched->first);
    parent = nullptr;
  }
}

ue_task_scheduler_manager::ue_task_scheduler_manager(timer_manager&          timers_,
                                                     task_executor&          exec_,
                                                     ocudulog::basic_logger& logger_) :
  timers(timers_), exec(exec_), logger(logger_), ues_to_rem(1024)
{
}

void ue_task_scheduler_manager::stop()
{
  for (auto& p : ue_ctrl_loop) {
    auto ctrl_loop = p.second->request_stop();

    // let loop go out of scope.
  }
  ues_to_rem.request_stop();
}

ue_task_scheduler_impl ue_task_scheduler_manager::create_ue_task_sched(cu_cp_ue_index_t ue_idx)
{
  if (ue_ctrl_loop.find(ue_idx) != ue_ctrl_loop.end()) {
    logger.error("ue={}: UE task scheduler already exists", ue_idx);
    return {};
  }
  auto ret = ue_ctrl_loop.emplace(ue_idx, std::make_unique<fifo_async_task_scheduler>(number_of_pending_ue_procedures));
  return ue_task_scheduler_impl{*this, *ret.first};
}

void ue_task_scheduler_manager::clear_pending_tasks(cu_cp_ue_index_t ue_index)
{
  logger.debug("ue={}: Clearing pending tasks", ue_index);
  ue_ctrl_loop.at(ue_index)->clear_pending_tasks();
}

// UE task scheduler
void ue_task_scheduler_manager::handle_ue_async_task(cu_cp_ue_index_t ue_index, async_task<void>&& task)
{
  if (ue_ctrl_loop.find(ue_index) == ue_ctrl_loop.end()) {
    logger.debug("ue={}: UE task scheduler not found. UE was already removed", ue_index);
    return;
  }
  logger.debug("ue={}: Scheduling async task", ue_index);
  ue_ctrl_loop.at(ue_index)->schedule(std::move(task));
}

async_task<bool> ue_task_scheduler_manager::dispatch_and_await_task_completion(cu_cp_ue_index_t ue_index,
                                                                               unique_task      task)
{
  return when_completed_on_task_sched(*ue_ctrl_loop.at(ue_index), std::move(task));
}

unique_timer ue_task_scheduler_manager::make_unique_timer()
{
  return timers.create_unique_timer(exec);
}
timer_manager& ue_task_scheduler_manager::get_timer_manager()
{
  return timers;
}

void ue_task_scheduler_manager::rem_ue_task_loop(cu_cp_ue_index_t ue_idx)
{
  auto it = ue_ctrl_loop.find(ue_idx);
  if (it == ue_ctrl_loop.end()) {
    logger.warning("ue={}: UE task scheduler not found", ue_idx);
    return;
  }

  // Given that it might be a UE coroutine that calls the removal of the UE object, we defer the destruction of the UE
  // task scheduler to a separate task loop.
  //  eager_async_task<void> ue_task_loop = it->second.request_stop();
  bool ret =
      ues_to_rem.schedule(launch_async([ue_sched = std::move(it->second)](coro_context<async_task<void>>& ctx) mutable {
        CORO_BEGIN(ctx);

        // Cancel pending UE tasks and stop the task loop.
        CORO_AWAIT(ue_sched->request_stop());

        CORO_RETURN();
      }));
  ocudu_assert(ret, "Failed to schedule UE task loop removal");

  // Remove UE and free its index.
  ue_ctrl_loop.erase(it);
}
