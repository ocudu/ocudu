// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "fapi_config_yaml_writer.h"
#include "fapi_config.h"

using namespace ocudu;

static void fill_du_low_log_section(YAML::Node node, ocudulog::basic_levels level)
{
  node["fapi_level"] = ocudulog::basic_level_to_string(level);
}

void ocudu::fill_fapi_config_in_yaml_schema(YAML::Node& node, const fapi_unit_config& config)
{
  fill_du_low_log_section(node["log"], config.fapi_level);
}
