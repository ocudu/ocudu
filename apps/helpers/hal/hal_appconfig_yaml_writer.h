// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include <yaml-cpp/yaml.h>

namespace ocudu {

struct hal_appconfig;

/// Fills the HAL configuration in the given YAML node.
void fill_hal_appconfig_section(YAML::Node node, const hal_appconfig& config);

} // namespace ocudu
