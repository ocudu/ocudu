// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/gateways/sctp_network_client.h"
#include "ocudu/gateways/sctp_network_gateway.h"
#include <memory>

namespace ocudu {

class task_executor;

/// Configuration of an SCTP client.
struct sctp_network_client_config {
  sctp_network_connector_config sctp;
  io_broker&                    broker;
  task_executor&                io_rx_executor;
};

/// Creates an SCTP network client.
std::unique_ptr<sctp_network_client> create_sctp_network_client(const sctp_network_client_config& config);

} // namespace ocudu
