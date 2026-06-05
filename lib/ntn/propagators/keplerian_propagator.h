// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../orbit_ephemeris_info.h"
#include "orbital_propagator_model.h"

namespace ocudu {
namespace ocudu_ntn {

/// \brief Keplerian (two-body) orbit propagator.
/// \details Analytically advances mean anomaly using n = sqrt(mu/a^3).
///          No perturbations (J2, drag) are modelled.
class keplerian_propagator : public orbital_propagation_model
{
public:
  void propagate(orbit_ephemeris_info& orbit, std::chrono::duration<double> propagation_time) override;
};

} // namespace ocudu_ntn
} // namespace ocudu
