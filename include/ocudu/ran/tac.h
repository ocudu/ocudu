// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/static_vector.h"
#include <cstdint>

namespace ocudu {

/// 3GPP TS 38.331, TrackingAreaCode ::= BIT STRING (SIZE (24)).
using tac_t                 = uint32_t;
constexpr tac_t INVALID_TAC = 16777216;

/// Returns true if the given struct is valid, otherwise false.
constexpr bool is_valid(tac_t tac)
{
  // TAC must be in [0..16777215].
  return tac < INVALID_TAC;
}

/// Max TACs a cell may broadcast per PLMN: maxTAC-r17 (TS 38.331) and maxnoofTACsinNTN (TS 38.413), both 12.
constexpr size_t MAX_NOF_TACS_NTN = 12;

/// List of TACs broadcast by a cell in \c trackingAreaList, TS 38.331.
using tac_list_t = static_vector<tac_t, MAX_NOF_TACS_NTN>;

} // namespace ocudu
