// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "orbit_ephemeris_info.h"
#include "propagators/orbital_propagator_model.h"
#include "ocudu/adt/ring_buffer.h"
#include "ocudu/ntn/ntn_configuration_manager_config.h"
#include "ocudu/ocudulog/ocudulog.h"
#include "ocudu/ran/slot_point.h"
#include <memory>
#include <variant>

namespace ocudu {
namespace ocudu_ntn {

/// Contains an ephemeris update for a given epoch time.
struct ephemeris_info_update {
  using time_point = std::chrono::system_clock::time_point;
  /// Indicates when the ephemeris_info is valid.
  time_point epoch_time;
  /// Ephemeris info as ECEF state vector or orbital parameters.
  ntn_ephemeris_info_t ephemeris_info;
};

/// Contains information about the NTN gateway location and its service time window.
struct ntn_gateway_location_info {
  using time_point = std::chrono::system_clock::time_point;
  /// Indicates when the NTN gateway starts serving the NTN payload.
  time_point service_start_time;
  /// Indicates when the NTN gateway stops serving the NTN payload.
  std::optional<time_point> service_stop_time;
  /// NTN gateway location as geodetic coordinates.
  geodetic_coordinates_t ntn_gateway_location;
  /// NTN gateway location in ECEF reference frame.
  std::optional<state_vector> ntn_gateway_ecef_location;
};

/// Propagated satellite orbital state at a given epoch time.
struct ntn_orbital_state {
  using time_point = std::chrono::system_clock::time_point;
  /// Whether computation succeeded.
  bool success;
  /// Epoch time for which the computation was performed.
  time_point epoch_time;
  /// Propagated ephemeris in the requested format. Valid only when success is true.
  ntn_ephemeris_info_t ephemeris_info;
  /// TA-info, present when gateway location or TA override is available. Valid only when success is true.
  std::optional<ta_info_t> ta_info;
};

/// Computes satellite orbital propagation, ephemeris, and TA-info for a given epoch time.
class ntn_orbital_compute_module
{
public:
  using time_point = std::chrono::system_clock::time_point;

  explicit ntn_orbital_compute_module(orbit_propagator_type type = orbit_propagator_type::rk4);

  /// \brief Enqueue new ephemeris information for orbit propagation.
  ///
  /// \param info Ephemeris information update (ECEF state vector or orbital parameters).
  /// \return true if the information was successfully enqueued, false otherwise (e.g., if the queue is full).
  bool enqueue_ephemeris_info(const ephemeris_info_update& info);

  /// \brief Enqueue new NTN gateway location info. Needed only if feeder link is present.
  ///
  /// \param info NTN gateway location update (service start/stop time and geodetic coordinates).
  /// \return true if the information was successfully enqueued, false otherwise (e.g., if the queue is full).
  bool enqueue_ntn_gw_location(ntn_gateway_location_info& info);

  /// \brief Sets a fixed TA-info override for transparent-architecture satellites, bypassing TA-info computation
  /// from the NTN gateway location. Mutually exclusive with the NTN gateway location in practice.
  void set_ta_info_override(const std::optional<ta_info_t>& info) { ta_info_override = info; }

  /// \brief Propagate the satellite orbit to the given epoch time and compute the orbital state.
  ///
  /// \param epoch_time               Epoch time to propagate to.
  /// \param ntn_ul_sync_validity_dur UL sync validity window [s], used for TA quadratic-fit.
  /// \param use_state_vector         Whether to produce ECEF state vectors (true) or orbital parameters (false).
  /// \return Propagated orbital state (ephemeris + optional TA-info).
  ntn_orbital_state
  compute_orbital_state(time_point epoch_time, unsigned ntn_ul_sync_validity_dur, bool use_state_vector = true);

private:
  /// \brief Retrieves the ephemeris information valid at the specified time point.
  orbit_ephemeris_info* get_ephemeris_info(time_point t);

  /// \brief Retrieves the NTN gateway location information valid at the specified time point.
  const ntn_gateway_location_info* get_ntn_gateway_location_info(time_point t);

  /// Logger.
  ocudulog::basic_logger& logger;
  /// Orbit propagator (rk4 or keplerian, set at construction).
  std::unique_ptr<orbital_propagation_model> orbit_propagator;

  /// Maximum number of entries that can be stored in the ring buffers.
  static constexpr unsigned max_nof_entries = 64;

  /// Ring buffer to store ephemeris information updates for orbit propagation.
  static_ring_buffer<orbit_ephemeris_info, max_nof_entries> ephemeris_info_queue;

  /// Ring buffer to store NTN gateway location info.
  static_ring_buffer<ntn_gateway_location_info, max_nof_entries> ntn_gateway_queue;

  /// Fixed TA-info override for transparent-architecture satellites, set via \c set_ta_info_override.
  std::optional<ta_info_t> ta_info_override;
};

} // namespace ocudu_ntn
} // namespace ocudu
