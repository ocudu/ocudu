// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "CLI/CLI11.hpp"

namespace ocudu {
namespace app_services {

struct metrics_appconfig;

/// Configures the given CLI11 application with the metrics logger application configuration schema.
void configure_cli11_with_metrics_appconfig_schema(CLI::App& app, metrics_appconfig& config);

} // namespace app_services
} // namespace ocudu
