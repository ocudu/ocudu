// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "CLI/CLI11.hpp"
#include <optional>

namespace ocudu {

struct dtls_appconfig;

/// \brief Adds DTLS option CLI11 parameters to the given application.
///
/// Options are added flat (no subcommand), so they appear at the same level as the caller's other options.
/// Some options like the DTLS mode (server or client) are link specific, thus ommited from this helper. Each link will
/// need to configure those options independently.
///
/// \param[out] app CLI11 application to configure.
/// \param[out] config DTLS configuration that stores the parameters.
void configure_cli11_dtls_args(CLI::App& app, dtls_appconfig& config);

} // namespace ocudu
