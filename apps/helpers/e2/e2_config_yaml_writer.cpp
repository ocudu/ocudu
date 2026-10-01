// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "e2_config_yaml_writer.h"
#include "apps/helpers/e2/e2_appconfig.h"
#include "apps/helpers/network/sctp_config_yaml_writer.h"

using namespace ocudu;

void ocudu::fill_e2_config_in_yaml_schema(YAML::Node node, const e2_config& config)
{
  node["addrs"]      = config.ip_addrs;
  node["port"]       = config.port;
  node["bind_addrs"] = config.bind_addrs;
  fill_sctp_config_in_yaml_schema(node, config.sctp);
  node["e2sm_kpm_enabled"] = config.e2sm_kpm_enabled;
  node["e2sm_rc_enabled"]  = config.e2sm_rc_enabled;
  node["e2sm_ccc_enabled"] = config.e2sm_ccc_enabled;
}
