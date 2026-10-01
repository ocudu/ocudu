// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include <yaml-cpp/yaml.h>

namespace ocudu {
namespace app_services {

struct executor_metrics_config;

/// Fills the given node with the application resource usage configuration values.
void fill_app_exec_metrics_config_in_yaml_schema(YAML::Node& node, const executor_metrics_config& config);

} // namespace app_services
} // namespace ocudu
