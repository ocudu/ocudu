// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include <yaml-cpp/yaml.h>

namespace ocudu {
namespace app_services {

struct buffer_pool_appconfig;

/// Fills the given node with the buffer pool configuration values.
void fill_buffer_pool_config_in_yaml_schema(YAML::Node& node, const buffer_pool_appconfig& config);

} // namespace app_services
} // namespace ocudu
