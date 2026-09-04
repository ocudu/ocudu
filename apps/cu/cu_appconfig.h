// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "apps/helpers/f1u/f1u_appconfig.h"
#include "apps/helpers/logger/logger_appconfig.h"
#include "apps/helpers/network/sctp_appconfig.h"
#include "apps/helpers/tracing/tracer_appconfig.h"
#include "apps/services/app_execution_metrics/executor_metrics_config.h"
#include "apps/services/app_resource_usage/app_resource_usage_config.h"
#include "apps/services/buffer_pool/buffer_pool_appconfig.h"
#include "apps/services/metrics/metrics_appconfig.h"
#include "apps/services/remote_control/remote_control_appconfig.h"
#include "apps/services/worker_manager/worker_manager_appconfig.h"
#include <string>

namespace ocudu {
namespace ocu {

/// F1AP configuration.
struct cu_f1ap_appconfig {
  /// F1-C bind addresses.
  std::vector<std::string> bind_addrs = {"127.0.10.1"};
  /// SCTP socket options.
  sctp_appconfig sctp;
};

/// Metrics report configuration.
struct metrics_appconfig {
  app_services::app_resource_usage_config rusage_config;
  app_services::metrics_appconfig         metrics_service_cfg;
  app_services::executor_metrics_config   executors_metrics_cfg;
};

} // namespace ocu

/// CU application configuration.
struct cu_appconfig {
  /// Default constructor to update the log filename.
  cu_appconfig() { log_cfg.filename = "/tmp/cu.log"; }
  /// Loggers configuration.
  logger_appconfig log_cfg;
  /// Tracers configuration.
  tracer_appconfig trace_cfg;
  /// Expert configuration.
  expert_execution_appconfig expert_execution_cfg;
  /// F1-U configuration.
  f1u_sockets_appconfig f1u_cfg;
  /// F1AP configuration.
  ocu::cu_f1ap_appconfig f1ap_cfg;
  /// Buffer pool configuration.
  app_services::buffer_pool_appconfig buffer_pool_config;
  /// Remote control configuration.
  remote_control_appconfig remote_control_config;
  /// Metrics configuration.
  ocu::metrics_appconfig metrics_cfg;
  /// Dryrun mode enabled flag.
  bool enable_dryrun = false;
};

} // namespace ocudu
