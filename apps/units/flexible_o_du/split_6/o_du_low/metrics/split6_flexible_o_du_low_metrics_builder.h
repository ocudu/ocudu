// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "apps/helpers/metrics/metrics_config.h"
#include "apps/services/metrics/metrics_config.h"
#include "ocudu/ran/pci.h"

namespace ocudu {

class split6_flexible_o_du_low_metrics_notifier;

namespace app_helpers {
struct metrics_config;
} // namespace app_helpers

namespace app_services {
class metrics_notifier;
class remote_server_metrics_gateway;
} // namespace app_services

/// Builds the split 6flexible O-DU low metrics configuration.
split6_flexible_o_du_low_metrics_notifier*
build_split6_flexible_o_du_low_metrics_config(std::vector<app_services::metrics_config>&   metrics,
                                              app_services::metrics_notifier&              notifier,
                                              app_services::remote_server_metrics_gateway* remote_metrics_gateway,
                                              const app_helpers::metrics_config&           metrics_cfg,
                                              const std::vector<pci_t>&                    pci_cell_map,
                                              std::chrono::nanoseconds                     symbol_duration);

} // namespace ocudu
