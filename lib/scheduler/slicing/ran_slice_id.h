// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/strong_type.h"
#include <cstdint>

namespace ocudu {

/// RAN slice identifier that should be unique for a given cell,PLMN,S-NSSAI.
struct ran_slice_id_tag {};
using ran_slice_id_t = strong_type<uint8_t,
                                   struct ran_slice_id_tag,
                                   strong_increment_decrement,
                                   strong_equality,
                                   strong_comparison,
                                   strong_conversion_to<std::size_t>>;

/// RAN slice ID for slice used to schedule SRB(s) traffic.
constexpr ran_slice_id_t SRB_RAN_SLICE_ID = ran_slice_id_t{0};

/// RAN slice ID for default slice used to schedule DRB(s) traffic.
constexpr ran_slice_id_t DEFAULT_DRB_RAN_SLICE_ID = ran_slice_id_t{1};

} // namespace ocudu
