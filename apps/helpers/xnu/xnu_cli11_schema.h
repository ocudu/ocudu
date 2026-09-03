// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "CLI/CLI11.hpp"

namespace ocudu {

struct xnu_sockets_appconfig;

/// Configures the given CLI11 application with the Xn-U sockets application configuration schema.
void configure_cli11_xnu_sockets_args(CLI::App& app, xnu_sockets_appconfig& xnu_params);

} // namespace ocudu
