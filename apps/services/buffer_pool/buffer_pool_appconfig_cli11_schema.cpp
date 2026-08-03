// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "apps/services/buffer_pool/buffer_pool_appconfig_cli11_schema.h"
#include "apps/helpers/metrics/metrics_config_cli11_schema.h"
#include "apps/services/buffer_pool/buffer_pool_appconfig.h"
#include "ocudu/support/cli11_utils.h"

using namespace ocudu;
using namespace app_services;

static void configure_cli11_buffer_pool_args(CLI::App& app, buffer_pool_appconfig& config)
{
  add_option(app, "--nof_segments", config.nof_segments, "Number of segments allocated by the buffer pool")
      ->capture_default_str();
  add_option(app, "--segment_size", config.segment_size, "Size of each buffer pool segment in bytes")
      ->capture_default_str();
  app.add_option("--use_hugepages", config.use_hugepages, "Enable huge page backing")->always_capture_default();
}

static void configure_cli11_metrics_args(CLI::App& app, bool& enable_metrics)
{
  auto* layers_subcmd = add_subcommand(app, "layers", "Layer basis metrics configuration")->configurable();
  add_option(*layers_subcmd, "--enable_app_usage", enable_metrics, "Enable application usage metrics")
      ->capture_default_str();
}

void ocudu::app_services::configure_cli11_with_buffer_pool_appconfig_schema(CLI::App&              app,
                                                                            buffer_pool_appconfig& config)
{
  // Buffer pool section.
  CLI::App* buffer_pool_subcmd = add_subcommand(app, "buffer_pool", "Buffer pool configuration");
  configure_cli11_buffer_pool_args(*buffer_pool_subcmd, config);

  // Metrics section.
  CLI::App* metrics_subcmd = add_subcommand(app, "metrics", "Metrics configuration")->configurable();
  configure_cli11_metrics_args(*metrics_subcmd, config.metrics_config.enable_metrics);
  app_helpers::configure_cli11_with_metrics_appconfig_schema(app, config.metrics_config.common_metrics_cfg);
}
