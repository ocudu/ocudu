// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

namespace ocudu {
namespace ocudu_ntn {

/// Earth's gravitational parameter [m^3/s^2].
static constexpr double MU_EARTH_M3_S2 = 3.986004418e14;
/// Earth's mean equatorial radius [m].
static constexpr double EARTH_RADIUS_M = 6378137.0;
/// Earth's J2 zonal harmonic coefficient.
static constexpr double J2 = 1.08262668e-3;

} // namespace ocudu_ntn
} // namespace ocudu
