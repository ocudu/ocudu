// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "ocudu/gateways/sctp_network_client_factory.h"
#include "sctp_network_client_impl.h"

using namespace ocudu;

std::unique_ptr<sctp_network_client> ocudu::create_sctp_network_client(const sctp_network_client_config& config)
{
  return sctp_network_client_impl::create(config.sctp, config.broker, config.io_rx_executor);
}
