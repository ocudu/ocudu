// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "tracer_appconfig_yaml_writer.h"
#include "tracer_appconfig.h"

using namespace ocudu;

void ocudu::fill_tracer_appconfig_in_yaml_schema(YAML::Node& node, const tracer_appconfig& config)
{
  YAML::Node log_node                          = node["trace"];
  log_node["filename"]                         = config.filename;
  log_node["max_tracing_events_per_file"]      = config.max_tracing_events_per_file;
  log_node["nof_tracing_events_before_severe"] = config.nof_tracing_events_before_severe;
}
