// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "apps/helpers/f1u/f1u_appconfig.h"
#include "apps/helpers/hal/hal_appconfig.h"
#include "apps/helpers/logger/logger_appconfig.h"
#include "apps/helpers/network/dtls_appconfig.h"
#include "apps/helpers/network/sctp_appconfig.h"
#include "apps/helpers/tracing/tracer_appconfig.h"
#include "apps/services/app_execution_metrics/executor_metrics_config.h"
#include "apps/services/app_resource_usage/app_resource_usage_config.h"
#include "apps/services/buffer_pool/buffer_pool_appconfig.h"
#include "apps/services/metrics/metrics_appconfig.h"
#include "apps/services/remote_control/remote_control_appconfig.h"
#include "apps/services/worker_manager/worker_manager_appconfig.h"
#include <optional>

namespace ocudu {
namespace odu {

/// Configuration of the F1-C interface of the DU.
struct f1ap_appconfig {
  /// CU-CP F1-C addresses the DU will connect to.
  std::vector<std::string> cu_cp_addresses = {"127.0.10.1"};
  /// DU F1-C bind addresses.
  std::vector<std::string> bind_addresses = {"127.0.10.2"};
  /// \brief Whether a failed F1-C connection setup is retried indefinitely.
  ///
  /// When enabled, the DU keeps retrying the F1-C TNL connection on startup, so that it does not require the CU-CP to
  /// be reachable when it is started. When disabled, the DU closes the application if the connection fails.
  bool retry_connection = false;
  /// SCTP socket options.
  sctp_appconfig sctp;
  /// DTLS options.
  dtls_appconfig dtls;
};

struct f1u_appconfig {
  unsigned              pdu_queue_size = 2048;
  f1u_sockets_appconfig f1u_sockets;
};

/// Metrics report configuration.
struct metrics_appconfig {
  app_services::app_resource_usage_config rusage_config;
  app_services::metrics_appconfig         metrics_service_cfg;
  app_services::executor_metrics_config   executors_metrics_cfg;
  bool                                    autostart_stdout_metrics = false;
};

} // namespace odu

/// DU application configuration.
struct du_appconfig {
  /// Default constructor to update the log filename.
  du_appconfig() { log_cfg.filename = "/tmp/du.log"; }
  /// Loggers configuration.
  logger_appconfig log_cfg;
  /// Tracers configuration.
  tracer_appconfig trace_cfg;
  /// Metrics configuration.
  odu::metrics_appconfig metrics_cfg;
  /// F1-C configuration.
  odu::f1ap_appconfig f1ap_cfg;
  /// F1-U configuration.
  odu::f1u_appconfig f1u_cfg;
  /// Buffer pool configuration.
  app_services::buffer_pool_appconfig buffer_pool_config;
  /// Expert configuration.
  expert_execution_appconfig expert_execution_cfg;
  /// HAL configuration.
  std::optional<hal_appconfig> hal_config;
  /// Remote control configuration.
  remote_control_appconfig remote_control_config;
  /// Dryrun mode enabled flag.
  bool enable_dryrun = false;
};

} // namespace ocudu
