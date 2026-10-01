// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "ocudu/gateways/udp_network_gateway_factory.h"
#include "udp_network_gateway_impl.h"

using namespace ocudu;

std::unique_ptr<udp_network_gateway> ocudu::create_udp_network_gateway(const udp_network_gateway_creation_message& msg)
{
  return std::make_unique<udp_network_gateway_impl>(
      msg.config, msg.data_notifier, msg.io_tx_executor, msg.io_rx_executor);
}
