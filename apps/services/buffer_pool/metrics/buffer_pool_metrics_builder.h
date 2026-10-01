// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "apps/services/metrics/metrics_channel.h"

namespace ocudu {

namespace app_helpers {
struct metrics_config;
} // namespace app_helpers

namespace app_services {

class remote_server_metrics_gateway;

///\brief Builds the buffer pool metrics channel.
///
/// \param metrics_channels[out] - Application metrics channels, extended with the buffer pool metrics channel.
/// \param metrics_cfg[in]       - Common metrics configuration.
/// \param metrics_gateway[in]   - Metrics JSON gateway.
void build_buffer_pool_metrics_config(metrics_channel_collection&        metrics_channels,
                                      const app_helpers::metrics_config& metrics_cfg,
                                      remote_server_metrics_gateway*     metrics_gateway);
} // namespace app_services
} // namespace ocudu
