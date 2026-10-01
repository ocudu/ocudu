// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../du_ue/du_ue_manager.h"
#include "ocudu/adt/scope_exit.h"
#include "ocudu/ran/du_types.h"
#include "ocudu/support/async/async_task.h"
#include "ocudu/support/async/manual_event.h"
#include <vector>

namespace ocudu {
namespace odu {

namespace detail {

/// Helper task to run a coroutine for different UEs and wait for all of them to complete.
template <typename TaskFactory>
class await_all_ues_impl
{
public:
  await_all_ues_impl(du_ue_manager_repository& ue_mng_,
                     span<const du_ue_index_t> ues_to_update_,
                     TaskFactory               ue_task_factory_) :
    ue_mng(ue_mng_),
    ue_task_factory(std::move(ue_task_factory_)),
    ues_to_update(ues_to_update_.begin(), ues_to_update_.end())
  {
  }

  void operator()(coro_context<async_task<void>>& ctx)
  {
    CORO_BEGIN(ctx);

    // Launch tasks in the respective UEs task schedulers.
    launch_ue_tasks();

    // Await all tasks to complete.
    CORO_AWAIT(all_ue_tasks_completed);

    CORO_RETURN();
  }

private:
  void launch_ue_tasks()
  {
    // Check how many UEs need to be updated.
    remaining_ues = ues_to_update.size();
    if (remaining_ues == 0) {
      // Fast path: No UEs to update. Flag completion.
      all_ue_tasks_completed.set();
      return;
    }

    const unsigned nof_ues = ues_to_update.size();
    for (unsigned ue_count = 0; ue_count != nof_ues; ++ue_count) {
      ue_mng.schedule_async_task(ues_to_update[ue_count], launch_ue_task(ues_to_update[ue_count]));
    }
  }

  async_task<void> launch_ue_task(du_ue_index_t ue_index)
  {
    // RAII object to decrement remaining UEs and flag completion if this is the last one.
    auto dec_remaining_ues = make_scope_exit([this]() {
      remaining_ues--;
      if (remaining_ues == 0) {
        // This is the last task to remove UEs. Flag completion.
        all_ue_tasks_completed.set();
      }
    });

    return launch_async(
        [this, ue_index, on_finish = std::move(dec_remaining_ues)](coro_context<async_task<void>>& ctx) {
          CORO_BEGIN(ctx);

          if (cur_ue = ue_mng.find_ue(ue_index); cur_ue != nullptr) {
            CORO_AWAIT(ue_task_factory(*cur_ue));
          }

          CORO_RETURN();
        });
  }

  du_ue_manager_repository& ue_mng;
  TaskFactory               ue_task_factory;

  std::vector<du_ue_index_t> ues_to_update;

  unsigned          remaining_ues = 0;
  manual_event_flag all_ue_tasks_completed;
  du_ue*            cur_ue = nullptr;
};

} // namespace detail

/// \brief Runs an async_task in the respective task scheduler of a list of UEs.
template <typename AsyncTaskFactory>
auto await_all_ues(du_ue_manager_repository& ue_mng,
                   span<const du_ue_index_t> ues_to_update,
                   AsyncTaskFactory          ue_task_factory) -> async_task<void>
{
  return launch_async<detail::await_all_ues_impl<AsyncTaskFactory>>(ue_mng, ues_to_update, std::move(ue_task_factory));
}

/// \brief Runs an async_task in the respective task scheduler of all UEs.
template <typename AsyncTaskFactory>
auto await_all_ues(du_ue_manager& ue_mng, AsyncTaskFactory ue_task_factory) -> async_task<void>
{
  // Fill list of UEs to update.
  const auto&                ue_db = ue_mng.get_du_ues();
  std::vector<du_ue_index_t> ues_to_update;
  ues_to_update.reserve(ue_db.size());
  for (const auto& u : ue_db) {
    ues_to_update.push_back(u.ue_index);
  }
  return await_all_ues(ue_mng, ues_to_update, std::forward<AsyncTaskFactory>(ue_task_factory));
}

} // namespace odu
} // namespace ocudu
