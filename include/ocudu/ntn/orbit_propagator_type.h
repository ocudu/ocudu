// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

namespace ocudu {
namespace ocudu_ntn {

/// Orbit propagator type for NTN ephemeris propagation.
enum class orbit_propagator_type { rk4, keplerian };

} // namespace ocudu_ntn
} // namespace ocudu
