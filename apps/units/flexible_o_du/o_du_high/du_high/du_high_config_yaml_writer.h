// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include <yaml-cpp/yaml.h>

namespace ocudu {

struct du_high_parsed_config;

/// Fills the given node with the DU high configuration values.
void fill_du_high_config_in_yaml_schema(YAML::Node& node, const du_high_parsed_config& parsed_cfg);

} // namespace ocudu
