// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ephemeris_info_converter.h"
#include <array>
#include <cmath>

using namespace ocudu;
using namespace ocudu_ntn;

/// Earth's gravitational parameter (GM) [m^3/s^2].
static constexpr double MU = 3.986004418e14;

/// Solve Kepler's equation for eccentric anomaly.
static double solve_kepler_equation(double M, double e, double tolerance = 1e-12, unsigned max_iterations = 100)
{
  double E = (e < 0.8) ? M : M_PI;
  for (unsigned i = 0; i != max_iterations; ++i) {
    double f       = E - e * std::sin(E) - M;
    double f_prime = 1.0 - e * std::cos(E);
    double delta   = f / f_prime;
    E -= delta;
    if (std::abs(delta) < tolerance)
      break;
  }
  return E;
}

orbital_elements ephemeris_info_converter::eci_to_orbital(const state_vector& eci_state)
{
  const auto& position = eci_state.position;
  const auto& velocity = eci_state.velocity;
  // Specific angular momentum.
  std::array<double, 3> h = {position.y * velocity.z - position.z * velocity.y,
                             position.z * velocity.x - position.x * velocity.z,
                             position.x * velocity.y - position.y * velocity.x};

  // Node vector.
  std::array<double, 3> n = {-h[1], h[0], 0.0};

  // Eccentricity vector.
  double                v2 = velocity.x * velocity.x + velocity.y * velocity.y + velocity.z * velocity.z;
  double                r  = std::sqrt(position.x * position.x + position.y * position.y + position.z * position.z);
  double                r_dot_v = position.x * velocity.x + position.y * velocity.y + position.z * velocity.z;
  std::array<double, 3> ev      = {((v2 - MU / r) * position.x - r_dot_v * velocity.x) / MU,
                                   ((v2 - MU / r) * position.y - r_dot_v * velocity.y) / MU,
                                   ((v2 - MU / r) * position.z - r_dot_v * velocity.z) / MU};

  // Semi-major axis.
  double a = -MU / (2.0 * (v2 / 2.0 - MU / r));

  // Eccentricity.
  double eccentricity = std::sqrt(ev[0] * ev[0] + ev[1] * ev[1] + ev[2] * ev[2]);

  // Inclination.
  double h_mag       = std::sqrt(h[0] * h[0] + h[1] * h[1] + h[2] * h[2]);
  double inclination = std::acos(h[2] / h_mag);

  // Longitude of ascending node.
  double n_mag     = std::sqrt(n[0] * n[0] + n[1] * n[1]);
  double longitude = std::atan2(n[1], n[0]);
  if (longitude < 0)
    longitude += 2.0 * M_PI;

  // Argument of periapsis.
  double ev_mag    = std::sqrt(ev[0] * ev[0] + ev[1] * ev[1] + ev[2] * ev[2]);
  double periapsis = std::acos((n[0] * ev[0] + n[1] * ev[1]) / (n_mag * ev_mag));
  if (ev[2] < 0)
    periapsis = 2.0 * M_PI - periapsis;

  // True anomaly.
  double true_anomaly = std::acos((ev[0] * position.x + ev[1] * position.y + ev[2] * position.z) / (ev_mag * r));
  if (r_dot_v < 0)
    true_anomaly = 2.0 * M_PI - true_anomaly;

  // Eccentric anomaly.
  double E = 2.0 * std::atan(std::sqrt((1.0 - eccentricity) / (1.0 + eccentricity)) * std::tan(true_anomaly / 2.0));

  // Mean anomaly.
  double mean_anomaly = E - eccentricity * std::sin(E);
  if (mean_anomaly < 0)
    mean_anomaly += 2.0 * M_PI;

  return {a, eccentricity, inclination, longitude, periapsis, mean_anomaly};
}

state_vector ephemeris_info_converter::orbital_to_eci(const orbital_elements& params)
{
  // Unpack parameters.
  double a     = params.semi_major_axis;
  double e     = params.eccentricity;
  double i     = params.inclination;
  double Omega = params.longitude;
  double omega = params.periapsis;
  double M     = params.mean_anomaly;

  // Solve Kepler's equation for eccentric anomaly.
  double E = solve_kepler_equation(M, e);

  // True anomaly.
  double true_anomaly = 2.0 * std::atan2(std::sqrt(1 + e) * std::sin(E / 2.0), std::sqrt(1 - e) * std::cos(E / 2.0));

  // Distance.
  double r = a * (1 - e * std::cos(E));
  double p = a * (1 - e * e);

  // Position in perifocal (PQW) frame.
  double x_p = r * std::cos(true_anomaly);
  double y_p = r * std::sin(true_anomaly);
  double z_p = 0.0;

  // Velocity in perifocal (PQW) frame.
  double mu   = MU;
  double vx_p = -std::sqrt(mu / p) * std::sin(true_anomaly);
  double vy_p = std::sqrt(mu / p) * (e + std::cos(true_anomaly));
  double vz_p = 0.0;

  // Rotation matrix components.
  double cos_O = std::cos(Omega), sin_O = std::sin(Omega);
  double cos_i = std::cos(i), sin_i = std::sin(i);
  double cos_w = std::cos(omega), sin_w = std::sin(omega);

  // Perifocal to ECI transformation.
  double R11 = cos_O * cos_w - sin_O * sin_w * cos_i;
  double R12 = -cos_O * sin_w - sin_O * cos_w * cos_i;
  double R13 = sin_O * sin_i;
  double R21 = sin_O * cos_w + cos_O * sin_w * cos_i;
  double R22 = -sin_O * sin_w + cos_O * cos_w * cos_i;
  double R23 = -cos_O * sin_i;
  double R31 = sin_w * sin_i;
  double R32 = cos_w * sin_i;
  double R33 = cos_i;

  // Create state_vector and fill position and velocity.
  state_vector rv;
  rv.position.x = R11 * x_p + R12 * y_p + R13 * z_p;
  rv.position.y = R21 * x_p + R22 * y_p + R23 * z_p;
  rv.position.z = R31 * x_p + R32 * y_p + R33 * z_p;
  rv.velocity.x = R11 * vx_p + R12 * vy_p + R13 * vz_p;
  rv.velocity.y = R21 * vx_p + R22 * vy_p + R23 * vz_p;
  rv.velocity.z = R31 * vx_p + R32 * vy_p + R33 * vz_p;

  return rv;
}
