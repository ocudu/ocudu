// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ordinary_differential_equation.h"

using namespace ocudu;
using namespace ocudu_ntn;

ordinary_differential_equation::ordinary_differential_equation(double time_step_s) :
  time(0), default_step_width_s(time_step_s)
{
}

void ordinary_differential_equation::set_initial_state(double initial_time_s, const ode_state_vector& initial_state)
{
  time  = initial_time_s;
  state = initial_state;
}

void ordinary_differential_equation::set_step_width(double step_width_s_)
{
  step_width_s = step_width_s_;
}

void ordinary_differential_equation::update()
{
  // Calculate k1.
  ode_state_vector k1 = derivative_function(time, state);

  // Calculate k2.
  ode_state_vector state_k2 = state;
  for (unsigned i = 0, e = state.size(); i != e; ++i) {
    state_k2[i] += 0.5 * step_width_s * k1[i];
  }
  ode_state_vector k2 = derivative_function(time + 0.5 * step_width_s, state_k2);

  // Calculate k3.
  ode_state_vector state_k3 = state;
  for (unsigned i = 0, e = state.size(); i != e; ++i) {
    state_k3[i] += 0.5 * step_width_s * k2[i];
  }
  ode_state_vector k3 = derivative_function(time + 0.5 * step_width_s, state_k3);

  // Calculate k4.
  ode_state_vector state_k4 = state;
  for (unsigned i = 0, e = state.size(); i != e; ++i) {
    state_k4[i] += step_width_s * k3[i];
  }
  ode_state_vector k4 = derivative_function(time + step_width_s, state_k4);

  // Update state.
  for (unsigned i = 0, e = state.size(); i != e; ++i) {
    state[i] += (step_width_s / 6.0) * (k1[i] + 2.0 * k2[i] + 2.0 * k3[i] + k4[i]);
  }

  // Update time.
  time += step_width_s;
}
