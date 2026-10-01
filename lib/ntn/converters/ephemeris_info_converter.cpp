// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ephemeris_info_converter.h"
#include "ocudu/support/error_handling.h"
#include <array>
#include <cmath>

using namespace ocudu;
using namespace ocudu_ntn;

/// Earth's gravitational parameter (GM) [m^3/s^2].
static constexpr double MU = 3.986004418e14;

/// Wraps an angle to [0, 2*pi). Corrects a single wrap in either direction, which covers every std::atan2 result
/// and the anomalies derived from one.
static double wrap_2pi(double angle)
{
  double wrapped = (angle < 0.0) ? angle + 2.0 * M_PI : angle;
  // Adding 2*pi to a tiny negative angle rounds up to exactly 2*pi, and Kepler's equation can land a hair past
  // it; fold both back to 0 so the result is never the excluded endpoint.
  return (wrapped < 2.0 * M_PI) ? wrapped : 0.0;
}

/// Solve Kepler's equation M = E - e * sin(E) for the eccentric anomaly E, by Newton-Raphson iteration.
static double
solve_kepler_equation(double mean_anomaly, double eccentricity, double tolerance = 1e-12, unsigned max_iterations = 100)
{
  double eccentric_anomaly = (eccentricity < 0.8) ? mean_anomaly : M_PI;
  for (unsigned i = 0; i != max_iterations; ++i) {
    double f       = eccentric_anomaly - eccentricity * std::sin(eccentric_anomaly) - mean_anomaly;
    double f_prime = 1.0 - eccentricity * std::cos(eccentric_anomaly);
    double delta   = f / f_prime;
    eccentric_anomaly -= delta;
    if (std::abs(delta) < tolerance) {
      break;
    }
  }
  return eccentric_anomaly;
}

orbital_elements ephemeris_info_converter::eci_to_orbital(const state_vector& eci_state)
{
  const auto& position = eci_state.position;
  const auto& velocity = eci_state.velocity;

  // A non-finite component would spread through every expression below and yield a silently unusable element set.
  // The eccentricity check further down cannot stand in for this one, because every comparison against NaN is
  // false.
  if (!std::isfinite(position.x) || !std::isfinite(position.y) || !std::isfinite(position.z) ||
      !std::isfinite(velocity.x) || !std::isfinite(velocity.y) || !std::isfinite(velocity.z)) {
    report_error("eci_to_orbital requires a finite ECI state");
  }

  // Specific angular momentum.
  std::array<double, 3> h = {position.y * velocity.z - position.z * velocity.y,
                             position.z * velocity.x - position.x * velocity.z,
                             position.x * velocity.y - position.y * velocity.x};

  // Node vector.
  std::array<double, 3> n = {-h[1], h[0], 0.0};

  // Distance from the focus. A satellite at the origin has no orbit, and would make every MU / r below infinite.
  double r = std::sqrt(position.x * position.x + position.y * position.y + position.z * position.z);
  if (r == 0.0) {
    report_error("eci_to_orbital requires a non-zero ECI position");
  }

  // Eccentricity vector.
  double                v2      = velocity.x * velocity.x + velocity.y * velocity.y + velocity.z * velocity.z;
  double                r_dot_v = position.x * velocity.x + position.y * velocity.y + position.z * velocity.z;
  std::array<double, 3> ev      = {((v2 - MU / r) * position.x - r_dot_v * velocity.x) / MU,
                                   ((v2 - MU / r) * position.y - r_dot_v * velocity.y) / MU,
                                   ((v2 - MU / r) * position.z - r_dot_v * velocity.z) / MU};

  // Semi-major axis.
  double semi_major_axis = -MU / (2.0 * (v2 / 2.0 - MU / r));

  // Eccentricity.
  double eccentricity = std::sqrt(ev[0] * ev[0] + ev[1] * ev[1] + ev[2] * ev[2]);

  // This implementation calculates the elliptic eccentric and mean anomalies and therefore does not support parabolic
  // or hyperbolic orbits.
  if (eccentricity >= 1.0) {
    report_error("eci_to_orbital supports only elliptic orbits");
  }

  double h_mag = std::sqrt(h[0] * h[0] + h[1] * h[1] + h[2] * h[2]);
  double n_mag = std::sqrt(n[0] * n[0] + n[1] * n[1]);
  // Define ev_mag separately, as the circular test below is about the eccentricity vector vanishing, not about the
  // orbit being round.
  double ev_mag = eccentricity;

  // Two orbit geometries make the classical angle elements degenerate, because the vector each angle is measured from
  // vanishes: circular (ev_mag -> 0) leaves the argument of periapsis undefined, along with the true anomaly measured
  // from it, and equatorial (n_mag -> 0) leaves the ascending node undefined.
  constexpr double ecc_tol   = 1e-11;
  constexpr double sin_i_tol = 1e-11;

  const bool circular = ev_mag < ecc_tol;
  // n_mag / h_mag = |sin(inclination)|, so use a relative dimensionless equatorial test.
  const bool equatorial = n_mag < sin_i_tol * h_mag;

  // Inclination, from n_mag = h_mag * sin(inclination) against h[2] = h_mag * cos(inclination). Preferred over
  // acos(h[2] / h_mag) because that ratio rounds to exactly 1.0 for any inclination below sqrt(2 * epsilon), about
  // 2e-8 rad, collapsing every near-equatorial orbit onto the equator; n_mag carries the same information with no
  // loss of precision at small angles.
  double inclination = std::atan2(n_mag, h[2]);

  // Longitude of ascending node. Undefined for an equatorial orbit -> convention 0.
  double longitude = equatorial ? 0.0 : wrap_2pi(std::atan2(n[1], n[0]));

  // Required when the orbit is equatorial and retrograde.
  const double equatorial_direction = h[2] >= 0.0 ? 1.0 : -1.0;

  double periapsis;
  double true_anomaly;

  if (circular) {
    // No periapsis: argument of periapsis = 0 by convention; the whole in-plane phase goes into the anomaly.
    periapsis = 0.0;
    if (!equatorial) {
      // Argument of latitude: angle from the ascending node to the position vector.
      true_anomaly = wrap_2pi(std::atan2(position.z * h_mag, n[0] * position.x + n[1] * position.y));
    } else {
      // Circular and equatorial: true longitude.
      true_anomaly = wrap_2pi(std::atan2(equatorial_direction * position.y, position.x));
    }
  } else {
    // Argument of periapsis.
    if (!equatorial) {
      periapsis = wrap_2pi(std::atan2(ev[2] * h_mag, n[0] * ev[0] + n[1] * ev[1]));
    } else {
      // Equatorial and elliptical: longitude of periapsis, measured from the x-axis.
      periapsis = wrap_2pi(std::atan2(equatorial_direction * ev[1], ev[0]));
    }
    // True anomaly is well-defined whenever the orbit is not circular.
    true_anomaly =
        wrap_2pi(std::atan2(r_dot_v * h_mag / MU, ev[0] * position.x + ev[1] * position.y + ev[2] * position.z));
  }

  // Eccentric anomaly.
  double eccentric_anomaly = 2.0 * std::atan2(std::sqrt(1.0 - eccentricity) * std::sin(true_anomaly / 2.0),
                                              std::sqrt(1.0 + eccentricity) * std::cos(true_anomaly / 2.0));
  eccentric_anomaly        = wrap_2pi(eccentric_anomaly);

  // Mean anomaly, from Kepler's equation M = E - e * sin(E).
  double mean_anomaly = wrap_2pi(eccentric_anomaly - eccentricity * std::sin(eccentric_anomaly));

  return {semi_major_axis, eccentricity, inclination, longitude, periapsis, mean_anomaly};
}

