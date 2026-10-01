// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "CLI/CLI11.hpp"

namespace ocudu {
namespace app_services {

struct app_resource_usage_config;

/// Configures the given CLI11 application with the application resource usage configuration schema.
void configure_cli11_with_app_resource_usage_config_schema(CLI::App& app, app_resource_usage_config& config);

} // namespace app_services
} // namespace ocudu
