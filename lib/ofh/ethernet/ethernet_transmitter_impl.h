// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ethernet_tx_metrics_collector_impl.h"
#include "ocudu/ocudulog/logger.h"
#include "ocudu/ofh/ethernet/ethernet_transmitter.h"
#include "ocudu/ofh/ethernet/ethernet_transmitter_config.h"
#include <linux/if_packet.h>

namespace ocudu {
namespace ether {

/// Implementation for the Ethernet transmitter.
class transmitter_impl : public transmitter
{
public:
  transmitter_impl(const transmitter_config& config, ocudulog::basic_logger& logger_);
  ~transmitter_impl() override;

  // See interface for documentation.
  void send(span<span<const uint8_t>> frames) override;

  // See interface for documentation.
  transmitter_metrics_collector* get_metrics_collector() override;

private:
  ocudulog::basic_logger&            logger;
  int                                socket_fd = -1;
  transmitter_metrics_collector_impl metrics_collector;
  ::sockaddr_ll                      socket_address;
};

} // namespace ether
} // namespace ocudu
