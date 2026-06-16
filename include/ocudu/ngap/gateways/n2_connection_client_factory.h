// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/gateways/sctp_network_gateway.h"
#include "ocudu/ngap/gateways/n2_connection_client.h"
#include "ocudu/support/executors/task_executor.h"
#include <variant>

namespace ocudu {

class io_broker;
class dlt_pcap;

namespace ocucp {

/// Parameters for the NG gateway instantiation.
struct n2_connection_client_config {
  /// Parameters for a local AMF stub connection.
  struct no_core {};

  /// Parameters specific to an SCTP network gateway.
  struct network {
    io_broker&                    broker;
    task_executor&                io_rx_executor;
    sctp_network_connector_config sctp;
  };

  /// PCAP writer for the NGAP messages.
  dlt_pcap& pcap;

  /// Mode of operation.
  std::variant<no_core, network> mode;
};

/// Create an N2 connection client.
std::unique_ptr<n2_connection_client> create_n2_connection_client(const n2_connection_client_config& params);

} // namespace ocucp
} // namespace ocudu
