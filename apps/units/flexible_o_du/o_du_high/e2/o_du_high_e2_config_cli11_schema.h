// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "CLI/CLI11.hpp"

namespace ocudu {

struct o_du_high_e2_config;

/// Configures the given CLI11 application with the O-RAN DU high E2 configuration schema.
void configure_cli11_with_o_du_high_e2_config_schema(CLI::App& app, o_du_high_e2_config& config);

/// Auto derive O-RAN DU high E2 parameters after the parsing.
void autoderive_o_du_high_e2_parameters_after_parsing(o_du_high_e2_config& unit_cfg);

} // namespace ocudu
