// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/e1ap/gateways/e1_connection_client.h"
#include "ocudu/e1ap/gateways/e1_connection_server.h"
#include <memory>

namespace ocudu {

class dlt_pcap;
class io_broker;
class task_executor;

class e1_local_connector : public ocuup::e1_connection_client, public ocucp::e1_connection_server
{};

struct e1_local_connector_config {
  /// PCAP writer for the E1AP messages.
  dlt_pcap& pcap;
};

/// Creates a local connector between CU-CP and CU-UP E1 interfaces, avoiding the need to pack/unpack the exchanged
/// E1AP PDUs or any socket send/recv.
std::unique_ptr<e1_local_connector> create_e1_local_connector(const e1_local_connector_config& cfg);

struct e1_local_sctp_connector_config {
  /// PCAP writer for the E1AP messages.
  dlt_pcap& pcap;
  /// IO broker to handle the SCTP Rx data and notifications.
  io_broker& broker;
  /// Execution context used to process received SCTP packets.
  task_executor& io_rx_executor;
  /// CU-CP control executor to process SCTP notifications deffered back from io_broker executor.
  task_executor& ctrl_exec;
  /// Port to bind the SCTP socket.
  int bind_port = 0;
};

/// Creates an E1 local connector using an SCTP socket as channel.
///
/// Note: This class is useful for testing.
std::unique_ptr<e1_local_connector> create_e1_local_connector(const e1_local_sctp_connector_config& cfg);

} // namespace ocudu
