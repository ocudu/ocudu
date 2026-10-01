// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "CLI/CLI11.hpp"

namespace ocudu {

struct split6_o_du_unit_config;

/// Configures the given CLI11 application with the O-RAN DU Split 6 unit configuration schema.
void configure_cli11_with_split6_o_du_unit_config_schema(CLI::App& app, split6_o_du_unit_config& config);

/// Auto derive O-RAN DU Split 6 parameters after the parsing.
void autoderive_split6_o_du_parameters_after_parsing(CLI::App& app, split6_o_du_unit_config& config);

} // namespace ocudu
