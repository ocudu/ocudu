// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "CLI/CLI11.hpp"

namespace ocudu {

struct expert_execution_appconfig;

/// Configures the given CLI11 application with the worker manager application configuration schema.
void configure_cli11_with_worker_manager_appconfig_schema(CLI::App& app, expert_execution_appconfig& config);

} // namespace ocudu
