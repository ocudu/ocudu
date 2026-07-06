// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ntn_orbital_compute_module.h"
#include "converters/coordinate_converter.h"
#include "coordinates_types.h"
#include "ntn_math_helpers.h"
#include "propagators/keplerian_propagator.h"
#include "propagators/rk4_propagator.h"
#include <chrono>
#include <vector>

using namespace ocudu;
using namespace ocudu_ntn;

/// Speed of light in vacuum [km/s]
static constexpr double SPEED_OF_LIGHT_KM_S = 299792.458;

/// \brief Computes timing advance (TA) information parameters: TA-common, TA-drift, and TA-drift-variant.
///
/// \param init_ephemeris_info Initial satellite orbit ephemeris information.
/// \param ntn_gateway_ecef ECEF state vector of the NTN gateway.
/// \param ntn_ul_sync_validity_dur Uplink synchronization validity duration [seconds].
/// \param nof_steps Number of propagation steps (default: 5).
/// \return Structure containing TA common, TA drift, and TA drift variant parameters.
static ta_info_t compute_ta_info(const orbit_ephemeris_info& init_ephemeris_info,
                                 const state_vector&         ntn_gateway_ecef,
                                 unsigned                    ntn_ul_sync_validity_dur,
                                 unsigned                    nof_steps = 5)
{
  ta_info_t ta_info = {0, 0, 0};

  // Make a copy of ephemeris_info to propagate it.
  orbit_ephemeris_info ephemeris_info = init_ephemeris_info;

  std::vector<double> t(nof_steps + 1);
  std::vector<double> y(nof_steps + 1);

  auto delta_t = std::chrono::duration<double>(static_cast<double>(ntn_ul_sync_validity_dur) / nof_steps);
  for (unsigned i = 0, e = nof_steps + 1; i != e; ++i) {
    if (i != 0) {
      ephemeris_info.propagate(delta_t, true);
    }
    state_vector rho = ntn_gateway_ecef - ephemeris_info.ecef_rv();
    double       r   = norm(rho.position);
    double       rtt = 2.0 * r / SPEED_OF_LIGHT_KM_S * 1e3;
    t[i]             = i * delta_t.count();
    y[i]             = rtt;
  }
  auto [c0, c1, c2] = fit_quadratic(t, y, y[0]);

  ta_info.ta_common               = std::clamp(c0, 0.0, 270730.0);
  ta_info.ta_common_drift         = std::clamp(c1, -51.4606, 51.4606);
  ta_info.ta_common_drift_variant = std::clamp(c2, 0.0, 0.57898);

  return ta_info;
}

ntn_orbital_compute_module::ntn_orbital_compute_module(orbit_propagator_type type) :
  logger(ocudulog::fetch_basic_logger("NTN"))
{
  if (type == orbit_propagator_type::keplerian) {
    orbit_propagator = std::make_unique<keplerian_propagator>();
  } else {
    orbit_propagator = std::make_unique<rk4_propagator>();
  }
}

bool ntn_orbital_compute_module::enqueue_ephemeris_info(const ephemeris_info_update& info)
{
  if (const auto* pos_vel = std::get_if<ecef_coordinates_t>(&info.ephemeris_info)) {
    state_vector ecef_rv;
    ecef_rv.position.x = pos_vel->position_x;
    ecef_rv.position.y = pos_vel->position_y;
    ecef_rv.position.z = pos_vel->position_z;
    ecef_rv.velocity.x = pos_vel->velocity_vx;
    ecef_rv.velocity.y = pos_vel->velocity_vy;
    ecef_rv.velocity.z = pos_vel->velocity_vz;
    orbit_ephemeris_info ephemeris_info{*orbit_propagator, info.epoch_time, ecef_rv, false};
    return ephemeris_info_queue.try_push(ephemeris_info);
  }

  const auto*      orbital_coordinates = std::get_if<orbital_coordinates_t>(&info.ephemeris_info);
  orbital_elements oe;
  oe.semi_major_axis = orbital_coordinates->semi_major_axis;
  oe.eccentricity    = orbital_coordinates->eccentricity;
  oe.inclination     = orbital_coordinates->inclination;
  oe.longitude       = orbital_coordinates->longitude;
  oe.periapsis       = orbital_coordinates->periapsis;
  oe.mean_anomaly    = orbital_coordinates->mean_anomaly;

  orbit_ephemeris_info ephemeris_info{*orbit_propagator, info.epoch_time, oe};
  return ephemeris_info_queue.try_push(ephemeris_info);
}

bool ntn_orbital_compute_module::enqueue_ntn_gw_location(ntn_gateway_location_info& ntn_gw_loc_update)
{
  if (!ntn_gw_loc_update.ntn_gateway_ecef_location) {
    ntn_gw_loc_update.ntn_gateway_ecef_location =
        coordinate_converter::geodetic_to_ecef(ntn_gw_loc_update.ntn_gateway_location.latitude,
                                               ntn_gw_loc_update.ntn_gateway_location.longitude,
                                               ntn_gw_loc_update.ntn_gateway_location.altitude);
  }
  return ntn_gateway_queue.try_push(ntn_gw_loc_update);
}

