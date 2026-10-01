// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "apps/services/metrics/metrics_config.h"

namespace ocudu {

class executor_metrics_notifier;

namespace app_helpers {
struct metrics_config;
} // namespace app_helpers

namespace app_services {
class metrics_notifier;
class remote_server_metrics_gateway;
} // namespace app_services

///\brief Builds the application execution metrics configuration.
///
/// \param metrics[out]        - Application metrics configuration of the executor metrics service.
/// \param notifier[in]        - Application metrics notifier interface.
/// \param metrics_cfg[in]     - Common metrics configuration.
/// \param metrics_gateway[in] - Gateway to send the metrics in JSON format.
///
/// \return reference to the executor metrics notifier.
executor_metrics_notifier&
build_app_execution_metrics_config(app_services::metrics_config&                exec_metric_cfg,
                                   app_services::metrics_notifier&              notifier,
                                   const app_helpers::metrics_config&           metrics_cfg,
                                   app_services::remote_server_metrics_gateway* metrics_gateway);

} // namespace ocudu
