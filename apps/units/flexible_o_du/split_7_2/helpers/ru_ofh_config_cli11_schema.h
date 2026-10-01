// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "CLI/CLI11.hpp"

namespace ocudu {

struct ru_ofh_unit_config;
struct ru_ofh_unit_parsed_config;

/// Configures the given CLI11 application with the Open Fronthaul Radio Unit configuration schema.
void configure_cli11_with_ru_ofh_config_schema(CLI::App& app, ru_ofh_unit_parsed_config& parsed_cfg);

} // namespace ocudu
