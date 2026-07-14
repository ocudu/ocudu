// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "logger_appconfig_yaml_writer.h"
#include "logger_appconfig.h"

using namespace ocudu;

void ocudu::fill_logger_appconfig_in_yaml_schema(YAML::Node& node, const logger_appconfig& config)
{
  YAML::Node log_node          = node["log"];
  log_node["filename"]         = config.filename;
  log_node["all_level"]        = ocudulog::basic_level_to_string(config.all_level);
  log_node["lib_level"]        = ocudulog::basic_level_to_string(config.lib_level);
  log_node["e2ap_level"]       = ocudulog::basic_level_to_string(config.e2ap_level);
  log_node["config_level"]     = ocudulog::basic_level_to_string(config.config_level);
  log_node["hex_max_size"]     = config.hex_max_size;
  log_node["flush_app_banner"] = config.flush_app_banner;
}