orbit_ephemeris_info* ntn_orbital_compute_module::get_ephemeris_info(time_point t)
{
  while (!ephemeris_info_queue.empty()) {
    orbit_ephemeris_info& first = *ephemeris_info_queue.begin();

    if (ephemeris_info_queue.size() == 1) {
      // No later entry to fall back on; propagators support propagating backward in time, so serve it regardless of
      // whether t precedes the reference time already reached by a previous (in-place) alignment.
      return &first;
    }

    orbit_ephemeris_info& second = *(ephemeris_info_queue.begin() + 1);

    if (t < second.reference_time()) {
      return &first;
    }

    // We're past the second element's reference time; discard the first and re-check.
    ephemeris_info_queue.pop();
  }
  return nullptr;
}

const ntn_gateway_location_info* ntn_orbital_compute_module::get_ntn_gateway_location_info(time_point t)
{
  if (ntn_gateway_queue.empty()) {
    return nullptr;
  }

  while (!ntn_gateway_queue.empty()) {
    const ntn_gateway_location_info& first_loc_info = *ntn_gateway_queue.begin();

    // If there's only one element, only take it if t0 is after its epoch time
    if (ntn_gateway_queue.size() == 1) {
      if (t >= first_loc_info.service_start_time) {
        return &first_loc_info;
      }
      return nullptr;
    }

    // For multiple elements, find the first one that satisfies the time window condition.
    const ntn_gateway_location_info& next_ephemeris_info = *(ntn_gateway_queue.begin() + 1);
    if (t >= first_loc_info.service_start_time && t < next_ephemeris_info.service_start_time) {
      return &first_loc_info;
    }

    // Remove the first element, if the 2nd one can be already used.
    if (t >= next_ephemeris_info.service_start_time) {
      ntn_gateway_queue.pop();
    } else {
      return nullptr;
    }
  }
  return nullptr;
}

ntn_orbital_state ntn_orbital_compute_module::compute_orbital_state(time_point epoch_time,
                                                                    unsigned   ntn_ul_sync_validity_dur,
                                                                    bool       use_state_vector)
{
  ntn_orbital_state state;
  state.success    = false;
  state.epoch_time = epoch_time;

  // Cannot generate NTN config without ephemeris info.
  if (ephemeris_info_queue.empty()) {
    return state;
  }

  orbit_ephemeris_info* ephemeris_info = get_ephemeris_info(epoch_time);
  if (ephemeris_info == nullptr) {
    return state;
  }

  // Propagate ephemeris info until the expected SIB19 Tx time.
  auto propagation_duration = epoch_time - ephemeris_info->epoch_time();
  ephemeris_info->propagate(propagation_duration);

  // Align ECI with ECEF reference frame at epoch time.
  ephemeris_info->align_reference_frames();

  if (use_state_vector) {
    const state_vector& ecef_rv = ephemeris_info->ecef_rv();
    ecef_coordinates_t  ecef_ephemeris_info;
    ecef_ephemeris_info.position_x  = ecef_rv.position.x;
    ecef_ephemeris_info.position_y  = ecef_rv.position.y;
    ecef_ephemeris_info.position_z  = ecef_rv.position.z;
    ecef_ephemeris_info.velocity_vx = ecef_rv.velocity.x;
    ecef_ephemeris_info.velocity_vy = ecef_rv.velocity.y;
    ecef_ephemeris_info.velocity_vz = ecef_rv.velocity.z;
    state.ephemeris_info            = ecef_ephemeris_info;
  } else {
    const orbital_elements& oe = ephemeris_info->oe();
    orbital_coordinates_t   orbital_ephemeris_info;
    orbital_ephemeris_info.semi_major_axis = oe.semi_major_axis;
    orbital_ephemeris_info.eccentricity    = oe.eccentricity;
    orbital_ephemeris_info.inclination     = oe.inclination;
    orbital_ephemeris_info.longitude       = oe.longitude;
    orbital_ephemeris_info.periapsis       = oe.periapsis;
    orbital_ephemeris_info.mean_anomaly    = oe.mean_anomaly;
    state.ephemeris_info                   = orbital_ephemeris_info;
  }

  // If a gateway is queued (but no override), look up the entry for this epoch.
  // If no valid entry is found yet, proceed without TA-info (e.g. regenerative architecture).
  const ntn_gateway_location_info* ntn_gateway_location = nullptr;
  if (not ta_info_override and not ntn_gateway_queue.empty()) {
    ntn_gateway_location = get_ntn_gateway_location_info(epoch_time);
  }

  if (ta_info_override) {
    state.ta_info = ta_info_override;
  } else if (ntn_gateway_location != nullptr) {
    state_vector ntn_gateway_ecef = *ntn_gateway_location->ntn_gateway_ecef_location;
    state.ta_info                 = compute_ta_info(*ephemeris_info, ntn_gateway_ecef, ntn_ul_sync_validity_dur);
  }

  state.success = true;
  return state;
}
