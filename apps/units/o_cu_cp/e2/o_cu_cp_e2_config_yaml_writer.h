// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include <yaml-cpp/yaml.h>

namespace ocudu {

struct o_cu_cp_e2_config;

/// Fills the given node with the O-RAN CU-CP E2 configuration values.
void fill_o_cu_cp_e2_config_in_yaml_schema(YAML::Node& node, const o_cu_cp_e2_config& config);

} // namespace ocudu
