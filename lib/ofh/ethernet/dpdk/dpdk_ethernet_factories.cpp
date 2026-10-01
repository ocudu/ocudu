// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "ocudu/ofh/ethernet/dpdk/dpdk_ethernet_factories.h"
#include "dpdk_ethernet_receiver.h"
#include "dpdk_ethernet_transmitter.h"
#include "ocudu/ofh/ethernet/dpdk/dpdk_ethernet_port_context.h"
#include "ocudu/ofh/ethernet/ethernet_transmitter_config.h"

using namespace ocudu;
using namespace ether;

std::shared_ptr<dpdk_port_context> ocudu::ether::create_dpdk_port_context(const transmitter_config& config)
{
  dpdk_port_config port_cfg;
  port_cfg.id                           = config.interface;
  port_cfg.is_promiscuous_mode_enabled  = config.is_promiscuous_mode_enabled;
  port_cfg.is_link_status_check_enabled = config.is_link_status_check_enabled;
  port_cfg.mtu_size                     = config.mtu_size;

  return dpdk_port_context::create(port_cfg);
}

std::pair<std::unique_ptr<transmitter>, std::unique_ptr<receiver>>
ocudu::ether::create_dpdk_txrx(const transmitter_config& config,
                               task_executor&            rx_executor,
                               ocudulog::basic_logger&   logger)
{
  auto ctx = create_dpdk_port_context(config);

  return {std::make_unique<dpdk_transmitter_impl>(ctx, logger, config.are_metrics_enabled),
          std::make_unique<dpdk_receiver_impl>(rx_executor, ctx, logger, config.are_metrics_enabled)};
}
