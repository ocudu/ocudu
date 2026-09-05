// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/support/enum_utils.h"
#include <cstdint>

namespace ocudu {

/// \brief Global NodeB CU UP ID used to uniquely identify the gNB-CU-UP at least within a gNB-CU-CP.
/// \remark See TS 38.463 Section 9.3.1.15: GNB-CU-UP-ID valid values: (0..2^36-1).
enum class gnb_cu_up_id_t : uint64_t { min = 0, max = 68719476735, invalid = max + 1 };

/// Converts an integer to a GNB-CU-UP-ID type.
constexpr gnb_cu_up_id_t uint_to_gnb_cu_up_id(uint64_t id)
{
  return static_cast<gnb_cu_up_id_t>(id);
}

constexpr uint64_t format_as(gnb_cu_up_id_t o)
{
  return to_underlying(o);
}

} // namespace ocudu
