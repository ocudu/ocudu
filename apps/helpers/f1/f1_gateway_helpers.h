// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "apps/helpers/f1u/f1u_appconfig.h"
#include "apps/helpers/network/dtls_appconfig.h"
#include "apps/helpers/network/sctp_appconfig.h"
#include "ocudu/f1ap/gateways/f1c_network_server_factory.h"
#include "ocudu/f1u/cu_up/split_connector/f1u_split_connector_factory.h"
#include "ocudu/f1u/du/split_connector/f1u_split_connector_factory.h"
#include "ocudu/gtpu/gtpu_demux.h"
#include "ocudu/gtpu/gtpu_gateway.h"
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace ocudu {

/// F1-C gateway configuration.
struct f1c_gateway_config {
  const std::vector<std::string>& bind_addrs;
  const sctp_appconfig&           sctp_cfg;
  const dtls_appconfig&           dtls_cfg;
  std::string                     if_name;
  uint16_t                        bind_port = F1AP_PORT;
  uint16_t                        ppid      = F1AP_PPID;
};

/// F1-C gateway dependencies.
struct f1c_gateway_dependencies {
  io_broker&     broker;
  task_executor& io_rx_executor;
  task_executor& ctrl_exec;
  dlt_pcap&      pcap;
};

/// Creates an F1-C gateway server that listens for incoming SCTP connections, using defaults for the F1AP port,
/// PPID and interface name when not specified in \c cfg.
std::unique_ptr<ocucp::f1c_connection_server> create_f1c_gateway_server(const f1c_gateway_config&       cfg,
                                                                        const f1c_gateway_dependencies& dependencies);

/// F1-U gateway configuration.
struct f1u_gateway_config {
  const f1u_socket_appconfig&  sock_cfg;
  const f1u_sockets_appconfig& sockets_cfg;
  std::string                  if_name;
  bool                         warn_on_drop;
};

/// F1-U gateway dependencies.
struct f1u_gateway_dependencies {
  io_broker&     broker;
  task_executor& io_tx_executor;
  task_executor& io_rx_executor;
};

/// Creates a UDP-based GTP-U gateway for the F1-U interface from the F1-U socket application config.
std::unique_ptr<gtpu_gateway> create_f1u_gtpu_gateway(const f1u_gateway_config&       cfg,
                                                      const f1u_gateway_dependencies& dependencies);

/// F1-U CU-UP split gateway configuration.
struct f1u_cu_up_split_gateway_config {
  gtpu_gateway_maps& gw_maps;
  gtpu_demux&        demux;
  dlt_pcap&          pcap;
  uint16_t           peer_port;
};

/// F1-U CU-UP split gateway dependencies.
/// \note The CU-UP split gateway factory currently takes all inputs via the creation message, so no extra
/// dependencies are required. The struct is declared for API symmetry with the other creation methods.
struct f1u_cu_up_split_gateway_dependencies {};

/// Creates a CU-UP F1-U split gateway from the GTP-U gateway maps, demux, PCAP writer and peer port.
std::unique_ptr<ocudu::f1u_cu_up_udp_gateway>
create_f1u_cu_up_split_gateway(const f1u_cu_up_split_gateway_config&       cfg,
                               const f1u_cu_up_split_gateway_dependencies& dependencies);

/// F1-U DU split gateway configuration.
struct f1u_du_split_gateway_config {
  gtpu_gateway_maps& gw_maps;
  gtpu_demux&        demux;
  dlt_pcap&          pcap;
  uint16_t           peer_port;
};

/// F1-U DU split gateway dependencies.
/// \note The DU split gateway factory currently takes all inputs via the creation message, so no extra
/// dependencies are required. The struct is declared for API symmetry with the other creation methods.
struct f1u_du_split_gateway_dependencies {};

/// Creates a DU F1-U split gateway from the GTP-U gateway maps, demux, PCAP writer and peer port.
std::unique_ptr<ocudu::odu::f1u_du_udp_gateway>
create_f1u_du_split_gateway(const f1u_du_split_gateway_config&       cfg,
                            const f1u_du_split_gateway_dependencies& dependencies);

} // namespace ocudu
