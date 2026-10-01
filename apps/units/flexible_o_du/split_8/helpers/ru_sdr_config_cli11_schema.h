// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "CLI/CLI11.hpp"

namespace ocudu {

struct ru_sdr_unit_config;

/// Configures the given CLI11 application with the SDR RU configuration schema.
void configure_cli11_with_ru_sdr_config_schema(CLI::App& app, ru_sdr_unit_config& parsed_cfg);

/// Auto derive SDR Radio Unit parameters after the parsing.
void autoderive_ru_sdr_parameters_after_parsing(CLI::App& app, ru_sdr_unit_config& parsed_cfg, unsigned nof_cells);

} // namespace ocudu
