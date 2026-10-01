// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include <yaml-cpp/yaml.h>

namespace ocudu {

struct split_7_2_o_du_unit_config;

/// Fills the given node with the split 7.2 O-RAN DU configuration values.
void fill_split_7_2_o_du_unit_config_in_yaml_schema(YAML::Node& node, const split_7_2_o_du_unit_config& config);

} // namespace ocudu
