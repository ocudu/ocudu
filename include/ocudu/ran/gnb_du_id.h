// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include <cstdint>

namespace ocudu {

/// Global NodeB DU ID as per TS 38.473.
enum class gnb_du_id_t : uint64_t { min = 0, max = 68719476735, invalid = max + 1 };

constexpr gnb_du_id_t int_to_gnb_du_id(uint64_t id)
{
  return static_cast<gnb_du_id_t>(id);
}

} // namespace ocudu
