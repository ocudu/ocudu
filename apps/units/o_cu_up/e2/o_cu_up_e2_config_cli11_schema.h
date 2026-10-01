// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "CLI/CLI11.hpp"

namespace ocudu {

struct o_cu_up_e2_config;

/// Configures the given CLI11 application with the O-RAN CU-UP application unit E2 configuration schema.
void configure_cli11_with_o_cu_up_e2_config_schema(CLI::App& app, o_cu_up_e2_config& unit_cfg);

/// Auto derive O-RAN CU-UP E2 parameters after the parsing.
void autoderive_o_cu_up_e2_parameters_after_parsing(o_cu_up_e2_config& unit_cfg);

} // namespace ocudu
