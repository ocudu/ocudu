// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "cell_deactivation_routine.h"
#include "../du_processor/du_processor_repository.h"
#include "ocudu/f1ap/cu_cp/f1ap_cu_configuration_update.h"
#include "ocudu/support/async/coroutine.h"
#include <map>

using namespace ocudu;
using namespace ocudu::ocucp;

cell_deactivation_routine::cell_deactivation_routine(const cu_cp_configuration&         cu_cp_cfg_,
                                                     std::vector<cell_lifecycle_target> targets,
                                                     std::vector<cu_cp_ue_index_t>      ues_to_release_,
                                                     ngap_cause_t                       release_cause_,
                                                     du_processor_repository&           du_db_,
                                                     cu_cp_ue_context_release_handler&  ue_release_handler_,
                                                     ue_manager&                        ue_mng_,
                                                     ocudulog::basic_logger&            logger_) :
  du_db(du_db_),
  ue_release_handler(ue_release_handler_),
  ue_mng(ue_mng_),
  logger(logger_),
  ues_to_release(std::move(ues_to_release_)),
  release_cause(release_cause_)
{
  // Group the targets into a single gNB-CU Configuration Update per DU.
  std::map<cu_cp_du_index_t, f1ap_gnb_cu_configuration_update> by_du;
  for (const cell_lifecycle_target& target : targets) {
    f1ap_gnb_cu_configuration_update& update = by_du[target.du_index];
    update.gnb_cu_name                       = cu_cp_cfg_.node.ran_node_name;
    update.cells_to_be_deactivated_list.push_back({target.cgi});
  }
  for (auto& [du_index, update] : by_du) {
    du_updates.emplace_back(du_index, std::move(update));
  }
}

void cell_deactivation_routine::operator()(coro_context<async_task<bool>>& ctx)
{
  CORO_BEGIN(ctx);

  logger.info("\"{}\" started...", name());
  proc_start_tp = std::chrono::steady_clock::now();

  // Release the UEs handed over by the caller before the cells go down. The release tasks are eagerly scheduled
  // onto each UE's FIFO task scheduler in trigger_context_release(), so they all run in parallel. The loop below is
  // a no-op when the caller leaves UE handling to the DU (empty list).
  trigger_context_release();
  for (ue_release_task_it = ue_release_tasks.begin(); ue_release_task_it != ue_release_tasks.end();
       ++ue_release_task_it) {
    CORO_AWAIT(*ue_release_task_it);
  }
  logger.debug("\"{}\" all UEs released, duration: {:.3f} seconds",
               name(),
               std::chrono::duration<double>(std::chrono::steady_clock::now() - proc_start_tp).count());

  // Send one gNB-CU Configuration Update per DU listing its cells for deactivation.
  for (du_update_it = du_updates.begin(); du_update_it != du_updates.end(); ++du_update_it) {
    du_proc = du_db.find_du_processor(du_update_it->first);
    if (du_proc == nullptr) {
      logger.warning("DU processor not found for index {}", du_update_it->first);
      routine_success = false;
      continue;
    }

    CORO_AWAIT_VALUE(f1ap_cu_cfg_update_response, du_proc->handle_configuration_update(du_update_it->second));
    if (!f1ap_cu_cfg_update_response.success) {
      logger.info("Configuration update for du={} failed. Cause: {}",
                  du_update_it->first,
                  f1ap_cu_cfg_update_response.cause.has_value()
                      ? fmt::to_string(f1ap_cu_cfg_update_response.cause.value())
                      : "timeout");
      routine_success = false;
    }
  }

  logger.info("\"{}\" finished {}, duration: {:.3f} seconds",
              name(),
              routine_success ? "successfully" : "with errors",
              std::chrono::duration<double>(std::chrono::steady_clock::now() - proc_start_tp).count());

  CORO_RETURN(routine_success);
}

void cell_deactivation_routine::trigger_context_release()
{
  // Each release task is dispatched onto the UE's own FIFO task scheduler via dispatch_and_await_task_completion,
  // then wrapped in an eager_async_task so the scheduling happens immediately (not deferred until awaited). This way
  // all releases are started in parallel.
  for (cu_cp_ue_index_t ue_index : ues_to_release) {
    logger.info("ue={}: Releasing UE before cell deactivation", ue_index);
    cu_cp_ue_context_release_command command{ue_index, release_cause, true, std::chrono::seconds{5}};

    auto release = ue_mng.get_task_sched().dispatch_and_await_task_completion(
        ue_index, ue_release_handler.handle_ue_context_release_command(command));
    ue_release_tasks.push_back(
        launch_async([release = std::move(release)](coro_context<ue_release_task_t>& ctx) mutable {
          CORO_BEGIN(ctx);
          CORO_AWAIT_VALUE(auto result, release);
          CORO_RETURN(result);
        }));
  }
}
