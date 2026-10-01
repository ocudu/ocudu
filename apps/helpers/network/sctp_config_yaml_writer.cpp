// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "sctp_config_yaml_writer.h"
#include "sctp_appconfig.h"

using namespace ocudu;

void ocudu::fill_sctp_config_in_yaml_schema(YAML::Node node, const sctp_appconfig& config)
{
  node["sctp_rto_initial"]       = config.rto_initial_ms;
  node["sctp_rto_min"]           = config.rto_min_ms;
  node["sctp_rto_max"]           = config.rto_max_ms;
  node["sctp_init_max_attempts"] = config.init_max_attempts;
  node["sctp_max_init_timeo"]    = config.max_init_timeo_ms;
  node["sctp_hb_interval"]       = config.hb_interval_ms;
  node["sctp_assoc_max_retx"]    = config.assoc_max_retx;
  node["sctp_nodelay"]           = config.nodelay;
}
