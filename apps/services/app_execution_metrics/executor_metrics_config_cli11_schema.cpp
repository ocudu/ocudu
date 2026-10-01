// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "executor_metrics_config_cli11_schema.h"
#include "apps/helpers/metrics/metrics_config_cli11_schema.h"
#include "executor_metrics_config.h"
#include "ocudu/ran/slot_point.h"
#include "ocudu/ran/slot_point_extended.h"
#include "ocudu/support/cli11_utils.h"

using namespace ocudu;
using namespace app_services;

static void configure_cli11_metrics_args(CLI::App& app, executor_metrics_config& config)
{
  auto* layers_subcmd = add_subcommand(app, "layers", "Layer basis metrics configuration")->configurable();
  add_option(*layers_subcmd,
             "--enable_executor",
             config.enable_executor_metrics,
             "Whether to log application executors metrics")
      ->capture_default_str();

  auto* periodicity_subcmd = add_subcommand(app, "periodicity", "Metrics periodicity configuration")->configurable();
  add_option(*periodicity_subcmd,
             "--executors_report_period",
             config.report_period_ms,
             "Executors metrics report period in milliseconds")
      ->capture_default_str()
      ->range(0U, static_cast<unsigned>(NOF_SUBFRAMES_PER_FRAME * NOF_SFNS * NOF_HYPER_SFNS));
}

void ocudu::app_services::configure_cli11_with_executor_metrics_appconfig_schema(CLI::App&                app,
                                                                                 executor_metrics_config& config)
{
  CLI::App* metrics_subcmd = add_subcommand(app, "metrics", "Metrics configuration")->configurable();
  configure_cli11_metrics_args(*metrics_subcmd, config);
  app_helpers::configure_cli11_with_metrics_appconfig_schema(app, config.common_metrics_cfg);
}
