// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

namespace ocudu {

/// Array of bytes used to store the UE Contention Resolution Id.
constexpr size_t UE_CON_RES_ID_LEN = 6;
using ue_con_res_id_t              = std::array<uint8_t, UE_CON_RES_ID_LEN>;

} // namespace ocudu
