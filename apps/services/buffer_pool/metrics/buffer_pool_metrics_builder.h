// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "apps/services/metrics/metrics_config.h"

namespace ocudu {

namespace app_helpers {
struct metrics_config;
} // namespace app_helpers

namespace app_services {

class buffer_pool_manager;
class metrics_notifier;
class remote_server_metrics_gateway;

///\brief Builds the buffer pool metrics configuration.
///
/// \param notifier[in]        - Application metrics notifier interface.
/// \param metrics_cfg[in]     - Common metrics configuration.
/// \param metrics_gateway[in] - Metrics JSON gateway.
///
/// \return buffer pool metrics configuration.
metrics_config build_buffer_pool_metrics_config(metrics_notifier&                  notifier,
                                                const app_helpers::metrics_config& metrics_cfg,
                                                remote_server_metrics_gateway*     metrics_gateway);
} // namespace app_services
} // namespace ocudu
