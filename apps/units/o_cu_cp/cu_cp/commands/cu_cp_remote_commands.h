// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "apps/services/remote_control/remote_command.h"
#include "ocudu/cu_cp/cu_cp_command_handler.h"
#include <chrono>

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
  expected<nlohmann::json, std::string> execute(const nlohmann::json& json) override;
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
  expected<nlohmann::json, std::string> execute(const nlohmann::json& json) override;
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
  expected<nlohmann::json, std::string> execute(const nlohmann::json& json) override;
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
  expected<nlohmann::json, std::string> execute(const nlohmann::json& json) override;
};

/// \brief Remote command that reads the recorded state of a single cell identified by its NR CGI.
///
/// The success response carries the cell's administrative state, operational state and barred intent.
class cell_status_remote_command : public app_services::remote_command
{
  ocucp::cu_cp_command_handler& cu_cp;

public:
  explicit cell_status_remote_command(ocucp::cu_cp_command_handler& cu_cp_) : cu_cp(cu_cp_) {}

  // See interface for documentation.
  std::string_view get_name() const override { return "cell_status"; }

  // See interface for documentation.
  std::string_view get_description() const override
  {
    return "Read the recorded state (admin_state, operational_state, cell_barred) of the cell identified by "
           "{plmn, nci}";
  }

  // See interface for documentation.
  expected<nlohmann::json, std::string> execute(const nlohmann::json& json) override;
};

/// \brief Remote command that adds a cell to the mobility measurement configuration or replaces its
/// measurement parameters.
///
/// The parameters are replaced as a whole: optional keys left out of the payload are cleared, which is also
/// how a parameter is unset. The neighbor relations and the periodic report of the cell are kept. For cells
/// served by a DU of this CU-CP the parameters provided over F1 take precedence on (re)attach, so the command
/// is mainly used to declare external cells (and their radio parameters) at runtime.
class mobility_cell_set_remote_command : public app_services::remote_command
{
  ocucp::cu_cp_command_handler& cu_cp;

public:
  explicit mobility_cell_set_remote_command(ocucp::cu_cp_command_handler& cu_cp_) : cu_cp(cu_cp_) {}

  // See interface for documentation.
  std::string_view get_name() const override { return "mobility_cell_set"; }

  // See interface for documentation.
  std::string_view get_description() const override
  {
    return "Add a cell to the mobility measurement configuration or replace its parameters: {nci, "
           "gnb_id_bit_length, [pci], "
           "[plmn], [tac], [band], [ssb_arfcn], [ssb_scs], [ssb_period], [ssb_offset], [ssb_duration]}";
  }

  // See interface for documentation.
  expected<nlohmann::json, std::string> execute(const nlohmann::json& json) override;
};

/// \brief Remote command that removes a cell from the mobility measurement configuration, including the
/// neighbor relations of other cells pointing at it.
class mobility_cell_remove_remote_command : public app_services::remote_command
{
  ocucp::cu_cp_command_handler& cu_cp;

public:
  explicit mobility_cell_remove_remote_command(ocucp::cu_cp_command_handler& cu_cp_) : cu_cp(cu_cp_) {}

  // See interface for documentation.
  std::string_view get_name() const override { return "mobility_cell_remove"; }

  // See interface for documentation.
  std::string_view get_description() const override
  {
    return "Remove a cell from the mobility measurement configuration: {nci}";
  }

  // See interface for documentation.
  expected<nlohmann::json, std::string> execute(const nlohmann::json& json) override;
};

/// \brief Remote command that adds a neighbor relation between two configured cells, or replaces the
/// report config ids of an existing one. Directional: the reverse relation must be added separately.
class neighbor_add_remote_command : public app_services::remote_command
{
  ocucp::cu_cp_command_handler& cu_cp;

public:
  explicit neighbor_add_remote_command(ocucp::cu_cp_command_handler& cu_cp_) : cu_cp(cu_cp_) {}

  // See interface for documentation.
  std::string_view get_name() const override { return "mobility_neighbor_add"; }

  // See interface for documentation.
  std::string_view get_description() const override
  {
    return "Add or update a neighbor relation: {nci, neighbor_nci, report_configs: [ids]}";
  }

  // See interface for documentation.
  expected<nlohmann::json, std::string> execute(const nlohmann::json& json) override;
};

/// \brief Remote command that removes the neighbor relation between two cells.
class neighbor_remove_remote_command : public app_services::remote_command
{
  ocucp::cu_cp_command_handler& cu_cp;

public:
  explicit neighbor_remove_remote_command(ocucp::cu_cp_command_handler& cu_cp_) : cu_cp(cu_cp_) {}

  // See interface for documentation.
  std::string_view get_name() const override { return "mobility_neighbor_remove"; }

  // See interface for documentation.
  std::string_view get_description() const override { return "Remove a neighbor relation: {nci, neighbor_nci}"; }

  // See interface for documentation.
  expected<nlohmann::json, std::string> execute(const nlohmann::json& json) override;
};

