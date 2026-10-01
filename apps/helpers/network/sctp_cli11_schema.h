// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "CLI/CLI11.hpp"

namespace ocudu {

struct sctp_appconfig;

/// \brief Adds SCTP socket option CLI11 parameters to the given application.
///
/// Options are added flat (no subcommand), so they appear at the same level as the caller's other options.
///
/// \param[out] app CLI11 application to configure.
/// \param[out] config SCTP configuration that stores the parameters.
void configure_cli11_sctp_socket_args(CLI::App& app, sctp_appconfig& config);

} // namespace ocudu
