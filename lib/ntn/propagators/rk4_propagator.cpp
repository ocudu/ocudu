// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "rk4_propagator.h"
#include "ocudu/support/ocudu_assert.h"
#include <cmath>

using namespace ocudu;
using namespace ocudu_ntn;

/// Earth's gravitational constant [m^3/s^2]
static constexpr double MU_EARTH_M3_S2 = 3.986004418e14;
/// Earth's radius in meters.
static constexpr double EARTH_RADIUS_M = 6378137.0;
/// Earth's J2 coefficient.
static constexpr double J2 = 1.08262668e-3;

rk4_propagator::rk4_propagator(std::chrono::duration<double> time_step_s) :
  ordinary_differential_equation(time_step_s.count())
{
}

void rk4_propagator::propagate(orbit_ephemeris_info& initial_orbit, std::chrono::duration<double> propagation_time)
{
  double total_propagation_time_s = propagation_time.count();

  if (total_propagation_time_s == 0.0) {
    return;
  }

  // Direction: +1 for forward in time, -1 for backward.
  const double direction   = (total_propagation_time_s > 0.0) ? 1.0 : -1.0;
  double       time_step_s = direction * default_step_width_s;

  // Set up initial state vector [x, y, z, vx, vy, vz].
  std::array<double, 6> initial_state = {initial_orbit.eci_rv().position.x,
                                         initial_orbit.eci_rv().position.y,
                                         initial_orbit.eci_rv().position.z,
                                         initial_orbit.eci_rv().velocity.x,
                                         initial_orbit.eci_rv().velocity.y,
                                         initial_orbit.eci_rv().velocity.z};

  // Set up ODE solver (use inherited methods).
  set_initial_state(0.0, initial_state);
  set_step_width(time_step_s);

  // Propagate until the state is within one step of the target time (in the chosen direction).
  double propagation_time_s = 0.0;
  while (direction * (total_propagation_time_s - propagation_time_s - time_step_s) > 1.0e-6) {
    update();
    propagation_time_s += time_step_s;
  }

  // Final step to reach the exact end time (can be positive or negative).
  time_step_s = total_propagation_time_s - propagation_time_s;
  set_step_width(time_step_s);
  update();
  propagation_time_s += time_step_s;

  // Create new orbit state.
  state_vector new_state;
  const auto&  cur_state = get_state();
  ocudu_assert(cur_state.size() >= 6, "Current state must have at least 6 elements.");
  new_state.position = {cur_state[0], cur_state[1], cur_state[2]};
  new_state.velocity = {cur_state[3], cur_state[4], cur_state[5]};

  auto duration = std::chrono::duration_cast<std::chrono::system_clock::duration>(
      std::chrono::duration<double>(propagation_time_s));
  time_point current_epoch = initial_orbit.epoch_time() + duration;
  initial_orbit.set_state(new_state, true, current_epoch);
}

coord_3d rk4_propagator::calculate_acceleration(const coord_3d& position) const
{
  // Calculate distance from Earth's center.
  double r = norm(position);

  // Central gravity acceleration components.
  coord_3d acceleration;
  acceleration.x = -MU_EARTH_M3_S2 * position.x / (r * r * r);
  acceleration.y = -MU_EARTH_M3_S2 * position.y / (r * r * r);
  acceleration.z = -MU_EARTH_M3_S2 * position.z / (r * r * r);

  // Compute and add J2 acceleration components.
  double r2     = r * r;
  double r5     = r2 * r2 * r;
  double z2     = position.z * position.z;
  double factor = 1.5 * J2 * MU_EARTH_M3_S2 * (EARTH_RADIUS_M * EARTH_RADIUS_M) / r5;

  acceleration.x += factor * position.x * (5.0 * z2 / r2 - 1.0);
  acceleration.y += factor * position.y * (5.0 * z2 / r2 - 1.0);
  acceleration.z += factor * position.z * (5.0 * z2 / r2 - 3.0);

  return acceleration;
}

ordinary_differential_equation::ode_state_vector
rk4_propagator::derivative_function(double t, const ode_state_vector& cur_state) const
{
  // Extract position and velocity from state vector.
  ocudu_assert(cur_state.size() >= 6, "Current state must have at least 6 elements.");
  coord_3d position = {cur_state[0], cur_state[1], cur_state[2]};
  coord_3d velocity = {cur_state[3], cur_state[4], cur_state[5]};

  // Calculate acceleration.
  coord_3d acceleration = calculate_acceleration(position);

  // Return derivative vector [vx, vy, vz, ax, ay, az].
  return {velocity.x, velocity.y, velocity.z, acceleration.x, acceleration.y, acceleration.z};
}
