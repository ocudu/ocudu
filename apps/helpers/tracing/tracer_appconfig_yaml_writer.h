// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include <yaml-cpp/yaml.h>

namespace ocudu {

struct tracer_appconfig;

/// Fills the given node with the tracer application configuration values.
void fill_tracer_appconfig_in_yaml_schema(YAML::Node& node, const tracer_appconfig& config);

} // namespace ocudu
