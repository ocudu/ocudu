// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "apps/helpers/e2/e2_appconfig.h"
#include "apps/helpers/network/sctp_config_translators.h"
#include "ocudu/e2/gateways/e2_network_client_factory.h"

namespace ocudu {

/// Retuns the E2 SCTP gateway configuration from the given parameters.
inline e2_sctp_gateway_config generate_e2_client_gateway_config(const e2_config& e2_cfg,
                                                                io_broker&       broker,
                                                                task_executor&   io_rx_executor,
                                                                dlt_pcap&        f1ap_pcap,
                                                                uint16_t         ppid)
{
  sctp_network_connector_config e2ap_sctp{};
  e2ap_sctp.if_name           = "E2";
  e2ap_sctp.dest_name         = "Near-RT-RIC";
  e2ap_sctp.connect_addresses = e2_cfg.ip_addrs;
  e2ap_sctp.connect_port      = e2_cfg.port;
  e2ap_sctp.ppid              = ppid;
  e2ap_sctp.bind_addresses    = e2_cfg.bind_addrs;
  fill_sctp_network_gateway_config_socket_params(e2ap_sctp, e2_cfg.sctp);

  const std::string logger_id = (ppid == E2_DU_PPID)   ? "E2-DU"
                                : (ppid == E2_UP_PPID) ? "E2-CU-UP"
                                : (ppid == E2_CP_PPID) ? "E2-CU-CP"
                                                       : "E2";
  return {e2ap_sctp, broker, io_rx_executor, f1ap_pcap, ocudulog::fetch_basic_logger(logger_id)};
}

} // namespace ocudu
