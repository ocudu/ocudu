// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "apps/services/remote_control/remote_command.h"
#include "ocudu/cu_cp/cu_cp_command_handler.h"

namespace ocudu {

/// \brief Remote command that locks a single cell identified by its NR CGI.
///
/// The CU-CP records the lock on its logical cell (so it survives DU restarts) and drives the graceful
/// stop: bar the cell, release its UEs from the CU-CP, then deactivate it via an F1AP gNB-CU Configuration
/// Update with the cell in cells_to_be_deactivated_list. Other cells on the same DU are unaffected.
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
/// Symmetric to cell_lock: the CU-CP clears the lock on its logical cell and dispatches an F1AP gNB-CU
/// Configuration Update with the cell in cells_to_be_activated_list. The DU restarts MAC and PHY; if the
/// logical cell carries barred intent, the CU-CP re-applies the bar right after activation.
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

/// \brief Remote command that bars a single cell identified by its NR CGI, without deactivating it.
///
/// The CU-CP records the barred intent on its logical cell and, if the cell is active, dispatches an F1AP
/// gNB-CU Configuration Update carrying the Cells to be Barred List (TS 38.473). The intent is re-applied
/// whenever the cell is reactivated or its DU reconnects.
class cell_bar_remote_command : public app_services::remote_command
{
  ocucp::cu_cp_command_handler& cu_cp;

public:
  explicit cell_bar_remote_command(ocucp::cu_cp_command_handler& cu_cp_) : cu_cp(cu_cp_) {}

  // See interface for documentation.
  std::string_view get_name() const override { return "cell_bar"; }

  // See interface for documentation.
  std::string_view get_description() const override
  {
    return "Bar a cell: CU-CP sets MIB cellBarred=barred on the cell identified by {plmn, nci}";
  }

  // See interface for documentation.
  error_type<std::string> execute(const nlohmann::json& json) override;
};

/// \brief Remote command that unbars a single cell identified by its NR CGI.
///
/// Symmetric to cell_bar: clears the barred intent and, if the cell is active, drives the F1AP update with
/// cellBarred=notBarred.
class cell_unbar_remote_command : public app_services::remote_command
{
  ocucp::cu_cp_command_handler& cu_cp;

public:
  explicit cell_unbar_remote_command(ocucp::cu_cp_command_handler& cu_cp_) : cu_cp(cu_cp_) {}

  // See interface for documentation.
  std::string_view get_name() const override { return "cell_unbar"; }

  // See interface for documentation.
  std::string_view get_description() const override
  {
    return "Unbar a cell: CU-CP sets MIB cellBarred=notBarred on the cell identified by {plmn, nci}";
  }

  // See interface for documentation.
  error_type<std::string> execute(const nlohmann::json& json) override;
};

} // namespace ocudu
