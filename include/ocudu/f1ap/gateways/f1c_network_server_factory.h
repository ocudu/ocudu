// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/f1ap/gateways/f1c_connection_server.h"
#include "ocudu/gateways/sctp_network_gateway.h"

namespace ocudu {

class dlt_pcap;
class io_broker;
class task_executor;

/// Configuration of an SCTP-based F1-C Gateway.
struct f1c_cu_sctp_gateway_config {
  /// SCTP configuration.
  sctp_network_gateway_config sctp;
  /// IO broker responsible for handling SCTP Rx data and notifications.
  io_broker& broker;
  /// Execution context used to process received SCTP packets.
  task_executor& io_rx_executor;
  /// CU-CP control executor to process SCTP notifications deffered back from io_broker executor.
  task_executor& ctrl_exec;
  /// PCAP writer for the F1AP messages.
  dlt_pcap& pcap;
};

/// Creates an F1-C Gateway server that listens for incoming SCTP connections, packs/unpacks F1AP PDUs and forwards
/// them to the GW/CU-CP F1AP handler.
std::unique_ptr<ocucp::f1c_connection_server> create_f1c_gateway_server(const f1c_cu_sctp_gateway_config& params);

} // namespace ocudu