state_vector ephemeris_info_converter::orbital_to_eci(const orbital_elements& oe)
{
  // Solve Kepler's equation for eccentric anomaly.
  double eccentric_anomaly = solve_kepler_equation(oe.mean_anomaly, oe.eccentricity);

  // True anomaly.
  double true_anomaly = 2.0 * std::atan2(std::sqrt(1 + oe.eccentricity) * std::sin(eccentric_anomaly / 2.0),
                                         std::sqrt(1 - oe.eccentricity) * std::cos(eccentric_anomaly / 2.0));

  // Orbital radius, i.e. the distance from the focus to the satellite.
  double r = oe.semi_major_axis * (1 - oe.eccentricity * std::cos(eccentric_anomaly));

  // Semi-latus rectum.
  double p = oe.semi_major_axis * (1 - oe.eccentricity * oe.eccentricity);

  // Position in perifocal (PQW) frame.
  double x_p = r * std::cos(true_anomaly);
  double y_p = r * std::sin(true_anomaly);
  double z_p = 0.0;

  // Velocity in perifocal (PQW) frame.
  double vx_p = -std::sqrt(MU / p) * std::sin(true_anomaly);
  double vy_p = std::sqrt(MU / p) * (oe.eccentricity + std::cos(true_anomaly));
  double vz_p = 0.0;

  // Rotation matrix components.
  double cos_raan = std::cos(oe.longitude);
  double sin_raan = std::sin(oe.longitude);
  double cos_i    = std::cos(oe.inclination);
  double sin_i    = std::sin(oe.inclination);
  double cos_w    = std::cos(oe.periapsis);
  double sin_w    = std::sin(oe.periapsis);

  // Perifocal to ECI transformation, the 3-1-3 rotation R_z(-raan) * R_x(-inclination) * R_z(-omega).
  double r11 = cos_raan * cos_w - sin_raan * sin_w * cos_i;
  double r12 = -cos_raan * sin_w - sin_raan * cos_w * cos_i;
  double r13 = sin_raan * sin_i;
  double r21 = sin_raan * cos_w + cos_raan * sin_w * cos_i;
  double r22 = -sin_raan * sin_w + cos_raan * cos_w * cos_i;
  double r23 = -cos_raan * sin_i;
  double r31 = sin_w * sin_i;
  double r32 = cos_w * sin_i;
  double r33 = cos_i;

  // Create state_vector and fill position and velocity.
  state_vector rv;
  rv.position.x = r11 * x_p + r12 * y_p + r13 * z_p;
  rv.position.y = r21 * x_p + r22 * y_p + r23 * z_p;
  rv.position.z = r31 * x_p + r32 * y_p + r33 * z_p;
  rv.velocity.x = r11 * vx_p + r12 * vy_p + r13 * vz_p;
  rv.velocity.y = r21 * vx_p + r22 * vy_p + r23 * vz_p;
  rv.velocity.z = r31 * vx_p + r32 * vy_p + r33 * vz_p;

  return rv;
}
