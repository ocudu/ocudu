// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../coordinates_types.h"

namespace ocudu {
namespace ocudu_ntn {

/// Tools to converter between ECI state vectors and orbital parameters.
namespace ephemeris_info_converter {

/// \brief Convert ECI state vector to orbital parameters.
///
/// \param eci_state State vector in ECI frame (position [m], velocity [m/s])
/// \return orbital_elements struct with classical orbital elements
orbital_elements eci_to_orbital(const state_vector& eci_state);

/// \brief Convert orbital parameters to ECI state vector.
///
/// \param params orbital_elements struct
/// \return state_vector in ECI frame
state_vector orbital_to_eci(const orbital_elements& params);

} // namespace ephemeris_info_converter
} // namespace ocudu_ntn
} // namespace ocudu
