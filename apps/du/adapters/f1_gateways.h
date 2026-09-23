// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "apps/helpers/network/dtls_appconfig.h"
#include "apps/helpers/network/dtls_config_translators.h"
#include "apps/helpers/network/sctp_config_translators.h"
#include "ocudu/f1ap/gateways/f1c_network_client_factory.h"

namespace ocudu {

/// Instantiates an F1-C DU client.
inline std::unique_ptr<odu::f1c_connection_client>
create_f1c_client_gateway(const std::vector<std::string>& cu_cp_addresses,
                          const std::vector<std::string>& bind_addresses,
                          const sctp_appconfig&           sctp_cfg,
                          const dtls_appconfig&           dtls_cfg,
                          bool                            retry_connection,
                          io_broker&                      broker,
                          task_executor&                  io_rx_executor,
                          dlt_pcap&                       f1ap_pcap)
{
  sctp_network_connector_config f1c_sctp{};
  f1c_sctp.if_name               = "F1-C";
  f1c_sctp.dest_name             = "CU-CP";
  f1c_sctp.connect_addresses     = cu_cp_addresses;
  f1c_sctp.connect_port          = F1AP_PORT;
  f1c_sctp.ppid                  = F1AP_PPID;
  f1c_sctp.bind_addresses        = bind_addresses;
  f1c_sctp.connection_is_retried = retry_connection;
  fill_sctp_network_gateway_config_socket_params(f1c_sctp, sctp_cfg);
  if (dtls_cfg.enabled) {
    f1c_sctp.dtls_cfg.emplace();
    fill_dtls_network_gateway_config_params(*f1c_sctp.dtls_cfg, dtls_cfg);
  }

  return create_f1c_gateway_client(f1c_du_sctp_gateway_config{f1c_sctp, broker, io_rx_executor, f1ap_pcap});
}

} // namespace ocudu
