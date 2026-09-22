// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "hal_appconfig_yaml_writer.h"
#include "hal_appconfig.h"

using namespace ocudu;

void ocudu::fill_hal_appconfig_section(YAML::Node node, const hal_appconfig& config)
{
  YAML::Node hal_node           = node["hal"];
  hal_node["eal_args"]          = config.eal_args;
  hal_node["enable_pdump_init"] = config.enable_pdump_init;
}
