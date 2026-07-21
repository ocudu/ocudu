// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "apps/services/remote_control/remote_command.h"
#include "ocudu/cu_cp/cu_cp_command_handler.h"

namespace ocudu {

/// \brief Remote command that locks a single cell identified by its NR CGI.
///
/// The CU-CP dispatches an F1AP gNB-CU Configuration Update with the cell in
/// cells_to_be_deactivated_list. The DU drains connected UEs (bar-first when the
/// graceful stop path is configured), stops MAC scheduling, and halts PHY. Other
/// cells on the same DU are unaffected.
class cell_lock_remote_command : public app_services::remote_command
{
  ocucp::cu_cp_command_handler& cu_cp;

public:
  explicit cell_lock_remote_command(ocucp::cu_cp_command_handler& cu_cp_) : cu_cp(cu_cp_) {}

  // See interface for documentation.
  std::string_view get_name() const override { return "cell_lock"; }

  // See interface for documentation.
  std::string_view get_description() const override
  {
    return "Lock a cell: CU-CP deactivates the cell identified by {plmn, nci}";
  }

  // See interface for documentation.
  error_type<std::string> execute(const nlohmann::json& json) override;
};

/// \brief Remote command that unlocks a single cell identified by its NR CGI.
///
/// Symmetric to cell_lock: the CU-CP dispatches an F1AP gNB-CU Configuration Update
/// with the cell in cells_to_be_activated_list. The DU restarts MAC and PHY and
/// restores the configured MIB cellBarred state.
class cell_unlock_remote_command : public app_services::remote_command
{
  ocucp::cu_cp_command_handler& cu_cp;

public:
  explicit cell_unlock_remote_command(ocucp::cu_cp_command_handler& cu_cp_) : cu_cp(cu_cp_) {}

  // See interface for documentation.
  std::string_view get_name() const override { return "cell_unlock"; }

  // See interface for documentation.
  std::string_view get_description() const override
  {
    return "Unlock a cell: CU-CP activates the cell identified by {plmn, nci}";
  }

  // See interface for documentation.
  error_type<std::string> execute(const nlohmann::json& json) override;
};

} // namespace ocudu
