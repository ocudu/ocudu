// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include <cstdint>
#include <optional>

namespace ocudu {
namespace ocucp {

class cu_cp_f1c_handler;

/// Connection server responsible for handling new connection requests/drops coming from DUs via the F1-C interface
/// and converting them CU-CP commands.
class f1c_connection_server
{
public:
  virtual ~f1c_connection_server() = default;

  /// Attach a CU-CP handler to the F1-C connection server.
  virtual void attach_cu_cp(cu_cp_f1c_handler& cu_f1c_handler_) = 0;

  /// Stop the F1-C connection server.
  virtual void stop() = 0;

  /// Get port on which the F1-C Server is listening for new connections.
  ///
  /// This method is useful in testing, where we don't want to use a specific port.
  /// \return The port number on which the F1-C Server is listening for new connections.
  virtual std::optional<uint16_t> get_listen_port() const = 0;
};

} // namespace ocucp
} // namespace ocudu
