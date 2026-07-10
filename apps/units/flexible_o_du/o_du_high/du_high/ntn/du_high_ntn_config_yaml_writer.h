// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include <vector>
#include <yaml-cpp/yaml.h>

namespace ocudu {

struct du_high_unit_cell_ntn_config;

/// Fills the given node with the NTN configuration values.
void fill_ntn_config_in_yaml_schema(YAML::Node& node, const du_high_unit_cell_ntn_config& config);

} // namespace ocudu
