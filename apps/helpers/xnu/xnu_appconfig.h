// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "apps/helpers/network/udp_appconfig.h"
#include "ocudu/gtpu/gtpu_config.h"
#include "ocudu/ran/qos/five_qi.h"
#include <vector>

namespace ocudu {

/// Xn-U sockets configuration.
struct xnu_socket_appconfig {
  /// Bind address used by the Xn-U interface.
  std::string bind_addr = "127.0.50.1";
  /// If the S-NSSAI is not present, the socket will be used by default.
  std::optional<uint8_t>  sst;
  std::optional<uint32_t> sd;
  /// If the 5QI is not present, the socket will be used by default.
  std::optional<five_qi_t> five_qi;
  udp_appconfig            udp_config;
};

/// Xn-U configuration.
struct xnu_sockets_appconfig {
  /// Bind port used by the Xn-U interface.
  uint16_t bind_port = GTPU_PORT;
  /// Peer port used by the Xn-U interface.
  uint16_t                          peer_port = GTPU_PORT;
  std::vector<xnu_socket_appconfig> xnu_socket_cfg;
};

} // namespace ocudu
