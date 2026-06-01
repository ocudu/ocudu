// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/e1ap/gateways/e1_connection_server.h"
#include "ocudu/gateways/sctp_network_gateway.h"

namespace ocudu {

class dlt_pcap;
class io_broker;
class task_executor;

/// Configuration of an SCTP-based E1 Gateway in the CU-CP.
struct e1_cu_cp_sctp_gateway_config {
  /// SCTP configuration.
  sctp_network_gateway_config sctp;
  /// IO broker responsible for handling SCTP Rx data and notifications.
  io_broker& broker;
  /// Execution context used to process received SCTP packets.
  task_executor& io_rx_executor;
  /// CU-CP control executor to process SCTP notifications deffered back from io_broker executor.
  task_executor& ctrl_exec;
  /// PCAP writer for the E1AP messages.
  dlt_pcap& pcap;
};

/// Creates an E1 Gateway server that listens for incoming SCTP connections, packs/unpacks E1AP PDUs and forwards
/// them to the GW/CU-CP E1AP handler.
std::unique_ptr<ocucp::e1_connection_server> create_e1_gateway_server(const e1_cu_cp_sctp_gateway_config& cfg);

} // namespace ocudu
