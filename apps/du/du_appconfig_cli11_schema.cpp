// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "du_appconfig_cli11_schema.h"
#include "apps/helpers/f1u/f1u_cli11_schema.h"
#include "apps/helpers/hal/hal_cli11_schema.h"
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
#include "du_appconfig.h"
#include "ocudu/support/cli11_utils.h"

using namespace ocudu;

static void configure_cli11_metrics_args(CLI::App& app, odu::metrics_appconfig& metrics_params)
{
  add_option(
      app, "--autostart_stdout_metrics", metrics_params.autostart_stdout_metrics, "Autostart stdout metrics reporting")
      ->capture_default_str();
}

static void configure_cli11_f1ap_args(CLI::App& app, odu::f1ap_appconfig& f1c_params)
{
  add_option(app,
             "--addrs,--cu_cp_addr",
             f1c_params.cu_cp_addresses,
             "CU-CP F1-C addresses to connect to. Multiple addresses can be specified for SCTP multi-homing. The "
             "'--cu_cp_addr' name is a deprecated alias and should not be used.")
      ->capture_default_str();
  add_option(
      app,
      "--bind_addrs,--bind_addr",
      f1c_params.bind_addresses,
      "DU F1-C bind addresses. Multiple addresses can be specified for SCTP "
      "multi-homing. If left empty, implicit bind is performed. The '--bind_addr' name is a deprecated alias and "
      "should not be used.")
      ->capture_default_str();
  add_option(app,
             "--retry_connection",
             f1c_params.retry_connection,
             "Keep retrying the F1-C connection setup instead of closing the application when it fails. Allows the DU "
             "to be started before the CU-CP is reachable.")
      ->capture_default_str();
  configure_cli11_sctp_socket_args(app, f1c_params.sctp);

  CLI::App* dtls_subcmd = add_subcommand(app, "dtls", "DTLS configuration");
  configure_cli11_dtls_client_args(*dtls_subcmd, f1c_params.dtls);
}

static void configure_cli11_f1u_args(CLI::App& app, odu::f1u_appconfig& f1u_params)
{
  add_option(app, "--queue_size", f1u_params.pdu_queue_size, "F1-U PDU queue size")->capture_default_str();
  configure_cli11_f1u_sockets_args(app, f1u_params.f1u_sockets);
}

void ocudu::configure_cli11_with_du_appconfig_schema(CLI::App& app, du_appconfig& du_cfg)
{
  app.add_flag("--dryrun", du_cfg.enable_dryrun, "Enable application dry run mode")->capture_default_str();

  // Loggers section.
  configure_cli11_with_logger_appconfig_schema(app, du_cfg.log_cfg);

  // Tracers section.
  configure_cli11_with_tracer_appconfig_schema(app, du_cfg.trace_cfg);

  // Buffer pool section.
  configure_cli11_with_buffer_pool_appconfig_schema(app, du_cfg.buffer_pool_config);

  // Expert execution section.
  configure_cli11_with_worker_manager_appconfig_schema(app, du_cfg.expert_execution_cfg);

  // F1-C section.
  CLI::App* f1ap_subcmd = add_subcommand(app, "f1ap", "F1AP interface configuration");
  configure_cli11_f1ap_args(*f1ap_subcmd, du_cfg.f1ap_cfg);

  // F1-U section.
  CLI::App* f1u_subcmd = add_subcommand(app, "f1u", "F1-U interface configuration");
  configure_cli11_f1u_args(*f1u_subcmd, du_cfg.f1u_cfg);

  // Metrics section.
  CLI::App* metrics_subcmd = add_subcommand(app, "metrics", "Metrics configuration")->configurable();
  configure_cli11_metrics_args(*metrics_subcmd, du_cfg.metrics_cfg);
  app_services::configure_cli11_with_app_resource_usage_config_schema(app, du_cfg.metrics_cfg.rusage_config);
  app_services::configure_cli11_with_metrics_appconfig_schema(app, du_cfg.metrics_cfg.metrics_service_cfg);
  app_services::configure_cli11_with_executor_metrics_appconfig_schema(app, du_cfg.metrics_cfg.executors_metrics_cfg);

#ifdef DPDK_FOUND
  // HAL section.
  du_cfg.hal_config.emplace();
  configure_cli11_with_hal_appconfig_schema(app, *du_cfg.hal_config);
#else
  app.failure_message([](const CLI::App* application, const CLI::Error& e) -> std::string {
    if (std::string(e.what()).find("INI was not able to parse hal.++") == std::string::npos) {
      return CLI::FailureMessage::simple(application, e);
    }

    return "Invalid configuration detected, 'hal' section is present but the application was built without DPDK "
           "support\n" +
           CLI::FailureMessage::simple(application, e);
  });
#endif

  // Remote control section.
  configure_cli11_with_remote_control_appconfig_schema(app, du_cfg.remote_control_config);
}

#ifdef DPDK_FOUND
static void manage_hal_optional(CLI::App& app, du_appconfig& du_cfg)
{
  if (!is_hal_section_present(app)) {
    du_cfg.hal_config.reset();
  }
}
#endif

static void configure_default_f1u(du_appconfig& du_cfg)
{
  if (du_cfg.f1u_cfg.f1u_sockets.f1u_socket_cfg.empty()) {
    f1u_socket_appconfig default_f1u_cfg;
    default_f1u_cfg.bind_addr = "127.0.10.2";
    du_cfg.f1u_cfg.f1u_sockets.f1u_socket_cfg.push_back(default_f1u_cfg);
  }
}

void ocudu::autoderive_du_parameters_after_parsing(CLI::App& app, du_appconfig& du_cfg)
{
#ifdef DPDK_FOUND
  manage_hal_optional(app, du_cfg);
#endif

  configure_default_f1u(du_cfg);
}
