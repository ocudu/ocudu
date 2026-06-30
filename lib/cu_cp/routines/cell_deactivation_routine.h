// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../du_processor/du_processor.h"
#include "../ue_manager/ue_manager_impl.h"
#include "cell_lifecycle_target.h"
#include "ocudu/adt/expected.h"
#include "ocudu/cu_cp/cu_cp_configuration.h"
#include "ocudu/f1ap/cu_cp/f1ap_cu_configuration_update.h"
#include "ocudu/ran/cause/ngap_cause.h"
#include "ocudu/support/async/eager_async_task.h"
#include <utility>

namespace ocudu::ocucp {

/// \brief Releases a caller-selected set of UEs and deactivates a caller-selected set of cells.
///
/// The CU-CP releases the UEs (so the operation does not rely on the DU autonomously draining them) before sending
/// the per-DU gNB-CU Configuration Updates that deactivate the cells. Pass an empty UE list to leave UE handling to
/// the DU.
class cell_deactivation_routine
{
public:
  cell_deactivation_routine(const cu_cp_configuration&         cu_cp_cfg_,
                            std::vector<cell_lifecycle_target> targets,
                            std::vector<cu_cp_ue_index_t>      ues_to_release_,
                            ngap_cause_t                       release_cause_,
                            du_processor_repository&           du_db_,
                            cu_cp_ue_context_release_handler&  ue_release_handler_,
                            ue_manager&                        ue_mng_,
                            ocudulog::basic_logger&            logger_);
  ~cell_deactivation_routine() = default;

  void operator()(coro_context<async_task<bool>>& ctx);

  static const char* name() { return "Cell Deactivation Routine"; }

  void trigger_context_release();

private:
  du_processor_repository&          du_db;
  cu_cp_ue_context_release_handler& ue_release_handler;
  ue_manager&                       ue_mng;
  ocudulog::basic_logger&           logger;

  std::vector<cu_cp_ue_index_t> ues_to_release;
  const ngap_cause_t            release_cause;

  // One gNB-CU Configuration Update per DU, built from the caller-provided targets.
  std::vector<std::pair<cu_cp_du_index_t, f1ap_gnb_cu_configuration_update>>           du_updates;
  std::vector<std::pair<cu_cp_du_index_t, f1ap_gnb_cu_configuration_update>>::iterator du_update_it;

  // (Sub-)Routine results.
  f1ap_gnb_cu_configuration_update_response f1ap_cu_cfg_update_response;
  bool                                      routine_success = true;

  using ue_release_task_t = eager_async_task<expected<cu_cp_ue_context_release_complete>>;
  std::vector<ue_release_task_t>           ue_release_tasks;
  std::vector<ue_release_task_t>::iterator ue_release_task_it;

  du_processor* du_proc = nullptr;

  std::chrono::steady_clock::time_point proc_start_tp;
};

} // namespace ocudu::ocucp
