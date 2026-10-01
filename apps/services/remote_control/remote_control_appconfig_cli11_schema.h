// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "CLI/CLI11.hpp"

namespace ocudu {

struct remote_control_appconfig;

/// Configures the given CLI11 application with the remote control application configuration schema.
void configure_cli11_with_remote_control_appconfig_schema(CLI::App& app, remote_control_appconfig& config);

} // namespace ocudu
