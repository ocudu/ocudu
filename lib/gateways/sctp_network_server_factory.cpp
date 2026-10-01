// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "ocudu/gateways/sctp_network_server_factory.h"
#include "sctp_network_server_impl.h"

using namespace ocudu;

std::unique_ptr<sctp_network_server> ocudu::create_sctp_network_server(const sctp_network_server_config& config)
{
  return sctp_network_server_impl::create(
      config.sctp, config.broker, config.io_rx_executor, config.app_exec, config.association_handler_factory);
}
