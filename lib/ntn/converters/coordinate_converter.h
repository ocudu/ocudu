// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../coordinates_types.h"

namespace ocudu {
namespace ocudu_ntn {

/// Tools to convert between geodetic and ECEF coordinates.
namespace coordinate_converter {

/// \brief Convert geodetic coordinates to ECEF coordinates.
///
/// \param lat_deg Latitude in degrees
/// \param lon_deg Longitude in degrees
/// \param alt_m Altitude in meters
/// \return state_vector with ECEF position [x, y, z] in meters, velocity set to zero
state_vector geodetic_to_ecef(double lat_deg, double lon_deg, double alt_m);

/// \brief Convert geodetic_coordinates to ECEF state_vector.
///
/// \param geo geodetic_coordinates (latitude [deg], longitude [deg], altitude [m])
/// \return state_vector with ECEF position [x, y, z] in meters, velocity set to zero
state_vector geodetic_to_ecef(const geodetic_coordinates& geo);

/// \brief Convert ECEF state vector to geodetic coordinates.
///
/// \param ecef_state state_vector with ECEF position [x, y, z] in meters
/// \return geodetic_coordinates (latitude [deg], longitude [deg], altitude [m])
geodetic_coordinates ecef_to_geodetic(const state_vector& ecef_state);

} // namespace coordinate_converter
} // namespace ocudu_ntn
} // namespace ocudu
