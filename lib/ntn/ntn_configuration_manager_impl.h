// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ntn_assistance_info_generator.h"
#include "ocudu/ntn/ntn_configuration_manager.h"
#include "ocudu/ntn/ntn_configuration_manager_config.h"
#include "ocudu/ntn/ntn_configuration_manager_dependencies.h"
#include "ocudu/ocudulog/ocudulog.h"
#include "ocudu/ran/ntn.h"
#include "ocudu/ran/sib/system_info_config.h"
#include "ocudu/ran/slot_point.h"
#include "ocudu/support/timers.h"
#include <map>
#include <optional>

namespace ocudu {

class timer_manager;
class task_executor;

namespace ocudu_ntn {

class ntn_sib19_update_handler;
class ntn_doppler_compensation_handler;
class ntn_time_provider;

/// Class that updates NTN Configuration based on the information provided by O&M.
class ntn_configuration_manager_impl : public ntn_configuration_manager
{
public:
  using time_point = std::chrono::system_clock::time_point;

  ntn_configuration_manager_impl(const ntn_configuration_manager_config& config,
                                 ntn_configuration_manager_dependencies  dependencies);

  /// \brief Handle NTN configuration update request for one or more cells.
  ///
  /// This function processes NTN configuration updates, including generating timestamped SIB19 PDUs and setting the
  /// Doppler shift for PHY layer pre- and post-compensation for the feeder link.
  /// \param req NTN configuration update request containing one or more cell configurations.
  /// \return Result containing lists of successfully updated and failed cells.
  ntn_config_update_result handle_ntn_config_update(const ntn_config_update_info& req) override;

private:
  /// \brief Handle NTN configuration update for a single cell.
  ///
  /// \param cell_req NTN configuration update request for one cell.
  /// \return True if the update was successfully handled; false otherwise.
  bool handle_ntn_cell_config_update(const ntn_cell_config_update_info& cell_req);

  /// Per-cell context holding cell-specific NTN assistance info.
  struct per_cell_context {
    ntn_cell_config               cell_cfg;
    ntn_assistance_info_generator ntn_info_generator;
    bool                          sat_switch_enabled = false;
    ntn_assistance_info_generator sat_switch_info_generator;
    unique_timer                  timer;
    std::optional<sib19_info>     last_sib19;
    /// Deferred cell-config updates waiting for SIB19_Tx_time >= epoch_time.
    static_ring_buffer<ntn_cell_config_update_info, 8> deferred_cell_cfg_queue;
  };

  /// \brief Apply a previously deferred cell config update to the per-cell context.
  ///
  /// Performs the field assignments that are deferred until SIB19_Tx_time >= entry.epoch_time.
  /// \param ctx    Per-cell context to update.
  /// \param update The deferred config update to apply.
  void apply_deferred_cell_config_update(per_cell_context& ctx, const ntn_cell_config_update_info& update);

  /// \brief Computes and sends a request to apply CFO compensation for the feeder link Doppler shift.
  ///
  /// \param ctx Per-cell context containing cell configuration
  /// \param doppler_update_time The time point at which the Doppler compensation should be updated.
  /// \param ta_info TA-Info used to compute Doppler shift frequencies.
  /// \return True if the request was successfully sent;
  /// false otherwise.
  bool
  send_cfo_compensation_request(const per_cell_context& ctx, time_point doppler_update_time, const ta_info_t& ta_info);

  /// \brief Periodic task to generate and update NTN configuration for a specific cell.
  ///
  /// Called periodically to calculate SI window boundaries, generate NTN assistance information (ephemeris, TA-info),
  /// send SIB19 updates to DU, and request Doppler compensation from RU for feeder link effects.
  ///
  /// \param nr_cgi Cell global ID
  /// \param tp Current system time point
  /// \param sl Current slot
  void periodic_ntn_config_update_task(const nr_cell_global_id_t& nr_cgi, time_point tp, slot_point sl);

  ocudulog::basic_logger&                           logger;
  ntn_configuration_manager_config                  cfg;
  std::unique_ptr<ntn_sib19_update_handler>         sib19_pdu_update_handler;
  std::unique_ptr<ntn_time_provider>                time_provider;
  std::unique_ptr<ntn_doppler_compensation_handler> doppler_handler;
  timer_manager&                                    timers;
  task_executor&                                    executor;
  std::map<nr_cell_global_id_t, per_cell_context>   cells;
};

} // namespace ocudu_ntn
} // namespace ocudu
