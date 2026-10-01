// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/ofh/ethernet/ethernet_mac_address.h"
#include "ocudu/support/units.h"

namespace ocudu {
namespace ether {

/// VLAN Tag protocol identifier.
constexpr uint16_t VLAN_TPID = 0x8100;

/// Ethernet header size.
constexpr units::bytes ETH_HEADER_SIZE{ETH_ADDR_LEN * 2 + sizeof(uint16_t)};

/// Ethernet VLAN tag size.
constexpr units::bytes ETH_VLAN_TAG_SIZE{sizeof(uint16_t) * 2};

} // namespace ether
} // namespace ocudu
