// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ntn/orbit_propagator_type.h"
#include "ocudu/ran/ntn.h"
#include <chrono>
#include <optional>
#include <variant>
#include <vector>

namespace ocudu {

/// Application-level global satellite configuration entry.
/// Defines orbital state for one satellite, identified by a user-chosen satellite_idx.
struct du_high_unit_ntn_satellite_config {
  unsigned                                                satellite_idx = 0;
  std::optional<std::chrono::system_clock::time_point>    epoch_timestamp;
  std::variant<ecef_coordinates_t, orbital_coordinates_t> ephemeris_info;
  std::optional<geodetic_coordinates_t>                   gateway_location;
  std::optional<ta_info_t>                                ta_info;
  ocudu_ntn::orbit_propagator_type                        propagator_type = ocudu_ntn::orbit_propagator_type::rk4;
};

} // namespace ocudu
