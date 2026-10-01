// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "apps/services/cmdline/stdout_metrics_command.h"
#include "apps/services/remote_control/remote_command.h"
#include <memory>
#include <vector>

namespace ocudu {

/// Application command-line commands.
struct cmdline_commands {
  std::vector<std::unique_ptr<app_services::cmdline_command>>                                       commands;
  std::vector<std::unique_ptr<app_services::toggle_stdout_metrics_app_command::metrics_subcommand>> metrics_subcommands;
};

/// \brief Application unit commands.
///
/// This container structure stores the commands (both command-line and remote) that the application unit provides.
struct application_unit_commands {
  cmdline_commands                                           cmdline;
  std::vector<std::unique_ptr<app_services::remote_command>> remote;
};

} // namespace ocudu
