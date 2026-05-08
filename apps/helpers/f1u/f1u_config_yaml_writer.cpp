// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "apps/helpers/f1u/f1u_config_yaml_writer.h"
#include "apps/helpers/f1u/f1u_appconfig.h"
#include "apps/helpers/network/udp_config_yaml_writer.h"
#include <yaml-cpp/yaml.h>

using namespace ocudu;

static void fill_cu_up_f1u_socket_entry(YAML::Node& node, const f1u_socket_appconfig& config)
{
  node["bind_addr"] = config.bind_addr;
  if (!config.udp_config.ext_addr.empty()) {
    node["ext_addr"] = config.udp_config.ext_addr;
  }
  if (config.sst.has_value()) {
    node["sst"] = static_cast<unsigned>(*config.sst);
  }
  if (config.sd.has_value()) {
    node["sd"] = *config.sd;
  }
  if (config.five_qi.has_value()) {
    node["five_qi"] = five_qi_to_uint(*config.five_qi);
  }

  fill_udp_config_in_yaml_schema(node["udp"], config.udp_config);
}

/// Fills the UDP configuration in the given YAML node.
static void fill_f1u_socket_section(YAML::Node& node, const std::vector<f1u_socket_appconfig>& sock_cfg)
{
  auto sock_node = node["socket"];
  for (const auto& cfg : sock_cfg) {
    YAML::Node node_sock;
    fill_cu_up_f1u_socket_entry(node_sock, cfg);
    sock_node.push_back(node_sock);
  }
}

void ocudu::fill_f1u_config_yaml_schema(YAML::Node& node, const f1u_sockets_appconfig& config)
{
  node["bind_port"] = config.bind_port;
  node["peer_port"] = config.peer_port;
  fill_f1u_socket_section(node, config.f1u_socket_cfg);
}
