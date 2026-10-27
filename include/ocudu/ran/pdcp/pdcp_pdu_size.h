// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/pdcp/pdcp_sn_size.h"
#include "ocudu/support/ocudu_assert.h"

namespace ocudu {

/// Maximum supported size of a PDCP SDU
/// Ref: TS 38.323 Sec. 4.3.1
constexpr size_t PDCP_SDU_MAX_SIZE = 9000;

/// Maximum supported size of a PDCP Control PDU
/// Ref: TS 38.323 Sec. 4.3.1
constexpr size_t PDCP_CONTROL_PDU_MAX_SIZE = 9000;

} // namespace ocudu
