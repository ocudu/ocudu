// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "apps/services/remote_control/remote_command.h"
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>

namespace ocudu {
namespace app_services {

/// Builds an error response encoded in JSON format.
std::string build_remote_error_response(std::string_view error, std::optional<std::string> cmd = std::nullopt);

/// \brief Dispatches the JSON requests received by the remote control server to the registered commands.
///
/// A request is a JSON object with a mandatory "cmd" string naming the command, and an optional "version" string
/// ("MAJOR" or "MAJOR.MINOR") with the remote control interface version the client expects. A request whose version
/// is not supported is rejected. The dispatcher always provides the "version" command, which reports the application
/// and interface versions, the "help" command, which lists the available commands, and the "quit" command.
class remote_command_dispatcher
{
public:
  remote_command_dispatcher();

  /// Registers the given command, replacing any command with the same name.
  void add_command(std::unique_ptr<remote_command> cmd);

  /// Handles the given JSON request and returns the JSON response.
  std::string handle_request(std::string_view request);

private:
  std::unordered_map<std::string, std::unique_ptr<remote_command>> commands;
};

} // namespace app_services
} // namespace ocudu
