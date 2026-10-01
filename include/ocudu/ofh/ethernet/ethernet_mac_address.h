// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include <array>
#include <cstdint>

namespace ocudu {
namespace ether {

/// Size in bytes of an Ethernet MAC address.
constexpr unsigned ETH_ADDR_LEN = 6;

/// Type alias for an Ethernet MAC address.
using mac_address = std::array<uint8_t, ETH_ADDR_LEN>;

} // namespace ether
} // namespace ocudu
