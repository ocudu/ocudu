// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "o_du_high_e2_config_yaml_writer.h"
#include "apps/helpers/e2/e2_config_yaml_writer.h"
#include "o_du_high_e2_config.h"

using namespace ocudu;

static void fill_o_du_high_e2_pcap_section(YAML::Node node, const o_du_high_e2_pcap_config& config)
{
  node["e2ap_du_filename"] = config.filename;
  node["e2ap_enable"]      = config.enabled;
}

void ocudu::fill_o_du_high_e2_config_in_yaml_schema(YAML::Node& node, const o_du_high_e2_config& config)
{
  YAML::Node e2_node      = node["e2"];
  e2_node["enable_du_e2"] = config.base_cfg.enable_unit_e2;
  fill_e2_config_in_yaml_schema(e2_node, config.base_cfg);

  // PCAPs.
  fill_o_du_high_e2_pcap_section(node["pcap"], config.pcaps);
}
