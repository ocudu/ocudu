// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/nr_cgi.h"
#include "ocudu/support/async/async_task.h"

namespace ocudu {
namespace ocucp {

/// Result of a cell-level command issued via cu_cp_cell_command_handler.
struct cu_cp_cell_command_response {
  /// Whether the command completed successfully from CU-CP's point of view (the DU accepted the F1AP update).
  bool success = false;
};

/// \brief Handler for external cell-level commands directed at the CU-CP.
///
/// Intended for use by external intelligence (O1, E2, management plane, or other controllers) that needs to drive
/// cell lifecycle at runtime without bypassing the CU-CP. Each command is translated by the CU-CP into an F1AP
/// gNB-CU Configuration Update targeted at the DU that serves the cell, which keeps CU-CP's internal registry
/// in sync with the DU's actual cell state.
class cu_cp_cell_command_handler
{
public:
  virtual ~cu_cp_cell_command_handler() = default;

  /// \brief Deactivate a single cell identified by its NR CGI.
  ///
  /// Dispatches an F1AP gNB-CU Configuration Update to the DU that serves the cell, listing the cell in
  /// cells_to_be_deactivated_list. Connected UEs on the cell are released by the DU (via its own UE drain
  /// procedure) before the MAC and PHY are stopped.
  /// \param[in] cgi NR Cell Global ID of the cell to deactivate.
  virtual async_task<cu_cp_cell_command_response> deactivate_cell(const nr_cell_global_id_t& cgi) = 0;

  /// \brief Activate a single cell identified by its NR CGI.
  ///
  /// Dispatches an F1AP gNB-CU Configuration Update to the DU that serves the cell, listing the cell in
  /// cells_to_be_activated_list. The DU brings the MAC and PHY online for the cell.
  /// \param[in] cgi NR Cell Global ID of the cell to activate.
  virtual async_task<cu_cp_cell_command_response> activate_cell(const nr_cell_global_id_t& cgi) = 0;
};

} // namespace ocucp
} // namespace ocudu
