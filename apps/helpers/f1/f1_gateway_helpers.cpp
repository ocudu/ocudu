// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "f1_gateway_helpers.h"
#include "apps/helpers/network/dtls_config_translators.h"
#include "apps/helpers/network/sctp_config_translators.h"
#include "ocudu/gateways/sctp_network_gateway.h"
#include "ocudu/gateways/udp_network_gateway.h"

namespace ocudu {

/// Creates an SCTP network gateway configuration from the application SCTP config, filling the socket parameters
/// from \c app_cfg.
static sctp_network_gateway_config make_sctp_network_gateway_config(const std::string&              if_name,
                                                                    const std::vector<std::string>& bind_addresses,
                                                                    uint16_t                        bind_port,
                                                                    uint16_t                        ppid,
                                                                    const sctp_appconfig&           app_cfg,
                                                                    const dtls_appconfig&           dtls_cfg)
{
  auto sctp_cfg = sctp_network_gateway_config{.if_name           = if_name,
                                              .bind_addresses    = bind_addresses,
                                              .ppid              = ppid,
                                              .rto_initial       = std::nullopt,
                                              .rto_min           = std::nullopt,
                                              .rto_max           = std::nullopt,
                                              .init_max_attempts = std::nullopt,
                                              .max_init_timeo    = std::nullopt,
                                              .hb_interval       = std::nullopt,
                                              .assoc_max_rxt     = std::nullopt,
                                              .nodelay           = std::nullopt};
  // bind_port lives in the common_network_gateway_config base class, so it cannot be set via designated
  // initializers and must be assigned separately.
  sctp_cfg.bind_port = bind_port;

  fill_sctp_network_gateway_config_socket_params(sctp_cfg, app_cfg);
  if (dtls_cfg.enabled) {
    sctp_cfg.dtls_cfg.emplace();
    fill_dtls_network_gateway_config_params(*sctp_cfg.dtls_cfg, dtls_cfg);
  }
  return sctp_cfg;
}

/// Creates a UDP network gateway configuration for a GTP-U gateway from the F1-U socket application config.
static udp_network_gateway_config make_udp_gtpu_gateway_config(const f1u_socket_appconfig&  sock_cfg,
                                                               const f1u_sockets_appconfig& sockets_cfg,
                                                               const std::string&           if_name,
                                                               bool                         warn_on_drop)
{
  udp_network_gateway_config gw_cfg{.if_name                  = if_name,
                                    .bind_address             = sock_cfg.bind_addr,
                                    .rx_max_mmsg              = sock_cfg.udp_config.rx_max_msgs,
                                    .tx_qsize                 = 4096,
                                    .tx_max_mmsg              = 256,
                                    .tx_max_segments          = 256,
                                    .pool_occupancy_threshold = sock_cfg.udp_config.pool_threshold,
                                    .dscp                     = sock_cfg.udp_config.dscp,
                                    .ext_bind_addr            = sock_cfg.udp_config.ext_addr,
                                    .warn_on_drop             = warn_on_drop};
  gw_cfg.bind_port  = sockets_cfg.bind_port;
  gw_cfg.reuse_addr = sock_cfg.udp_config.reuse_addr;
  return gw_cfg;
}

std::unique_ptr<ocucp::f1c_connection_server> create_f1c_gateway_server(const f1c_gateway_config&       cfg,
                                                                        const f1c_gateway_dependencies& dependencies)
{
  return ocudu::create_f1c_gateway_server(
      f1c_cu_sctp_gateway_config{make_sctp_network_gateway_config(
                                     cfg.if_name, cfg.bind_addrs, cfg.bind_port, cfg.ppid, cfg.sctp_cfg, cfg.dtls_cfg),
                                 dependencies.broker,
                                 dependencies.io_rx_executor,
                                 dependencies.ctrl_exec,
                                 dependencies.pcap});
}

std::unique_ptr<gtpu_gateway> create_f1u_gtpu_gateway(const f1u_gateway_config&       cfg,
                                                      const f1u_gateway_dependencies& dependencies)
{
  const auto gw_cfg = make_udp_gtpu_gateway_config(cfg.sock_cfg, cfg.sockets_cfg, cfg.if_name, cfg.warn_on_drop);
  return create_udp_gtpu_gateway(gw_cfg, dependencies.broker, dependencies.io_tx_executor, dependencies.io_rx_executor);
}

std::unique_ptr<ocudu::f1u_cu_up_udp_gateway>
create_f1u_cu_up_split_gateway(const f1u_cu_up_split_gateway_config& cfg,
                               const f1u_cu_up_split_gateway_dependencies& /*dependencies*/)
{
  return ocuup::create_split_f1u_gw(
      ocuup::f1u_cu_up_split_gateway_creation_msg{cfg.gw_maps, cfg.demux, cfg.pcap, cfg.peer_port});
}

std::unique_ptr<ocudu::odu::f1u_du_udp_gateway>
create_f1u_du_split_gateway(const f1u_du_split_gateway_config& cfg,
                            const f1u_du_split_gateway_dependencies& /*dependencies*/)
{
  return odu::create_split_f1u_gw(
      odu::f1u_du_split_gateway_creation_msg{cfg.gw_maps, &cfg.demux, cfg.pcap, cfg.peer_port});
}

} // namespace ocudu
