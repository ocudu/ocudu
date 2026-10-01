// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include <cstdint>
#include <optional>

namespace ocudu {
namespace ocucp {

class cu_cp_e1_handler;

/// Connection server responsible for handling new connection requests/drops coming from CU-UPs via the E1 interface
/// and converting them into CU-CP commands.
class e1_connection_server
{
public:
  virtual ~e1_connection_server() = default;

  /// Attach a CU-CP handler to the E1 connection server.
  virtual void attach_cu_cp(cu_cp_e1_handler& cu_e1_handler_) = 0;

  /// Stop the E1 connection server.
  virtual void stop() = 0;

  /// \brief Get port on which the E1 Server is listening for new connections.
  /// \return The port number on which the E1 Server is listening for new connections.
  virtual std::optional<uint16_t> get_listen_port() const = 0;
};

} // namespace ocucp
} // namespace ocudu
