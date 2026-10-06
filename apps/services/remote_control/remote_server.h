// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "apps/services/remote_control/remote_command.h"
#include "apps/services/remote_control/remote_server_metrics_gateway.h"
#include "ocudu/adt/span.h"
#include <memory>

namespace ocudu {

struct remote_control_appconfig;

namespace app_services {

/// Remote server operation controller.
class remote_server_operation_controller
{
public:
  virtual ~remote_server_operation_controller() = default;

  /// Starts listening new requests.
  virtual void start() = 0;

  /// Stops listening new requests.
  virtual void stop() = 0;

  /// \brief Rejects any new incoming command while metrics keep being sent.
  ///
  /// Returns once no command is being executed.
  virtual void reject_commands() = 0;
};

/// \brief Remote server interface.
///
/// WebSocket server that listens for external incoming requests.
class remote_server
{
public:
  /// Default destructor.
  virtual ~remote_server() = default;

  /// \brief Add the given commands to the remote server.
  ///
  /// The commands can only be added before the remote server has started to operate.
  virtual void add_commands(span<std::unique_ptr<remote_command>> remote_cmds) = 0;

  /// Returns the operation controller of this remote server.
  virtual remote_server_operation_controller& get_operation_controller() = 0;

  /// Returns the metrics gateway of this remote server. Returns nullptr if the remote server is not enabled to work
  /// with metrics commands.
  virtual remote_server_metrics_gateway* get_metrics_gateway() = 0;
};

/// Creates a Remote Server instance with a give list of commands.
std::unique_ptr<remote_server> create_remote_server(const remote_control_appconfig& cfg);

} // namespace app_services
} // namespace ocudu
