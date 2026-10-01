// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "udp_config_yaml_writer.h"
#include "udp_appconfig.h"

using namespace ocudu;

/// Fills the UDP configuration in the given YAML node.
void ocudu::fill_udp_config_in_yaml_schema(YAML::Node node, const udp_appconfig& config)
{
  node["max_rx_msgs"]     = config.rx_max_msgs;
  node["tx_qsize"]        = config.tx_qsize;
  node["max_tx_msgs"]     = config.tx_max_msgs;
  node["max_tx_segments"] = config.tx_max_segments;
  node["pool_threshold"]  = config.pool_threshold;
  node["reuse_addr"]      = config.reuse_addr;
  if (config.dscp.has_value()) {
    node["dscp"] = *config.dscp;
  }
}
