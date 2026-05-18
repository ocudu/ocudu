// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "cu_appconfig_cli11_schema.h"
#include "apps/helpers/f1u/f1u_cli11_schema.h"
#include "apps/helpers/logger/logger_appconfig_cli11_schema.h"
#include "apps/helpers/network/sctp_cli11_schema.h"
#include "apps/helpers/tracing/tracer_appconfig_cli11_schema.h"
#include "apps/services/app_execution_metrics/executor_metrics_config_cli11_schema.h"
#include "apps/services/app_resource_usage/app_resource_usage_config_cli11_schema.h"
#include "apps/services/buffer_pool/buffer_pool_appconfig_cli11_schema.h"
#include "apps/services/metrics/metrics_config_cli11_schema.h"
#include "apps/services/remote_control/remote_control_appconfig_cli11_schema.h"
#include "apps/services/worker_manager/worker_manager_cli11_schema.h"
#include "cu_appconfig.h"
#include "ocudu/support/cli11_utils.h"

using namespace ocudu;

static void configure_cli11_f1ap_args(CLI::App& app, ocu::cu_f1ap_appconfig& f1ap_params)
{
  add_option(
      app,
      "--bind_addrs,--bind_addr", // TODO: old name kept for backward compatibility, should be removed in the future
      f1ap_params.bind_addrs,
      "CU F1-C bind addresses. Multiple addresses can be specified for SCTP multi-homing")
      ->capture_default_str();
  configure_cli11_sctp_socket_args(app, f1ap_params.sctp);
}

void ocudu::configure_cli11_with_cu_appconfig_schema(CLI::App& app, cu_appconfig& cu_cfg)
{
  app.add_flag("--dryrun", cu_cfg.enable_dryrun, "Enable application dry run mode")->capture_default_str();

  // Logging section.
  configure_cli11_with_logger_appconfig_schema(app, cu_cfg.log_cfg);

  // Tracing section.
  configure_cli11_with_tracer_appconfig_schema(app, cu_cfg.trace_cfg);

  // Buffer pool section.
  configure_cli11_with_buffer_pool_appconfig_schema(app, cu_cfg.buffer_pool_config);

  // Expert execution section.
  configure_cli11_with_worker_manager_appconfig_schema(app, cu_cfg.expert_execution_cfg);

  // Remote control section.
  configure_cli11_with_remote_control_appconfig_schema(app, cu_cfg.remote_control_config);

  // Metrics section.
  app_services::configure_cli11_with_app_resource_usage_config_schema(app, cu_cfg.metrics_cfg.rusage_config);
  app_services::configure_cli11_with_metrics_appconfig_schema(app, cu_cfg.metrics_cfg.metrics_service_cfg);
  app_services::configure_cli11_with_executor_metrics_appconfig_schema(app, cu_cfg.metrics_cfg.executors_metrics_cfg);

  // F1AP section.
  CLI::App* cu_cp_subcmd = add_subcommand(app, "cu_cp", "CU-CP parameters")->configurable();
  CLI::App* f1ap_subcmd  = add_subcommand(*cu_cp_subcmd, "f1ap", "F1AP parameters")->configurable();
  configure_cli11_f1ap_args(*f1ap_subcmd, cu_cfg.f1ap_cfg);

  // NR-U section.
  CLI::App* cu_up_subcmd = add_subcommand(app, "cu_up", "CU-UP parameters")->configurable();
  CLI::App* f1u_subcmd   = add_subcommand(*cu_up_subcmd, "f1u", "F1-U parameters")->configurable();
  configure_cli11_f1u_sockets_args(*f1u_subcmd, cu_cfg.f1u_cfg);
}
