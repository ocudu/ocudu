// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../du_processor/du_processor.h"
#include "../du_processor/du_processor_repository.h"
#include "cell_lifecycle_target.h"
#include "ocudu/cu_cp/cu_cp_configuration.h"
#include "ocudu/f1ap/cu_cp/f1ap_cu_configuration_update.h"
#include <utility>

namespace ocudu::ocucp {

/// \brief Bars or unbars a caller-selected set of cells via per-DU gNB-CU Configuration Updates carrying the
/// Cells to be Barred List (TS 38.473), without touching their activation state.
///
/// Used to apply the operator's barred intent: standalone bar/unbar commands, and re-application of barred
/// intent after a cell is (re)activated or its DU reconnects.
class cell_barring_routine
{
public:
  cell_barring_routine(const cu_cp_configuration&         cu_cp_cfg_,
                       std::vector<cell_lifecycle_target> targets,
                       bool                               barred_,
                       du_processor_repository&           du_db_,
                       ocudulog::basic_logger&            logger_);
  ~cell_barring_routine() = default;

  void operator()(coro_context<async_task<bool>>& ctx);

  static const char* name() { return "Cell Barring Routine"; }

private:
  const cu_cp_configuration& cu_cp_cfg;
  du_processor_repository&   du_db;
  ocudulog::basic_logger&    logger;

  // One gNB-CU Configuration Update per DU, built from the caller-provided targets.
  std::vector<std::pair<cu_cp_du_index_t, f1ap_gnb_cu_configuration_update>>           du_updates;
  std::vector<std::pair<cu_cp_du_index_t, f1ap_gnb_cu_configuration_update>>::iterator du_update_it;

  // (Sub-)Routine results.
  f1ap_gnb_cu_configuration_update_response f1ap_cu_cfg_update_response;
  bool                                      routine_success = true;

  du_processor* du_proc = nullptr;
};

} // namespace ocudu::ocucp
