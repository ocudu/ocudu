// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "metrics_config_yaml_writer.h"
#include "metrics_config.h"

using namespace ocudu;
using namespace app_helpers;

void ocudu::app_helpers::fill_metrics_appconfig_in_yaml_schema(YAML::Node& node, const metrics_config& config)
{
  YAML::Node log_node        = node["metrics"];
  log_node["enable_log"]     = config.enable_log_metrics;
  log_node["enable_json"]    = config.enable_json_metrics;
  log_node["enable_verbose"] = config.enable_verbose;
}
