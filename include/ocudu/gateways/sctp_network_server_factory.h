// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/gateways/sctp_network_gateway.h"
#include "ocudu/gateways/sctp_network_server.h"

namespace ocudu {

class task_executor;

/// Configuration of an SCTP server.
struct sctp_network_server_config {
  sctp_network_gateway_config       sctp;
  io_broker&                        broker;
  task_executor&                    io_rx_executor;
  task_executor&                    app_exec;
  sctp_network_association_factory& association_handler_factory;
};

/// Creates and starts an SCTP network node that can operate both as a server and client and create new SCTP
/// associations.
std::unique_ptr<sctp_network_server> create_sctp_network_server(const sctp_network_server_config& config);

} // namespace ocudu
