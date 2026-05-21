// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ue_manager.h"
#include "ocudu/support/async/execute_on_blocking.h"

using namespace ocudu;
using namespace ocuup;

ue_manager::ue_manager(const ue_manager_config& config, const ue_manager_dependencies& dependencies) :
  max_nof_ues(config.max_nof_ues),
  n3_config(config.n3_config),
  test_mode_config(config.test_mode_config),
  e1aps(dependencies.e1aps),
  f1u_gw(dependencies.f1u_gw),
  ngu_session_mngr(dependencies.ngu_session_mngr),
  cu_up_mngr_pdcp_if(dependencies.cu_up_mngr_pdcp_if),
  gtpu_rx_demux(dependencies.gtpu_rx_demux),
  n3_teid_allocator(dependencies.n3_teid_allocator),
  f1u_teid_allocator(dependencies.f1u_teid_allocator),
  exec_pool(dependencies.exec_pool),
  ctrl_executor(exec_pool.ctrl_executor()),
  gtpu_pcap(dependencies.gtpu_pcap),
  timers(dependencies.timers),
  logger(dependencies.logger)
{
  // Initialize a ue task schedulers for all UE indexes.
  ue_task_schedulers.reserve(config.max_nof_ues);
  for (size_t i = 0; i < config.max_nof_ues; ++i) {
    ue_task_schedulers.emplace_back(std::make_unique<fifo_async_task_scheduler>(UE_TASK_QUEUE_SIZE));
  }
}

async_task<void> ue_manager::stop()
{
  // Stopping all UE's. Stop GTP-U demux first.
  gtpu_rx_demux.stop();

  // Stop F1-U gateway. This will internallly stop the GTP-U demux
  // and UDP TX batched queues, if in split CU/DU scenarios.
  f1u_gw.stop();

  // Routine to stop all UEs
  return remove_all_ues();
}

async_task<void> ue_manager::remove_all_ues()
{
  // Routine to stop all UEs
  auto ue_it = ue_db.begin();
  return launch_async([this, ue_it](coro_context<async_task<void>>& ctx) mutable {
    CORO_BEGIN(ctx);

    // Remove all UEs.
    while (ue_it != ue_db.end()) {
      CORO_AWAIT(schedule_and_wait_ue_removal((ue_it++)->first));
    }

    CORO_RETURN();
  });
}

async_task<void> ue_manager::remove_e1_ues(cu_up_e1_index_t e1_index)
{
  // Routine to stop all UEs
  auto ue_it = ue_db.begin();
  return launch_async([this, ue_it, e1_index](coro_context<async_task<void>>& ctx) mutable {
    CORO_BEGIN(ctx);

    // Remove all UEs.
    while (ue_it != ue_db.end()) {
      if (ue_it->second->get_e1_index() == e1_index) {
        CORO_AWAIT(schedule_and_wait_ue_removal((ue_it++)->first));
      } else {
        ue_it++;
      }
    }

    CORO_RETURN();
  });
}

ue_context* ue_manager::find_ue(cu_up_ue_index_t ue_index)
{
  ocudu_assert(ue_index < max_nof_ues, "Invalid ue_index={}", fmt::underlying(ue_index));
  return ue_db.find(ue_index) != ue_db.end() ? ue_db[ue_index].get() : nullptr;
}

