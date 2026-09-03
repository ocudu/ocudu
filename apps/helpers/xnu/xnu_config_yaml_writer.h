// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include <yaml-cpp/yaml.h>

namespace ocudu {

struct xnu_sockets_appconfig;

/// Fills the Xn-U configuration in the given YAML node.
void fill_xnu_config_yaml_schema(YAML::Node& node, const xnu_sockets_appconfig& config);

} // namespace ocudu
