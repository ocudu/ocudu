// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "reference_frame_converter.h"
#include <cmath>
#include <optional>

using namespace ocudu;
using namespace ocudu_ntn;

/// Earth's rotation rate in rad/s
static constexpr double EARTH_ROTATION_RATE = 7.292115146706979e-5;

reference_frame_converter::reference_frame_converter(std::chrono::system_clock::time_point epoch_time_) :
  epoch_time(epoch_time_)
{
}

state_vector
reference_frame_converter::ecef_to_eci(const state_vector&                                  ecef_state,
                                       std::optional<std::chrono::system_clock::time_point> current_time) const
{
  using namespace std::chrono;
  auto   ref_time  = current_time.value_or(epoch_time);
  double dt        = duration_cast<duration<double>>(ref_time - epoch_time).count();
  double angle     = EARTH_ROTATION_RATE * dt;
  double cos_angle = std::cos(angle);
  double sin_angle = std::sin(angle);

  const auto& ecef_pos = ecef_state.position;
  const auto& ecef_vel = ecef_state.velocity;

  // Create state_vector and fill position and velocity.
  state_vector eci_rv;
  eci_rv.position.x = ecef_pos.x * cos_angle - ecef_pos.y * sin_angle;
  eci_rv.position.y = ecef_pos.x * sin_angle + ecef_pos.y * cos_angle;
  eci_rv.position.z = ecef_pos.z;
  eci_rv.velocity.x = ecef_vel.x * cos_angle - ecef_vel.y * sin_angle - EARTH_ROTATION_RATE * ecef_pos.y * cos_angle -
                      EARTH_ROTATION_RATE * ecef_pos.x * sin_angle;
  eci_rv.velocity.y = ecef_vel.x * sin_angle + ecef_vel.y * cos_angle + EARTH_ROTATION_RATE * ecef_pos.x * cos_angle -
                      EARTH_ROTATION_RATE * ecef_pos.y * sin_angle;
  eci_rv.velocity.z = ecef_vel.z;

  return eci_rv;
}

state_vector
reference_frame_converter::eci_to_ecef(const state_vector&                                  eci_state,
                                       std::optional<std::chrono::system_clock::time_point> current_time) const
{
  using namespace std::chrono;
  auto   ref_time  = current_time.value_or(epoch_time);
  double dt        = duration_cast<duration<double>>(ref_time - epoch_time).count();
  double angle     = -EARTH_ROTATION_RATE * dt;
  double cos_angle = std::cos(angle);
  double sin_angle = std::sin(angle);

  const auto& eci_pos = eci_state.position;
  const auto& eci_vel = eci_state.velocity;

  // Create state_vector and fill position and velocity.
  state_vector ecef_rv;
  ecef_rv.position.x = eci_pos.x * cos_angle - eci_pos.y * sin_angle;
  ecef_rv.position.y = eci_pos.x * sin_angle + eci_pos.y * cos_angle;
  ecef_rv.position.z = eci_pos.z;
  ecef_rv.velocity.x = eci_vel.x * cos_angle - eci_vel.y * sin_angle + EARTH_ROTATION_RATE * eci_pos.y * cos_angle +
                       EARTH_ROTATION_RATE * eci_pos.x * sin_angle;
  ecef_rv.velocity.y = eci_vel.x * sin_angle + eci_vel.y * cos_angle - EARTH_ROTATION_RATE * eci_pos.x * cos_angle +
                       EARTH_ROTATION_RATE * eci_pos.y * sin_angle;
  ecef_rv.velocity.z = eci_vel.z;

  return ecef_rv;
}