/// \brief Remote command that adds a new report configuration or replaces an existing one.
///
/// Distance and time based conditional events (d1, d2, t1) are not supported through this command.
class report_config_set_remote_command : public app_services::remote_command
{
  ocucp::cu_cp_command_handler& cu_cp;

public:
  explicit report_config_set_remote_command(ocucp::cu_cp_command_handler& cu_cp_) : cu_cp(cu_cp_) {}

  // See interface for documentation.
  std::string_view get_name() const override { return "mobility_report_config_set"; }

  // See interface for documentation.
  std::string_view get_description() const override
  {
    return "Add or update a report configuration: {report_cfg_id, report_type, [report_interval_ms], "
           "[event_triggered_report_type], [meas_trigger_quantity], [meas_trigger_quantity_threshold_db], "
           "[meas_trigger_quantity_threshold_2_db], [meas_trigger_quantity_offset_db], [hysteresis_db], "
           "[time_to_trigger_ms], [t312], [periodic_ho_rsrp_offset_db]}";
  }

  // See interface for documentation.
  expected<nlohmann::json, std::string> execute(const nlohmann::json& json) override;
};

/// \brief Remote command that removes a report configuration. Refused while any neighbor relation or
/// serving-cell periodic report references it.
class report_config_remove_remote_command : public app_services::remote_command
{
  ocucp::cu_cp_command_handler& cu_cp;

public:
  explicit report_config_remove_remote_command(ocucp::cu_cp_command_handler& cu_cp_) : cu_cp(cu_cp_) {}

  // See interface for documentation.
  std::string_view get_name() const override { return "mobility_report_config_remove"; }

  // See interface for documentation.
  std::string_view get_description() const override { return "Remove a report configuration: {report_cfg_id}"; }

  // See interface for documentation.
  expected<nlohmann::json, std::string> execute(const nlohmann::json& json) override;
};

/// \brief Remote command that sets or clears the serving-cell periodical report config of a cell.
class periodic_report_set_remote_command : public app_services::remote_command
{
  ocucp::cu_cp_command_handler& cu_cp;

public:
  explicit periodic_report_set_remote_command(ocucp::cu_cp_command_handler& cu_cp_) : cu_cp(cu_cp_) {}

  // See interface for documentation.
  std::string_view get_name() const override { return "mobility_periodic_report_set"; }

  // See interface for documentation.
  std::string_view get_description() const override
  {
    return "Set or clear the periodical report of a cell: {nci, [report_cfg_id]} (omit report_cfg_id to clear)";
  }

  // See interface for documentation.
  expected<nlohmann::json, std::string> execute(const nlohmann::json& json) override;
};

/// \brief Remote command that triggers the handover of a UE to a target cell.
///
/// The UE is identified by its serving cell PCI and RNTI; the target by PCI, PLMN and TAC. The command
/// reports whether the CU-CP accepted the trigger (UE and target cell known, trigger dispatched on the CU-CP
/// executor); the handover outcome is asynchronous.
class trigger_handover_remote_command : public app_services::remote_command
{
  ocucp::cu_cp_command_handler& cu_cp;

public:
  explicit trigger_handover_remote_command(ocucp::cu_cp_command_handler& cu_cp_) : cu_cp(cu_cp_) {}

  // See interface for documentation.
  std::string_view get_name() const override { return "trigger_handover"; }

  // See interface for documentation.
  std::string_view get_description() const override
  {
    return "Trigger handover of a UE to a target cell: {serving_pci, rnti, target_pci, plmn, tac}";
  }

  // See interface for documentation.
  expected<nlohmann::json, std::string> execute(const nlohmann::json& json) override;
};

/// \brief Remote command that triggers a Conditional Handover (CHO) with one or more target cells.
///
/// The command reports whether the CU-CP accepted the trigger; the outcome of the conditional handover is
/// asynchronous.
class trigger_conditional_handover_remote_command : public app_services::remote_command
{
  ocucp::cu_cp_command_handler& cu_cp;
  std::chrono::milliseconds     default_timeout;

public:
  explicit trigger_conditional_handover_remote_command(ocucp::cu_cp_command_handler& cu_cp_,
                                                       std::chrono::milliseconds     default_timeout_) :
    cu_cp(cu_cp_), default_timeout(default_timeout_)
  {
  }

  // See interface for documentation.
  std::string_view get_name() const override { return "trigger_conditional_handover"; }

  // See interface for documentation.
  std::string_view get_description() const override
  {
    return "Trigger conditional handover of a UE: {serving_pci, rnti, target_pcis: [1..8 pcis], [timeout_ms], "
           "[t1_thres]} (t1_thres as unix ms integer, or a unix ms / YYYY-MM-DDTHH:MM:SS[.mmm] string)";
  }

  // See interface for documentation.
  expected<nlohmann::json, std::string> execute(const nlohmann::json& json) override;
};

} // namespace ocudu
