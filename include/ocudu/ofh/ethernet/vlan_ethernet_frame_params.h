// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/ofh/ethernet/ethernet_mac_address.h"
#include "ocudu/ofh/ethernet/ethernet_vlan_params.h"
#include <optional>

namespace ocudu {
namespace ether {

/// VLAN Ethernet frame header parameters.
struct vlan_frame_params {
  /// Destination MAC address.
  mac_address mac_dst_address;
  /// Source MAC address.
  mac_address mac_src_address;
  /// VLAN configuration parameters.
  std::optional<vlan_parameters> vlan_config;
  /// Ethernet type field.
  uint16_t eth_type;
};

} // namespace ether
} // namespace ocudu
