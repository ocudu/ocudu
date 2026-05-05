// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "cu_up_appconfig_cli11_schema.h"
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
#include "cu_up_appconfig.h"
#include "ocudu/support/cli11_utils.h"
#include "ocudu/support/config_parsers.h"

using namespace ocudu;

static void configure_cli11_e1ap_args(CLI::App& app, ocuup::e1ap_appconfig& e1ap_params)
{
  app.add_option(
         "--addrs,--cu_cp_addr", // TODO: old name kept for backward compatibility, should be removed in the future
         e1ap_params.cu_cp_addresses,
         "CU-CP addresses to be used for E1 interface. Multiple addresses can be specified for SCTP multi-homing")
      ->capture_default_str();
  app.add_option(
         "--bind_addrs,--bind_addr", // TODO: old name kept for backward compatibility, should be removed in the future
         e1ap_params.bind_addresses,
         "CU-UP bind addresses to be used for E1 interface. Multiple addresses can be specified for SCTP "
         "multi-homing. If left empty, implicit bind is performed")
      ->capture_default_str();
  configure_cli11_sctp_socket_args(app, e1ap_params.sctp);
}

static void configure_cli11_e1ap_list_args(CLI::App& app, ocuup::e1ap_list_appconfig& e1ap_params)
{
  // Add option for multiple sockets, for usage with different slices, 5QIs or parallization.
  auto sock_lambda = [&e1ap_params](const std::vector<std::string>& values) {
    // Prepare the radio bearers
    e1ap_params.e1ap_cfgs.resize(values.size());

    // Format every F1-U socket configuration.
    for (unsigned i = 0, e = values.size(); i != e; ++i) {
      CLI::App subapp("E1AP parameters", "E1AP socket config, item #" + std::to_string(i));

      subapp.config_formatter(create_yaml_config_parser());
      subapp.allow_config_extras(CLI::config_extras_mode::capture);
      configure_cli11_e1ap_args(subapp, e1ap_params.e1ap_cfgs[i]);
      std::istringstream ss(values[i]);
      subapp.parse_from_stream(ss);
    }
  };
  add_option_cell(app, "gateways", sock_lambda, "Configures UDP/IP socket parameters of the F1-U interface");
}

void ocudu::configure_cli11_with_cu_appconfig_schema(CLI::App& app, cu_up_appconfig& cu_up_cfg)
{
  app.add_flag("--dryrun", cu_up_cfg.enable_dryrun, "Enable application dry run mode")->capture_default_str();

  // Logging section.
  configure_cli11_with_logger_appconfig_schema(app, cu_up_cfg.log_cfg);

  // Tracers section.
  configure_cli11_with_tracer_appconfig_schema(app, cu_up_cfg.trace_cfg);

  // Buffer pool section.
  configure_cli11_with_buffer_pool_appconfig_schema(app, cu_up_cfg.buffer_pool_config);

  // Expert execution section.
  configure_cli11_with_worker_manager_appconfig_schema(app, cu_up_cfg.expert_execution_cfg);

  // Remote control section.
  configure_cli11_with_remote_control_appconfig_schema(app, cu_up_cfg.remote_control_config);

  // Metrics section.
  app_services::configure_cli11_with_app_resource_usage_config_schema(app, cu_up_cfg.metrics_cfg.rusage_config);
  app_services::configure_cli11_with_metrics_appconfig_schema(app, cu_up_cfg.metrics_cfg.metrics_service_cfg);
  app_services::configure_cli11_with_executor_metrics_appconfig_schema(app,
                                                                       cu_up_cfg.metrics_cfg.executors_metrics_cfg);

  CLI::App* cu_up_subcmd = add_subcommand(app, "cu_up", "CU-UP parameters")->configurable();

  // E1AP section.
  CLI::App* e1ap_subcmd = add_subcommand(*cu_up_subcmd, "e1ap", "E1AP parameters")->configurable();
  configure_cli11_e1ap_list_args(*e1ap_subcmd, cu_up_cfg.e1ap_cfg);
  // NR-U section.
  CLI::App* f1u_subcmd = add_subcommand(*cu_up_subcmd, "f1u", "F1-U parameters")->configurable();
  configure_cli11_f1u_sockets_args(*f1u_subcmd, cu_up_cfg.f1u_cfg);
}
