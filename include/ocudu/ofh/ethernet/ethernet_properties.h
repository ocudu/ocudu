// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include <cstdint>

namespace ocudu {
namespace ether {

/// Ethernet type for the eCPRI.
constexpr uint16_t ECPRI_ETH_TYPE = 0xaefe;

/// Maximum length of Ethernet Jumbo frame.
constexpr unsigned MAX_ETH_FRAME_LENGTH = 9600;

/// Minimal Ethernet frame length.
constexpr unsigned MIN_ETH_FRAME_LENGTH = 64;

/// Maximum number of frames allowed to be transmitted in each symbol.
constexpr unsigned MAX_TX_BURST_SIZE = 256;

} // namespace ether
} // namespace ocudu
