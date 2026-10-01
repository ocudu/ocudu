// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "cmdline_command.h"
#include "ocudu/adt/span.h"
#include "ocudu/ocudulog/logger.h"
#include "ocudu/support/io/io_broker.h"
#include <string>
#include <unordered_map>

namespace ocudu {
namespace app_services {

/// \brief Command-line command dispatcher.
///
/// The command-line command dispatcher parses commands from the STDIN and executes them if they were registered upon
/// creation, otherwise prints a list of the registered commands with their description.
class cmdline_command_dispatcher
{
public:
  cmdline_command_dispatcher(io_broker&                             io_broker,
                             task_executor&                         executor,
                             span<std::unique_ptr<cmdline_command>> commands_);

private:
  /// Parses any contents in the STDIN file descriptor.
  void parse_stdin();

  /// Handles the given command.
  void handle_command(const std::string& command);

  /// Prints the registered events registered in this console.
  void print_help();

private:
  ocudulog::basic_logger&                                           logger;
  io_broker::subscriber                                             stdin_handle;
  std::unordered_map<std::string, std::unique_ptr<cmdline_command>> commands;
};

} // namespace app_services
} // namespace ocudu
