// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include <yaml-cpp/yaml.h>

namespace ocudu {

struct fapi_unit_config;

/// Fills the given node with the FAPI configuration values.
void fill_fapi_config_in_yaml_schema(YAML::Node& node, const fapi_unit_config& config);

} // namespace ocudu
