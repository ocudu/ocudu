// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "keplerian_propagator.h"
#include "../converters/ephemeris_info_converter.h"
#include "ntn_orbital_constants.h"
#include <cmath>

using namespace ocudu;
using namespace ocudu_ntn;

void keplerian_propagator::propagate(orbit_ephemeris_info& orbit, std::chrono::duration<double> propagation_time)
{
  const double dt = propagation_time.count();
  if (dt == 0.0) {
    return;
  }

  // Get current orbital elements (a, e, i, RAAN, argp, M).
  // Returns cached OE when the orbit was initialised from orbital elements,
  // or computes via eci_to_orbital() when initialised from an ECI/ECEF state.
  orbital_elements oe = orbit.oe();

  // Advance mean anomaly: M_new = M + n*dt, n = sqrt(mu/a^3).
  const double n = std::sqrt(MU_EARTH_M3_S2 / (oe.semi_major_axis * oe.semi_major_axis * oe.semi_major_axis));
  oe.mean_anomaly += n * dt;
  oe.mean_anomaly = std::fmod(oe.mean_anomaly, 2.0 * M_PI);
  if (oe.mean_anomaly < 0.0) {
    oe.mean_anomaly += 2.0 * M_PI;
  }

  // Solve Kepler's equation (M -> E) and compute the ECI state vector
  // via the perifocal-frame rotation matrix.  Storing ECI state (not OE)
  // matches the RK4 propagator pattern and avoids deferred-conversion issues.
  const state_vector new_eci = ephemeris_info_converter::orbital_to_eci(oe);

  const auto duration =
      std::chrono::duration_cast<std::chrono::system_clock::duration>(std::chrono::duration<double>(dt));
  orbit.set_state(new_eci, true, orbit.epoch_time() + duration);
}
