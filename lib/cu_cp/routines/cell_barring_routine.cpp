// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "cell_barring_routine.h"
#include "ocudu/support/async/coroutine.h"
#include <map>

using namespace ocudu;
using namespace ocudu::ocucp;

cell_barring_routine::cell_barring_routine(const cu_cp_configuration&         cu_cp_cfg_,
                                           std::vector<cell_lifecycle_target> targets,
                                           bool                               barred_,
                                           du_processor_repository&           du_db_,
                                           ocudulog::basic_logger&            logger_) :
  cu_cp_cfg(cu_cp_cfg_), du_db(du_db_), logger(logger_)
{
  // Group the targets into a single gNB-CU Configuration Update per DU.
  //
  // Note: the Cells to be Barred List IE has criticality "ignore" (TS 38.473 section 9.2.1.10). A DU that
  // does not support it drops the IE and still acknowledges the update, so a reported success proves the
  // request was accepted, not that the barred MIB reached the air.
  std::map<cu_cp_du_index_t, f1ap_gnb_cu_configuration_update> by_du;
  for (const cell_lifecycle_target& target : targets) {
    f1ap_gnb_cu_configuration_update& update = by_du[target.du_index];
    update.gnb_cu_name                       = cu_cp_cfg.node.ran_node_name;
    update.cells_to_be_barred_list.push_back({target.cgi, barred_});
  }
  for (auto& [du_index, update] : by_du) {
    du_updates.emplace_back(du_index, std::move(update));
  }
}

void cell_barring_routine::operator()(coro_context<async_task<bool>>& ctx)
{
  CORO_BEGIN(ctx);

  logger.info("\"{}\" started...", name());

  for (du_update_it = du_updates.begin(); du_update_it != du_updates.end(); ++du_update_it) {
    du_proc = du_db.find_du_processor(du_update_it->first);
    if (du_proc == nullptr) {
      logger.warning("DU processor not found for index {}", du_update_it->first);
      routine_success = false;
      continue;
    }

    CORO_AWAIT_VALUE(f1ap_cu_cfg_update_response, du_proc->handle_configuration_update(du_update_it->second));
    if (!f1ap_cu_cfg_update_response.success) {
      logger.info("Cell barring update for du={} failed. Cause: {}",
                  du_update_it->first,
                  f1ap_cu_cfg_update_response.cause.has_value()
                      ? fmt::to_string(f1ap_cu_cfg_update_response.cause.value())
                      : "timeout");
      routine_success = false;
    }
  }

  logger.info("\"{}\" finished {}", name(), routine_success ? "successfully" : "with errors");

  CORO_RETURN(routine_success);
}
