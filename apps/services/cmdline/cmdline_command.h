// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/span.h"
#include <string_view>

namespace ocudu {
namespace app_services {

/// \brief Command-line command interface.
///
/// This interface allows to define commands for the command line that change the behavior of the application.
class cmdline_command
{
public:
  /// Default destructor.
  virtual ~cmdline_command() = default;

  /// Returns the name of this command.
  virtual std::string_view get_name() const = 0;

  /// Returns the description of this command.
  virtual std::string_view get_description() const = 0;

  /// Execute this command with the given arguments.
  virtual void execute(span<const std::string> args) = 0;
};

} // namespace app_services
} // namespace ocudu
