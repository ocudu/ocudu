// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../coordinates_types.h"
#include "../orbit_ephemeris_info.h"
#include "orbital_propagator_model.h"
#include "ordinary_differential_equation.h"

namespace ocudu {
namespace ocudu_ntn {

/// \brief RK4 propagator class for orbital propagation.
/// \details This class implements the Runge-Kutta 4th order method for orbital propagation.
class rk4_propagator : public orbital_propagation_model, public ordinary_differential_equation
{
public:
  using time_point = std::chrono::system_clock::time_point;

  /// \brief Constructor.
  ///
  /// \param time_step_s Time step for the ODE solver in seconds.
  explicit rk4_propagator(std::chrono::duration<double> time_step_s = std::chrono::duration<double>(1.0));

  /// \brief Propagate orbit using RK4 method.
  ///
  /// \param initial_orbit_state Initial orbit state to be propagated.
  /// \param propagation_time Propagate the ephemeris info for a given duration.
  void propagate(orbit_ephemeris_info& initial_orbit_state, std::chrono::duration<double> propagation_time) override;

private:
  /// \brief Calculate gravitational acceleration.
  ///
  /// \param position Position vector in ECI frame.
  /// \return Acceleration vector in ECI frame.
  coord_3d calculate_acceleration(const coord_3d& position) const;

  /// \brief Calculate derivative of state vector.
  ///
  /// \param t Current time.
  /// \param state Current state vector.
  /// \return Derivative of state vector.
  ode_state_vector derivative_function(double t, const ode_state_vector& state) const override;
};

} // namespace ocudu_ntn
} // namespace ocudu
