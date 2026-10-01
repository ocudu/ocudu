// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "CLI/CLI11.hpp"

namespace ocudu {

struct e2_config;

/// \brief Configures the given CLI11 application with the E2 application configuration schema.
///
/// \param[out] app CLI11 application to configure.
/// \param[out] config E2 configuration that stores the parameters.
/// \param[in] option_name Option name for the E2 unit property.
/// \param[out] option_description Option description for the E2 unit property.
void configure_cli11_with_e2_config_schema(CLI::App&          app,
                                           e2_config&         config,
                                           const std::string& option_name,
                                           const std::string& option_description);

} // namespace ocudu
