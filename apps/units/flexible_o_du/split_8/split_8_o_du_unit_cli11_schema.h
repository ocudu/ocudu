// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "CLI/CLI11.hpp"

namespace ocudu {

struct split_8_o_du_unit_config;

/// Configures the given CLI11 application with the split 8 O-RAN DU unit configuration schema.
void configure_cli11_with_split_8_o_du_unit_config_schema(CLI::App& app, split_8_o_du_unit_config& parsed_cfg);

/// Auto derive split 8 O-RAN DU parameters after the parsing.
void autoderive_split_8_o_du_parameters_after_parsing(CLI::App& app, split_8_o_du_unit_config& parsed_cfg);

} // namespace ocudu
