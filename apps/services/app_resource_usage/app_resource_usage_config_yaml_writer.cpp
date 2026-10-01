// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "app_resource_usage_config_yaml_writer.h"
#include "app_resource_usage_config.h"
#include "apps/helpers/metrics/metrics_config_yaml_writer.h"

using namespace ocudu;

static void fill_metrics_section(YAML::Node node, const app_services::app_resource_usage_config& config)
{
  auto layers_node                = node["layers"];
  layers_node["enable_app_usage"] = config.enable_app_usage;
}

void app_services::fill_app_resource_usage_config_in_yaml_schema(YAML::Node&                      node,
                                                                 const app_resource_usage_config& config)
{
  app_helpers::fill_metrics_appconfig_in_yaml_schema(node, config.metrics_consumers_cfg);
  fill_metrics_section(node["metrics"], config);
}
