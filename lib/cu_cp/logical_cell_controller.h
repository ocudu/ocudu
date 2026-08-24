// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "cu_cp_impl_interface.h"
#include "du_processor/du_processor_repository.h"
#include "du_processor/du_reported_cell.h"
#include "logical_cell_manager.h"
#include "ue_manager/ue_manager_impl.h"
#include "ocudu/cu_cp/cu_cp_cell_command_handler.h"
#include "ocudu/cu_cp/cu_cp_configuration.h"
#include "ocudu/support/async/async_task_scheduler.h"
#include <functional>
#include <vector>

namespace ocudu::ocucp {

/// \brief Owns the CU-CP logical cells and drives the cell-level command surface on top of them.
///
/// The controller is a component of the CU-CP: it implements the external cell commands (lock/unlock,
/// bar/unbar, state query), decides at F1 Setup which reported cells are activated, and keeps the recorded
/// cell state consistent through DU removal. The CU-CP forwards its DU events here and exposes the
/// controller as its cu_cp_cell_command_handler.
class logical_cell_controller final : public cu_cp_cell_command_handler
{
public:
  logical_cell_controller(const cu_cp_configuration&        cfg_,
                          du_processor_repository&          du_db_,
                          ue_manager&                       ue_mng_,
                          async_task_scheduler&             common_task_sched_,
                          cu_cp_ue_context_release_handler& ue_release_handler_);

  // cu_cp_cell_command_handler.
  async_task<cu_cp_cell_command_response> deactivate_cell(const nr_cell_global_id_t& cgi) override;
  async_task<cu_cp_cell_command_response> activate_cell(const nr_cell_global_id_t& cgi) override;
  async_task<cu_cp_cell_command_response> bar_cell(const nr_cell_global_id_t& cgi, bool barred) override;
  bool                                    dispatch_deactivate_cell(const nr_cell_global_id_t& cgi) override;
  bool                                    dispatch_activate_cell(const nr_cell_global_id_t& cgi) override;
  bool                                    dispatch_bar_cell(const nr_cell_global_id_t& cgi, bool barred) override;
  std::optional<cu_cp_cell_state>         dispatch_get_cell_state(const nr_cell_global_id_t& cgi) override;
  std::optional<cu_cp_cell_state>         get_cell_state(const nr_cell_global_id_t& cgi) const override;

  /// \brief Handle the cells reported by a DU in the F1 Setup procedure.
  ///
  /// Realizes the corresponding logical cells and decides, per reported cell, whether the CU-CP activates
  /// it. Undeclared NCIs get a dynamic logical cell: unlocked when no cells were declared in configuration,
  /// locked otherwise (the declared set acts as the activation whitelist).
  /// \return NCIs of the reported cells to include in the F1 Setup Response Cells to be Activated List;
  /// reported cells absent from it stay dormant (admin-locked).
  std::vector<nr_cell_identity> handle_du_cells_reported(cu_cp_du_index_t du_index, span<const du_reported_cell> cells);

  /// Handle the removal of a DU, de-realizing its logical cells while keeping operator intent.
  void handle_du_removed(cu_cp_du_index_t du_index);

  /// Registry access for the CU-CP fault-recovery flows (AMF loss and reconnection).
  logical_cell_manager&       cells() { return logical_cells; }
  const logical_cell_manager& cells() const { return logical_cells; }

private:
  /// Marshal the validation and scheduling of a cell command onto the CU-CP executor.
  bool dispatch_cell_command(const char* name, std::function<bool()> validate_and_schedule);

  const cu_cp_configuration&        cfg;
  du_processor_repository&          du_db;
  ue_manager&                       ue_mng;
  async_task_scheduler&             common_task_sched;
  cu_cp_ue_context_release_handler& ue_release_handler;

  logical_cell_manager logical_cells;

  ocudulog::basic_logger& logger = ocudulog::fetch_basic_logger("CU-CP");
};

} // namespace ocudu::ocucp
