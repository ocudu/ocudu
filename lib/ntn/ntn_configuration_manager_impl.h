// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ntn_orbital_compute_module.h"
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

  /// Per-satellite context holding the orbital compute module for one satellite.
  struct per_satellite_context {
    explicit per_satellite_context(orbit_propagator_type type) : ocm(type) {}

    ntn_orbital_compute_module ocm;

    /// Cached result of the last OCM computation.
    struct cached_result {
      time_point        epoch_time;
      slot_point        epoch_slot;
      unsigned          ntn_ul_sync_validity_dur;
      bool              use_state_vector;
      ntn_orbital_state state;
    };
    std::optional<cached_result> last_result;
  };

  /// Full cell config snapshot at a specific epoch time.
  struct cell_config_snapshot {
    time_point      epoch_time;
    ntn_cell_config config;
  };

  /// Per-cell context holding cell-specific NTN assistance info.
  struct per_cell_context {
    unique_timer              timer;
    std::optional<sib19_info> last_sib19;
    /// Queue of full cell config snapshots ordered by epoch_time. Always non-empty (seeded at construction).
    static_ring_buffer<cell_config_snapshot, 8> cell_cfg_queue;
  };

  /// \brief Returns the cell config applicable for the given epoch time.
  ///
  /// Pops snapshots superseded by a newer applicable entry and returns a reference to the
  /// most recent snapshot whose epoch_time <= t.
  /// \param ctx Per-cell context.
  /// \param t   SIB19 epoch time.
  const ntn_cell_config& get_cell_config(per_cell_context& ctx, time_point t) const;

  /// \brief Looks up the per-satellite context for a given satellite index.
  /// \return Pointer to the context, or nullptr if no satellite with this index is configured.
  per_satellite_context* find_satellite_context(unsigned satellite_index);

  /// \brief Computes the orbital state for the given epoch time, reusing the cached result if the epoch slot and time
  /// match within a 5 ms tolerance to avoid redundant OCM propagations within the same SI window tick.
  ntn_orbital_state compute_orbital_state(per_satellite_context& sat,
                                          time_point             epoch_time,
                                          slot_point             epoch_slot,
                                          unsigned               ntn_ul_sync_validity_dur,
                                          bool                   use_state_vector) const;

  /// \brief Computes and sends a request to apply CFO compensation for the feeder link Doppler shift.
  ///
  /// \param cell_cfg Cell configuration containing feeder link parameters and sector ID.
  /// \param doppler_update_time The time point at which the Doppler compensation should be updated.
  /// \param ta_info TA-Info used to compute Doppler shift frequencies.
  /// \return True if the request was successfully sent; false otherwise.
  bool send_cfo_compensation_request(const ntn_cell_config& cell_cfg,
                                     time_point             doppler_update_time,
                                     const ta_info_t&       ta_info);

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
  std::unique_ptr<ntn_sib19_update_handler>         sib19_pdu_update_handler;
  std::unique_ptr<ntn_time_provider>                time_provider;
  std::unique_ptr<ntn_doppler_compensation_handler> doppler_handler;
  timer_manager&                                    timers;
  task_executor&                                    executor;
  std::map<unsigned, per_satellite_context>         satellite_contexts;
  std::map<nr_cell_global_id_t, per_cell_context>   cells;
};

} // namespace ocudu_ntn
} // namespace ocudu
