// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "nlohmann/json.hpp"
#include "ocudu/adt/expected.h"
#include <string>
#include <string_view>

namespace ocudu {
namespace app_services {

/// \brief Remote command interface.
///
/// This interface allows to define commands executed by the remote control server.
class remote_command
{
public:
  /// Default destructor.
  virtual ~remote_command() = default;

  /// Returns the name of this command.
  virtual std::string_view get_name() const = 0;

  /// Returns the description of this command.
  virtual std::string_view get_description() const = 0;

  /// \brief Execute this command using the parameters encoded in the given JSON.
  ///
  /// Returns the response payload on success — a null value when the command has none — or an error
  /// description. The server nests a non-null payload under the "result" key of the success response.
  virtual expected<nlohmann::json, std::string> execute(const nlohmann::json& json) = 0;
};

} // namespace app_services
} // namespace ocudu
