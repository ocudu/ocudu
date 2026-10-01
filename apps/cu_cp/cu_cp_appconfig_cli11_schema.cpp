// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "cu_cp_appconfig_cli11_schema.h"
#include "apps/helpers/logger/logger_appconfig_cli11_schema.h"
#include "apps/helpers/network/dtls_cli11_schema.h"
#include "apps/helpers/network/sctp_cli11_schema.h"
#include "apps/helpers/tracing/tracer_appconfig_cli11_schema.h"
#include "apps/services/app_execution_metrics/executor_metrics_config_cli11_schema.h"
#include "apps/services/app_resource_usage/app_resource_usage_config_cli11_schema.h"
#include "apps/services/buffer_pool/buffer_pool_appconfig_cli11_schema.h"
#include "apps/services/metrics/metrics_config_cli11_schema.h"
#include "apps/services/remote_control/remote_control_appconfig_cli11_schema.h"
#include "apps/services/worker_manager/worker_manager_cli11_schema.h"
#include "cu_cp_appconfig.h"
#include "ocudu/support/cli11_utils.h"

using namespace ocudu;

static void configure_cli11_e1ap_args(CLI::App& app, ocucp::e1ap_appconfig& e1ap_params)
{
  add_option(app,
             "--bind_addrs,--bind_addr",
             e1ap_params.bind_addrs,
             "E1 bind addresses. The '--bind_addr' name is a deprecated alias and should not be used.")
      ->capture_default_str();
  configure_cli11_sctp_socket_args(app, e1ap_params.sctp);

  CLI::App* dtls = add_subcommand(app, "dtls", "DTLS parameters")->configurable();
  configure_cli11_dtls_server_args(*dtls, e1ap_params.dtls);
}

static void configure_cli11_f1ap_args(CLI::App& app, ocucp::f1ap_appconfig& f1ap_params)
{
  add_option(
      app,
      "--bind_addrs,--bind_addr",
      f1ap_params.bind_addrs,
      "CU-CP F1-C bind addresses. Multiple addresses can be specified for SCTP multi-homing. The '--bind_addr' name "
      "is a deprecated alias and should not be used.")
      ->capture_default_str();
  configure_cli11_sctp_socket_args(app, f1ap_params.sctp);

  CLI::App* dtls_subcmd = add_subcommand(app, "dtls", "DTLS configuration");
  configure_cli11_dtls_server_args(*dtls_subcmd, f1ap_params.dtls);
}

void ocudu::configure_cli11_with_cu_cp_appconfig_schema(CLI::App& app, cu_cp_appconfig& cu_cp_cfg)
{
  app.add_flag("--dryrun", cu_cp_cfg.enable_dryrun, "Enable application dry run mode")->capture_default_str();

  // Logging section.
  configure_cli11_with_logger_appconfig_schema(app, cu_cp_cfg.log_cfg);

  // Tracers section.
  configure_cli11_with_tracer_appconfig_schema(app, cu_cp_cfg.trace_cfg);

  // Buffer pool section.
  configure_cli11_with_buffer_pool_appconfig_schema(app, cu_cp_cfg.buffer_pool_config);

  // Expert execution section.
  configure_cli11_with_worker_manager_appconfig_schema(app, cu_cp_cfg.expert_execution_cfg);

  // Remote control section.
  configure_cli11_with_remote_control_appconfig_schema(app, cu_cp_cfg.remote_control_config);

  // Metrics section.
  app_services::configure_cli11_with_app_resource_usage_config_schema(app, cu_cp_cfg.metrics_cfg.rusage_config);
  app_services::configure_cli11_with_metrics_appconfig_schema(app, cu_cp_cfg.metrics_cfg.metrics_service_cfg);
  app_services::configure_cli11_with_executor_metrics_appconfig_schema(app,
                                                                       cu_cp_cfg.metrics_cfg.executors_metrics_cfg);

  CLI::App* cu_cp_subcmd = add_subcommand(app, "cu_cp", "CU-CP parameters")->configurable();
  // E1AP section.
  CLI::App* e1ap_subcmd = add_subcommand(*cu_cp_subcmd, "e1ap", "E1AP parameters")->configurable();
  configure_cli11_e1ap_args(*e1ap_subcmd, cu_cp_cfg.e1ap_cfg);
  // F1AP section.
  CLI::App* f1ap_subcmd = add_subcommand(*cu_cp_subcmd, "f1ap", "F1AP parameters")->configurable();
  configure_cli11_f1ap_args(*f1ap_subcmd, cu_cp_cfg.f1ap_cfg);
}
