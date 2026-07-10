// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/ntn.h"
#include <chrono>
#include <string>
#include <vector>
#include <yaml-cpp/yaml.h>

namespace ocudu {

struct ntn_satellite_config;

/// Convert time_point to ISO 8601 format string (YYYY-MM-DDTHH:MM:SS.mmm).
std::string ntn_timepoint_to_iso8601(const std::chrono::system_clock::time_point& tp);

/// Builds a YAML node with the given geodetic coordinates.
YAML::Node build_geodetic_yaml_node(const geodetic_coordinates_t& loc, bool with_altitude = true);

/// Builds a YAML node with the given NTN polarization info.
YAML::Node build_ntn_polarization_yaml_node(const ntn_polarization_t& pol);

/// Fills the given node with the values of a satellite reference or definition.
void fill_ntn_satellite_in_yaml_schema(YAML::Node& node, const ntn_satellite_config& config);

/// Fills the ntn.satellites section of the given node.
void fill_ntn_satellites_in_yaml_schema(YAML::Node& node, const std::vector<ntn_satellite_config>& satellites);

} // namespace ocudu
