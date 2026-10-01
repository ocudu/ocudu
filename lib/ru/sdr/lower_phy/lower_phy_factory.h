// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "lower_phy_sector.h"
#include "ocudu/phy/lower/lower_phy_factory.h"
#include <memory>

namespace ocudu {

struct lower_phy_configuration;

/// Lower phy sector dependencies.
struct lower_phy_sector_dependencies {
  ocudulog::basic_logger&       logger;
  task_executor&                rx_task_executor;
  task_executor&                tx_task_executor;
  task_executor&                dl_task_executor;
  task_executor&                ul_task_executor;
  task_executor&                prach_async_executor;
  lower_phy_error_notifier&     error_notifier;
  lower_phy_timing_notifier*    timing_notifier;
  baseband_gateway&             bb_gateway;
  lower_phy_rx_symbol_notifier& rx_symbol_notifier;
};

/// Helper class that creates the lower PHY sector using the given configuration.
std::unique_ptr<lower_phy_sector> create_lower_phy_sector(const lower_phy_configuration&       lower_phy_config,
                                                          const lower_phy_sector_dependencies& sector_deps);

} // namespace ocudu
