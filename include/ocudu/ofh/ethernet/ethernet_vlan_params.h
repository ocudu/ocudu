// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include <stdint.h>

namespace ocudu {
namespace ether {

/// VLAN Ethernet configuration parameters.
struct vlan_parameters {
  /// Tag control information VLAN identifier field.
  uint16_t tci_vid;
  /// Tag control information Priority code point (PCP) field.
  uint8_t tci_pcp = 0;
};

} // namespace ether
} // namespace ocudu
