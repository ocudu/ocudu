// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "du_low_appconfig_cli11_schema.h"
#include "apps/helpers/hal/hal_cli11_schema.h"
#include "apps/helpers/logger/logger_appconfig_cli11_schema.h"
#include "apps/helpers/tracing/tracer_appconfig_cli11_schema.h"
#include "apps/services/app_execution_metrics/executor_metrics_config_cli11_schema.h"
#include "apps/services/app_resource_usage/app_resource_usage_config_cli11_schema.h"
#include "apps/services/metrics/metrics_config_cli11_schema.h"
#include "apps/services/remote_control/remote_control_appconfig_cli11_schema.h"
#include "apps/services/worker_manager/worker_manager_cli11_schema.h"
#include "du_low_appconfig.h"

using namespace ocudu;

void ocudu::configure_cli11_with_du_low_appconfig_schema(CLI::App& app, du_low_appconfig& config)
{
  app.add_flag("--dryrun", config.enable_dryrun, "Enable application dry run mode")->capture_default_str();

  // Loggers section.
  configure_cli11_with_logger_appconfig_schema(app, config.log_cfg);

  // Tracers section.
  configure_cli11_with_tracer_appconfig_schema(app, config.trace_cfg);

  // Expert execution section.
  configure_cli11_with_worker_manager_appconfig_schema(app, config.expert_execution_cfg);

  // Metrics section.
  app_services::configure_cli11_with_executor_metrics_appconfig_schema(app, config.metrics_cfg.executors_metrics_cfg);
  app_services::configure_cli11_with_app_resource_usage_config_schema(app, config.metrics_cfg.rusage_config);
  app_services::configure_cli11_with_metrics_appconfig_schema(app, config.metrics_cfg.metrics_service_cfg);

  // Remote control section.
  configure_cli11_with_remote_control_appconfig_schema(app, config.remote_control_config);

#ifdef DPDK_FOUND
  // HAL section.
  config.hal_config.emplace();
  configure_cli11_with_hal_appconfig_schema(app, *config.hal_config);
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
}

#ifdef DPDK_FOUND
static void manage_hal_optional(CLI::App& app, du_low_appconfig& gnb_cfg)
{
  if (!is_hal_section_present(app)) {
    gnb_cfg.hal_config.reset();
  }
}
#endif

void ocudu::autoderive_du_low_parameters_after_parsing(CLI::App& app, du_low_appconfig& config)
{
#ifdef DPDK_FOUND
  manage_hal_optional(app, config);
#endif
}
