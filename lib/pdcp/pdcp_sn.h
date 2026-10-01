// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/pdcp/pdcp_sn_size.h"
#include "ocudu/support/ocudu_assert.h"

namespace ocudu {

constexpr uint32_t pdcp_compute_sn(uint32_t count, pdcp_sn_size sn_size)
{
  return count & pdcp_compute_sn_mask(sn_size);
}

constexpr uint32_t pdcp_compute_hfn(uint32_t count, pdcp_sn_size sn_size)
{
  return (count >> pdcp_sn_size_to_uint(sn_size));
}

constexpr uint32_t pdcp_compute_count(uint32_t hfn, uint32_t sn, pdcp_sn_size sn_size)
{
  return (hfn << pdcp_sn_size_to_uint(sn_size)) | sn;
}

constexpr uint32_t pdcp_sn_cardinality(pdcp_sn_size sn_size)
{
  uint16_t sn_size_num = pdcp_sn_size_to_uint(sn_size);
  ocudu_assert(sn_size_num < 32, "Cardinality of sn_size={} exceeds return type 'uint32_t'", sn_size_num);
  return (1 << sn_size_num);
}

} // namespace ocudu
