// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "CLI/CLI11.hpp"

namespace ocudu {

struct du_appconfig;

/// Configures the given CLI11 application with the DU application configuration schema.
void configure_cli11_with_du_appconfig_schema(CLI::App& app, du_appconfig& parsed_cfg);

/// Auto derive DU parameters after the parsing.
void autoderive_du_parameters_after_parsing(CLI::App& app, du_appconfig& du_cfg);

} // namespace ocudu
