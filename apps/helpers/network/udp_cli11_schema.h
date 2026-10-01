// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "CLI/CLI11.hpp"

namespace ocudu {

struct udp_appconfig;

/// \brief Configures the given CLI11 application with the UDP application configuration schema.
///
/// \param[out] app CLI11 application to configure.
/// \param[out] config UDP configuration that stores the parameters.
void configure_cli11_with_udp_config_schema(CLI::App& app, udp_appconfig& config);

} // namespace ocudu
