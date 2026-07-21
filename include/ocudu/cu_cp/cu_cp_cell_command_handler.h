// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/nr_cgi.h"
#include "ocudu/support/async/async_task.h"

namespace ocudu::ocucp {

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

  /// \brief Deactivate (administratively lock) a single cell identified by its NR CGI.
  ///
  /// Records the lock on the CU-CP's logical cell — so the intent survives DU restarts — and drives the
  /// CU-driven graceful stop: the cell is barred (TS 38.473 Cells to be Barred List) so idle UEs reselect
  /// away, its UEs are released from the CU-CP (the gNB-CU Configuration Update procedure itself does not
  /// affect existing UE-related contexts, TS 38.473 section 8.2.5.1), and finally the cell is deactivated
  /// via a gNB-CU Configuration Update listing it in cells_to_be_deactivated_list.
  ///
  /// The returned task completes once the DU has acknowledged the F1AP updates. Callers that do not need
  /// to await completion (e.g. fire-and-forget from a WS/O1 handler) should use dispatch_deactivate_cell
  /// instead.
  /// \param[in] cgi NR Cell Global ID of the cell to deactivate.
  virtual async_task<cu_cp_cell_command_response> deactivate_cell(const nr_cell_global_id_t& cgi) = 0;

  /// \brief Activate (administratively unlock) a single cell identified by its NR CGI.
  ///
  /// Clears the lock on the CU-CP's logical cell and dispatches an F1AP gNB-CU Configuration Update to the
  /// DU that serves the cell, listing the cell in cells_to_be_activated_list. The DU brings the MAC and PHY
  /// online for the cell. If the logical cell carries barred intent, the CU-CP re-applies the bar right
  /// after the activation.
  /// \param[in] cgi NR Cell Global ID of the cell to activate.
  virtual async_task<cu_cp_cell_command_response> activate_cell(const nr_cell_global_id_t& cgi) = 0;

  /// \brief Bar or unbar a single cell identified by its NR CGI, without changing its activation state.
  ///
  /// Records the barred intent on the CU-CP's logical cell and, if the cell is currently active, drives an
  /// F1AP gNB-CU Configuration Update carrying the Cells to be Barred List (TS 38.473). The intent is
  /// re-applied whenever the cell is (re)activated or its DU reconnects. If the cell is currently
  /// deactivated, only the intent is recorded (a dormant cell transmits no MIB to bar).
  /// \param[in] cgi NR Cell Global ID of the cell to bar/unbar.
  /// \param[in] barred New barred state: true to bar, false to unbar.
  virtual async_task<cu_cp_cell_command_response> bar_cell(const nr_cell_global_id_t& cgi, bool barred) = 0;

  /// \brief Fire-and-forget synchronous variant of deactivate_cell.
  ///
  /// Schedules the deactivation on the CU-CP task scheduler and returns immediately. Intended for
  /// callers that cannot block (e.g. WebSocket/O1 command handlers running on the IO broker thread).
  /// Returns true if the command was accepted (CGI resolved to a served DU) and scheduled; false if
  /// the CGI is unknown or scheduling failed. The actual deactivation, including UE drain and PHY
  /// stop, completes asynchronously.
  virtual bool dispatch_deactivate_cell(const nr_cell_global_id_t& cgi) = 0;

  /// Fire-and-forget synchronous variant of activate_cell. See dispatch_deactivate_cell for the rationale.
  virtual bool dispatch_activate_cell(const nr_cell_global_id_t& cgi) = 0;

  /// Fire-and-forget synchronous variant of bar_cell. See dispatch_deactivate_cell for the rationale.
  virtual bool dispatch_bar_cell(const nr_cell_global_id_t& cgi, bool barred) = 0;
};

} // namespace ocudu::ocucp
