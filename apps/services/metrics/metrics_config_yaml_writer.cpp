// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "metrics_config_yaml_writer.h"
#include "metrics_appconfig.h"

using namespace ocudu;
using namespace app_services;

void ocudu::app_services::fill_metrics_appconfig_in_yaml_schema(YAML::Node& node, const metrics_appconfig& config)
{
  YAML::Node metrics_node                     = node["metrics"];
  YAML::Node periodicity_node                 = metrics_node["periodicity"];
  periodicity_node["app_usage_report_period"] = config.app_usage_report_period;
}
