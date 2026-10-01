// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/ofh/ethernet/ethernet_mac_address.h"
#include "ocudu/support/units.h"

namespace ocudu {
namespace ether {

/// Configuration for the Ethernet transmitter.
struct transmitter_config {
  /// Ethernet interface name or identifier.
  std::string interface;
  /// Promiscuous mode flag.
  bool is_promiscuous_mode_enabled = false;
  /// Ethernet link status checking flag.
  bool is_link_status_check_enabled = false;
  /// If set to true, metrics are enabled in the Ethernet transmitter.
  bool are_metrics_enabled = false;
  /// MTU size.
  units::bytes mtu_size;
  /// Destination MAC address.
  mac_address mac_dst_address;
};

} // namespace ether
} // namespace ocudu
