// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "apps/helpers/network/udp_appconfig.h"
#include "ocudu/gtpu/gtpu_config.h"
#include "ocudu/ran/qos/five_qi.h"
#include <vector>

namespace ocudu {

/// F1-U sockets configuration.
struct f1u_socket_appconfig {
  /// Bind address used by the F1-U interface.
  std::string bind_addr = "127.0.10.1";
  /// If the S-NSSAI is not present, the socket will be used by default.
  std::optional<uint8_t>  sst;
  std::optional<uint32_t> sd;
  /// If the 5QI is not present, the socket will be used by default.
  std::optional<five_qi_t> five_qi;
  udp_appconfig            udp_config;
};

/// F1-U configuration.
struct f1u_sockets_appconfig {
  /// Bind port used by the F1-U interface.
  uint16_t bind_port = GTPU_PORT;
  /// Peer port used by the F1-U interface.
  uint16_t                          peer_port = GTPU_PORT;
  std::vector<f1u_socket_appconfig> f1u_socket_cfg;
};

} // namespace ocudu
