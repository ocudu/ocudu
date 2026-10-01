// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/ocudulog/logger.h"
#include "ocudu/ofh/ethernet/dpdk/dpdk_ethernet_port_context.h"
#include "ocudu/ofh/ethernet/ethernet_receiver.h"
#include "ocudu/ofh/ethernet/ethernet_transmitter.h"
#include <memory>

namespace ocudu {

class task_executor;

namespace ether {

class frame_notifier;
struct transmitter_config;

/// Creates a DPDK port context object.
std::shared_ptr<dpdk_port_context> create_dpdk_port_context(const transmitter_config& config);

/// Creates a DPDK Ethernet transmitter and receiver pair.
std::pair<std::unique_ptr<transmitter>, std::unique_ptr<receiver>>
create_dpdk_txrx(const transmitter_config& config, task_executor& rx_executor, ocudulog::basic_logger& logger);

} // namespace ether
} // namespace ocudu
