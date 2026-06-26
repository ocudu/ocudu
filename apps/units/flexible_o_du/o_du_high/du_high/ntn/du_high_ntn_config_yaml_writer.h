// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include <vector>
#include <yaml-cpp/yaml.h>

namespace ocudu {

struct du_high_unit_cell_ntn_config;
struct du_high_unit_ntn_satellite_config;

/// Fills the given node with the NTN configuration values.
void fill_ntn_config_in_yaml_schema(YAML::Node& node, const du_high_unit_cell_ntn_config& config);

/// Fills the ntn.satellites section of the given node.
void fill_ntn_satellites_in_yaml_schema(YAML::Node&                                           node,
                                        const std::vector<du_high_unit_ntn_satellite_config>& satellites);

} // namespace ocudu
