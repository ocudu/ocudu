// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include <optional>
#include <string>

namespace ocudu {

/// UDP specific configuration of an UDP gateway.
struct udp_appconfig {
  /// Maximum amount of messages RX in a single syscall.
  unsigned rx_max_msgs = 256;
  /// Batched queue size.
  unsigned tx_qsize = 4096;
  /// Maximum amount of messages TX in a single syscall.
  unsigned tx_max_msgs = 256;
  /// Maximum amount of segments in a single TX SDU.
  unsigned tx_max_segments = 256;
  /// Pool accupancy threshold after which packets are dropped.
  float pool_threshold = 0.9;
  /// Differentiated Services Code Point value.
  std::optional<unsigned> dscp;
  /// Allow multiple sockets to re-use the bind port.
  bool reuse_addr = false;
  /// External address advertised by the UDP-GW.
  std::string ext_addr = "auto";
};

} // namespace ocudu
