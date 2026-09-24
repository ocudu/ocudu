// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "bcch_dl_sch_encoder.h"
#include "ocudu/mac/mac_cell_manager.h"
#include "ocudu/mac/mac_clock_controller.h"
#include "ocudu/mac/mac_metrics.h"

namespace ocudu {

/// Notifier used by MAC DL to forward cell metric reports.
class mac_cell_metric_notifier
{
public:
  virtual ~mac_cell_metric_notifier() = default;

  /// \brief Polling on whether a new MAC cell metric report is required.
  virtual bool is_report_required(slot_point_extended slot_tx) = 0;

  /// \brief Called when a new cell is activated.
  virtual void on_cell_activation() = 0;

  /// \brief Called when a cell is deactivated and provides the last report.
  virtual void on_cell_deactivation(const mac_dl_cell_metric_report& report) = 0;

  /// \brief Called when a new cell metric report is ready.
  virtual void on_cell_metric_report(const mac_dl_cell_metric_report& report) = 0;
};

/// \brief Dependencies between a MAC cell and remaining components of the MAC.
struct mac_cell_config_dependencies {
  /// Timer source for the cell.
  std::unique_ptr<mac_cell_clock_controller> timer_source;
  /// \brief Period of the metric reporting.
  std::chrono::milliseconds report_period{0};
  /// \brief Pointer to the MAC cell metric notifier.
  mac_cell_metric_notifier* notifier = nullptr;
};

/// Reconfiguration of a MAC cell in the MAC DL processor.
struct mac_dl_cell_reconfig_request {
  /// If not empty, contains the updates to be applied to the RRM policies.
  std::optional<du_cell_slice_reconfig_request> slice_reconf_req;
  /// If not empty, contains a new reference location uplink timing advance for an NTN cell.
  std::optional<sched_cell_ntn_ul_ta_update> ntn_ul_ta_update;
  /// If set, contains the new value for the MIB cellBarred flag (TS 38.331). Takes effect on the next SSB build.
  std::optional<bool> cell_barred_mod;
  /// If set, contains the new value for the MIB intraFreqReselection flag (TS 38.331). Takes effect on the next SSB
  /// build.
  std::optional<bool> intra_freq_reselection_mod;
};

/// Interface used to handle the activation/reconfiguration/deactivation of a cell in the MAC DL processor.
class mac_dl_cell_controller
{
public:
  virtual ~mac_dl_cell_controller() = default;

  /// Start the cell.
  virtual async_task<void> start() = 0;

  /// Stop the cell.
  virtual async_task<void> stop() = 0;

  /// \brief Starts the broadcast of the cell System Information.
  /// \param[in] ext_handler Handler of the SI PDU updates that bypass the SI modification window.
  /// \param[in] cmd First SI epoch of the cell.
  /// \param[in] pws_end_notifier Notified once the cell stops broadcasting the warnings of an ETWS/CMAS SI epoch.
  /// \remark Must be called once, before the cell is started.
  virtual void start_broadcast(std::shared_ptr<si_message_extension_handler> ext_handler,
                               const si_update_command&                      cmd,
                               std::unique_ptr<pws_broadcast_end_notifier>   pws_end_notifier) = 0;

  /// \brief Applies a new SI epoch, to be broadcast by the cell.
  ///
  /// An epoch listing active PWS SI messages is the one broadcast while a warning is on air, and coexists with the
  /// epoch of the normal operation.
  /// \remark Must be called from the cell control executor.
  virtual void handle_si_update(const si_update_command& cmd) = 0;

  /// Reconfigure operational cell.
  virtual async_task<void> reconfigure(const mac_dl_cell_reconfig_request& request) = 0;
};

/// Configurator of MAC cells in the MAC DL processor.
class mac_dl_cell_manager
{
public:
  virtual ~mac_dl_cell_manager() = default;

  /// Add new cell and set its configuration.
  virtual mac_dl_cell_controller& add_cell(const mac_cell_creation_request& cell_cfg,
                                           mac_cell_config_dependencies     deps) = 0;

  /// Remove an existing cell configuration.
  virtual void remove_cell(du_cell_index_t cell_index) = 0;
};

} // namespace ocudu
