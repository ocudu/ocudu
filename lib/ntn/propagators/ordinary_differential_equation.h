// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include <array>

namespace ocudu {
namespace ocudu_ntn {

/// \brief Base class for Ordinary Differential Equation solvers.
class ordinary_differential_equation
{
public:
  /// Type alias for ODE state vector.
  using ode_state_vector = std::array<double, 6>;

  /// \brief Constructor.
  ///
  /// \param time_step_s Time step in seconds.
  explicit ordinary_differential_equation(double time_step_s);

  virtual ~ordinary_differential_equation() = default;

  /// \brief Set the ODE solver with initial conditions.
  ///
  /// \param initial_time_s Initial time in seconds.
  /// \param initial_state Initial state vector.
  void set_initial_state(double initial_time_s, const ode_state_vector& initial_state);

  /// \brief Set the step width for integration.
  ///
  /// \param step_width_s Step width in seconds.
  void set_step_width(double step_width_s);

  /// \brief Get the current state vector.
  ///
  /// \return Current state vector.
  const ode_state_vector& get_state() const { return state; }

  /// \brief Update the state using the integration method.
  void update();

protected:
  /// \brief Calculate the derivative of the state vector.
  ///
  /// \param t Current time.
  /// \param state Current state vector.
  /// \return Derivative of the state vector.
  virtual ode_state_vector derivative_function(double t, const ode_state_vector& state) const = 0;

  /// Current time in seconds.
  double time;

  /// Current state vector.
  ode_state_vector state;

  /// Initial step width in seconds.
  double default_step_width_s;

  /// Current step width in seconds.
  double step_width_s;
};

} // namespace ocudu_ntn
} // namespace ocudu