ue_context* ue_manager::add_ue(cu_up_e1_index_t e1_index, const ue_context_cfg& ue_cfg)
{
  if (ue_db.size() >= max_nof_ues) {
    logger.error("Can't add new UE. Max number of UEs reached.");
    return nullptr;
  }

  // Find E1AP for this bearer context.
  if (cu_up_e1_index_to_uint(e1_index) >= e1aps.size()) {
    return nullptr;
  }
  std::reference_wrapper<e1ap_interface> e1ap = e1aps[cu_up_e1_index_to_uint(e1_index)];

  cu_up_ue_index_t new_idx = get_next_ue_index();
  if (new_idx == INVALID_CU_UP_UE_INDEX) {
    logger.error("No free ue_index available");
    return nullptr;
  }

  // Create UE executors
  // TODO, these should be created within the same function, so that UL, DL and CTRL executors
  // can point to the same executor.
  std::unique_ptr<ue_executor_mapper> ue_exec_mapper = exec_pool.create_ue_executor_mapper();

  // Create executor-specific timer factories
  timer_factory ue_dl_timer_factory   = {timers, ue_exec_mapper->dl_pdu_executor()};
  timer_factory ue_ul_timer_factory   = {timers, ue_exec_mapper->ul_pdu_executor()};
  timer_factory ue_ctrl_timer_factory = {timers, ue_exec_mapper->ctrl_executor()};

  // Create UE object
  std::unique_ptr<ue_context> new_ctx =
      std::make_unique<ue_context>(new_idx,
                                   ue_cfg,
                                   n3_config,
                                   test_mode_config,
                                   ue_context_dependencies{e1ap,
                                                           std::move(ue_exec_mapper),
                                                           *ue_task_schedulers[new_idx],
                                                           ue_dl_timer_factory,
                                                           ue_ul_timer_factory,
                                                           ue_ctrl_timer_factory,
                                                           f1u_gw,
                                                           ngu_session_mngr,
                                                           cu_up_mngr_pdcp_if,
                                                           n3_teid_allocator,
                                                           f1u_teid_allocator,
                                                           gtpu_rx_demux,
                                                           gtpu_pcap});

  // add to DB
  ue_db.emplace(new_idx, std::move(new_ctx));
  return ue_db[new_idx].get();
}

async_task<void> ue_manager::remove_ue(cu_up_ue_index_t ue_index)
{
  logger.debug("ue={}: Scheduling UE deletion", fmt::underlying(ue_index));
  ocudu_assert(
      ue_db.find(ue_index) != ue_db.end(), "Remove UE called for nonexistent ue_index={}", fmt::underlying(ue_index));

  // Move UE context out from ue_db and erase the slot (from CU-UP shared ctrl executor)
  std::unique_ptr<ue_context> ue_ctxt = std::move(ue_db[ue_index]);
  ue_db.erase(ue_index);

  // Dispatch the stopping and deletion of the UE context to UE-specific ctrl executor
  return launch_async([this, ue_ctxt = std::move(ue_ctxt)](coro_context<async_task<void>>& ctx) mutable {
    CORO_BEGIN(ctx);

    CORO_AWAIT(ue_ctxt->stop(ctrl_executor, timers));

    CORO_RETURN();
  });
}

cu_up_ue_index_t ue_manager::get_next_ue_index()
{
  // Search unallocated UE index
  for (unsigned i = 0; i < max_nof_ues; i++) {
    if (ue_db.find(static_cast<cu_up_ue_index_t>(i)) == ue_db.end()) {
      return int_to_ue_index(i);
      break;
    }
  }
  return INVALID_CU_UP_UE_INDEX;
}

void ue_manager::schedule_ue_async_task(cu_up_ue_index_t ue_index, async_task<void> task)
{
  ue_context* ue_ctx = find_ue(ue_index);
  if (ue_ctx == nullptr) {
    logger.error("Cannot schedule UE task, could not find UE. ue_index={}", fmt::underlying(ue_index));
    return;
  }
  ue_ctx->task_sched.schedule(std::move(task));
}

async_task<expected<>> ue_manager::schedule_and_wait_ue_removal(cu_up_ue_index_t ue_index)
{
  ue_context* ue_ctx = find_ue(ue_index);
  if (ue_ctx == nullptr) {
    logger.error("Cannot schedule UE removal, could not find UE. ue_index={}", fmt::underlying(ue_index));
    return launch_async([](coro_context<async_task<expected<>>>& ctx) mutable {
      CORO_BEGIN(ctx);
      CORO_RETURN(make_unexpected(default_error_t{}));
    });
  }

  auto t = launch_async([this, ue_index](coro_context<async_task<void>>& ctx) mutable {
    CORO_BEGIN(ctx);
    CORO_AWAIT(remove_ue(ue_index));
    CORO_RETURN();
  });

  return when_coroutine_completed_on_task_sched(ue_ctx->task_sched, std::move(t));
}
