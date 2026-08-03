// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "buffer_pool_config_yaml_writer.h"
#include "apps/helpers/metrics/metrics_config_yaml_writer.h"
#include "buffer_pool_appconfig.h"

using namespace ocudu;
using namespace app_services;

static void fill_buffer_pool_section(YAML::Node node, const buffer_pool_appconfig& config)
{
  auto buffer_pool_node             = node["buffer_pool"];
  buffer_pool_node["nof_segments"]  = config.nof_segments;
  buffer_pool_node["segment_size"]  = config.segment_size;
  buffer_pool_node["use_hugepages"] = config.use_hugepages;
}

void ocudu::app_services::fill_buffer_pool_config_in_yaml_schema(YAML::Node& node, const buffer_pool_appconfig& config)
{
  app_helpers::fill_metrics_appconfig_in_yaml_schema(node, config.metrics_config.common_metrics_cfg);
  fill_buffer_pool_section(node, config);
}
