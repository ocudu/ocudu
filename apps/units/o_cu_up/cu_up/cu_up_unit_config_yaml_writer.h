// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include <yaml-cpp/yaml.h>

namespace ocudu {

struct cu_up_unit_config;

/// Fills the given node with the CU-UP configuration values.
void fill_cu_up_config_in_yaml_schema(YAML::Node& node, const cu_up_unit_config& config);

} // namespace ocudu
