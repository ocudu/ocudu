// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "propagators/rk4_propagator.h"
#include "ocudu/adt/ring_buffer.h"
#include "ocudu/ocudulog/ocudulog.h"
#include "ocudu/ran/ntn.h"
#include "ocudu/ran/slot_point.h"
#include <variant>

namespace ocudu {
namespace ocudu_ntn {

/// Contains an ephemeris update for a given epoch time.
struct ephemeris_info_update {
  using time_point = std::chrono::system_clock::time_point;
  /// Indicates when the ephemeris_info is valid.
  time_point epoch_time;
  /// Ephemeris info as ECEF state vector or orbital parameters.
  std::variant<ecef_coordinates_t, orbital_coordinates_t> ephemeris_info;
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

/// Contains parameters for generating a SIB19 NTN-config message.
struct sib19_ntn_configs_request {
  using time_point = std::chrono::system_clock::time_point;
  /// Whether to generate ECEF state vectors or Orbital parameters.
  bool use_state_vector;
  /// Indicates whether TA-Info for the feeder link is requested.
  bool feeder_link_present;
  /// Specifies SIB19 epoch time for which the NTN-config message content is to be generated.
  time_point epoch_time;
  /// Corresponding Tx slot.
  slot_point epoch_slot;
  /// Indicates how long the assistance info can be used. Unit is seconds.
  unsigned ntn_ul_sync_validity_dur;
};

/// Contains NTN-config message content.
struct sib19_ntn_configs_reply {
  using time_point = std::chrono::system_clock::time_point;
  /// Whether SIB19 NTN-Config was successfully generated.
  bool success;
  /// Specifies SIB19 epoch time for which the NTN-config message content is generated.
  time_point epoch_time;
  /// Corresponding Tx slot.
  slot_point epoch_slot;
  /// Indicates how long the assistance info can be used.
  unsigned ntn_ul_sync_validity_dur;
  /// Ephemeris info is always present as it is needed for the access link.
  std::variant<ecef_coordinates_t, orbital_coordinates_t> ephemeris_info;
  /// TA-info, present only if feeder link is present.
  std::optional<ta_info_t> ta_info;
};

/// Generates SIB19 NTN-config messages for given times and slots.
class ntn_assistance_info_generator
{
public:
  using time_point = std::chrono::system_clock::time_point;

  ntn_assistance_info_generator();

  /// \brief Enqueue new ephemeris information for orbit propagation.
  ///
  /// \param info Ephemeris information update (ECEF state vector or orbital parameters).
  /// \return true if the information was successfully enqueued, false otherwise (e.g., if the queue is full).
  bool enqueue_ephemeris_info(const ephemeris_info_update& info);

  /// \brief Enqueue new NTN gateway location info. Needed only if feedel link is present.
  ///
  /// \param info NTN gateway location update (service start/stop time and geodetic coordinates).
  /// \return true if the information was successfully enqueued, false otherwise (e.g., if the queue is full).
  bool enqueue_ntn_gw_location(ntn_gateway_location_info& info);

  /// \brief Generate SIB19 NTN-config message for the requested tx time.
  ///
  /// This function propagates the orbital ephemeris infotmation to the specified tx_time,
  /// and generates the corresponding SIB19 message content, including ephemeris information and optional TA-info.
  /// \param req The request containing tx_time, slot and configuration options.
  /// \return A reply containing the generated SIB19 NTN-config message.
  sib19_ntn_configs_reply generate_ntn_config(const sib19_ntn_configs_request& req);

private:
  /// \brief Retrieves the ephemeris information valid at the specified time point.
  ///
  /// \param t The time point for which to retrieve ephemeris information.
  /// \return The ephemeris information if available, or nullptr.
  orbit_ephemeris_info* get_ephemeris_info(time_point t);

  /// \brief Retrieves the NTN gateway location information valid at the specified time point.
  ///
  /// \param t The time point for which to retrieve NTN gateway location information.
  /// \return The NTN gateway location information if available, or nullptr.
  const ntn_gateway_location_info* get_ntn_gateway_location_info(time_point t);

  /// Logger.
  ocudulog::basic_logger& logger;
  /// RK4 propagator.
  rk4_propagator orbit_propagator;

  /// Maximum number of entries that can be stored in the ring buffers.
  static constexpr unsigned max_nof_entries = 64;

  /// Ring buffer to store ephemeris information updates for orbit propagation.
  static_ring_buffer<orbit_ephemeris_info, max_nof_entries> ephemeris_info_queue;

  /// Ring buffer to store NTN gateway location info.
  static_ring_buffer<ntn_gateway_location_info, max_nof_entries> ntn_gateway_queue;
};

} // namespace ocudu_ntn
} // namespace ocudu
