// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "CLI/CLI11.hpp"

namespace ocudu {

struct cu_cp_appconfig;

/// Configures the given CLI11 application with the CU application configuration schema.
void configure_cli11_with_cu_cp_appconfig_schema(CLI::App& app, cu_cp_appconfig& cu_cfg);

} // namespace ocudu
