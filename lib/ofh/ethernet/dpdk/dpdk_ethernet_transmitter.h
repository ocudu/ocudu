// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "../ethernet_tx_metrics_collector_impl.h"
#include "ocudu/ocudulog/logger.h"
#include "ocudu/ofh/ethernet/dpdk/dpdk_ethernet_port_context.h"
#include "ocudu/ofh/ethernet/ethernet_transmitter.h"

namespace ocudu {
namespace ether {

struct transmitter_config;

/// DPDK Ethernet transmitter implementation.
class dpdk_transmitter_impl : public transmitter
{
public:
  dpdk_transmitter_impl(std::shared_ptr<dpdk_port_context> port_ctx_,
                        ocudulog::basic_logger&            logger_,
                        bool                               are_metrics_enabled) :
    logger(logger_), port_ctx(std::move(port_ctx_)), metrics_collector(are_metrics_enabled)
  {
    ocudu_assert(port_ctx, "Invalid port context");
  }

  // See interface for documentation.
  void send(span<span<const uint8_t>> frames) override;

  // See interface for documentation.
  transmitter_metrics_collector* get_metrics_collector() override;

private:
  ocudulog::basic_logger&            logger;
  std::shared_ptr<dpdk_port_context> port_ctx;
  transmitter_metrics_collector_impl metrics_collector;
};

} // namespace ether
} // namespace ocudu
