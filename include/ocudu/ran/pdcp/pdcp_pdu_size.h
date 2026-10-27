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

// Header length
constexpr size_t PDCP_DATA_PDU_HEADER_SIZE_12BIT = 2;
constexpr size_t PDCP_DATA_PDU_HEADER_SIZE_18BIT = 3;
constexpr size_t PDCP_DATA_PDU_HEADER_SIZE_MAX   = PDCP_DATA_PDU_HEADER_SIZE_18BIT;
constexpr size_t pdcp_data_pdu_header_size(pdcp_sn_size sn_size)
{
  switch (sn_size) {
    case pdcp_sn_size::size12bits:
      return PDCP_DATA_PDU_HEADER_SIZE_12BIT;
    case pdcp_sn_size::size18bits:
      return PDCP_DATA_PDU_HEADER_SIZE_18BIT;
    case ocudu::pdcp_sn_size::invalid:
      break;
  }
  ocudu_assertion_failure("Cannot determine PDCP data PDU header size: unsupported sn_size={}",
                          pdcp_sn_size_to_uint(sn_size));
  return PDCP_DATA_PDU_HEADER_SIZE_12BIT;
}

} // namespace ocudu
