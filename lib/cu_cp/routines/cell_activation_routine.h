// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../du_processor/du_processor_repository.h"
#include "cell_lifecycle_target.h"

namespace ocudu::ocucp {

/// Holds the cell activation routine configuration parameters.
struct cell_activation_routine_configuration {
  std::string                        ran_node_name;
  std::vector<cell_lifecycle_target> targets;
};

/// Holds the cell activation routine dependencies.
struct cell_activation_routine_dependencies {
  du_processor_repository& du_db;
  logical_cell_manager&    logical_cells;
  ocudulog::basic_logger&  logger;
};

/// \brief Activates a caller-selected set of cells via per-DU gNB-CU Configuration Updates.
///
/// The logical cells of an acknowledged update become operationally enabled.
class cell_activation_routine
{
public:
  cell_activation_routine(const cell_activation_routine_configuration& cfg,
                          const cell_activation_routine_dependencies&  dependencies);

  ~cell_activation_routine() = default;

  void operator()(coro_context<async_task<bool>>& ctx);

  static const char* name() { return "Cell Activation Routine"; }

private:
  du_processor_repository& du_db;
  logical_cell_manager&    logical_cells;
  ocudulog::basic_logger&  logger;

  // One gNB-CU Configuration Update per DU, built from the caller-provided targets.
  std::vector<std::pair<cu_cp_du_index_t, f1ap_gnb_cu_configuration_update>>           du_updates;
  std::vector<std::pair<cu_cp_du_index_t, f1ap_gnb_cu_configuration_update>>::iterator du_update_it;

  // (Sub-)Routine results.
  f1ap_gnb_cu_configuration_update_response f1ap_cu_cfg_update_response;
  bool                                      routine_success = true;

  du_processor* du_proc = nullptr;
};

} // namespace ocudu::ocucp
