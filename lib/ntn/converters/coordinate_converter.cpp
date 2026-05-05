// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "coordinate_converter.h"
#include <cmath>

using namespace ocudu;
using namespace ocudu_ntn;

/// Earth's radius in meters.
static constexpr double EARTH_RADIUS_M = 6378137.0;
/// Earth's eccentricity squared.
static constexpr double EARTH_ECCENTRICITY_2 = 0.006694379990141316;

/// \brief Converts degrees to radians.
/// \param deg angle in degrees
/// \return angle in radians
static constexpr double deg_to_rad(double deg)
{
  return deg * M_PI / 180.0;
}

/// \brief Converts radians to degrees.
/// \param rad angle in radians
/// \return angle in degrees
static constexpr double rad_to_deg(double rad)
{
  return rad * 180.0 / M_PI;
}

state_vector coordinate_converter::geodetic_to_ecef(double lat_deg, double lon_deg, double alt_m)
{
  // Convert degrees to radians.
  double lat_rad = deg_to_rad(lat_deg);
  double lon_rad = deg_to_rad(lon_deg);

  // Calculate trigonometric values.
  double sin_lat = std::sin(lat_rad);
  double cos_lat = std::cos(lat_rad);
  double sin_lon = std::sin(lon_rad);
  double cos_lon = std::cos(lon_rad);

  // Calculate radius of curvature in the prime vertical.
  double N = EARTH_RADIUS_M / std::sqrt(1.0 - EARTH_ECCENTRICITY_2 * sin_lat * sin_lat);

  // Calculate ECEF coordinates.
  double x = (N + alt_m) * cos_lat * cos_lon;
  double y = (N + alt_m) * cos_lat * sin_lon;
  double z = (N * (1 - EARTH_ECCENTRICITY_2) + alt_m) * sin_lat;

  return state_vector{{x, y, z}, {0.0, 0.0, 0.0}};
}

state_vector coordinate_converter::geodetic_to_ecef(const geodetic_coordinates& geo)
{
  return coordinate_converter::geodetic_to_ecef(geo.latitude_deg, geo.longitude_deg, geo.altitude_m);
}

geodetic_coordinates coordinate_converter::ecef_to_geodetic(const state_vector& ecef_state)
{
  constexpr double a  = EARTH_RADIUS_M;
  constexpr double e2 = EARTH_ECCENTRICITY_2;

  const auto& pos = ecef_state.position;
  double      lon = std::atan2(pos.y, pos.x);
  double      p   = std::sqrt(pos.x * pos.x + pos.y * pos.y);

  // Iterative computation for latitude.
  double lat = std::atan2(pos.z, p * (1.0 - e2));
  double lat_prev;
  double N;
  int    iter = 0;
  do {
    lat_prev = lat;
    N        = a / std::sqrt(1.0 - e2 * std::sin(lat) * std::sin(lat));
    lat      = std::atan2(pos.z + e2 * N * std::sin(lat), p);
    ++iter;
  } while (std::abs(lat - lat_prev) > 1e-12 && iter < 10);

  N          = a / std::sqrt(1.0 - e2 * std::sin(lat) * std::sin(lat));
  double alt = p / std::cos(lat) - N;

  // Convert radians to degrees.
  double lat_deg = rad_to_deg(lat);
  double lon_deg = rad_to_deg(lon);

  return geodetic_coordinates{lat_deg, lon_deg, alt};
}
