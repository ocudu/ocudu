// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/scheduler/config/pucch_default_resource.h"
#include "ocudu/support/math/math_utils.h"

using namespace ocudu;

unsigned ocudu::get_pucch_default_resource_index(unsigned n_cce, unsigned nof_cce, unsigned delta_pri)
{
  return (2 * n_cce) / nof_cce + 2 * delta_pri;
}

std::pair<unsigned, unsigned>
ocudu::get_pucch_default_prb_index(unsigned r_pucch, unsigned rb_bwp_offset, unsigned nof_cs, unsigned N_bwp_size)
{
  ocudu_assert(r_pucch < 16, "The PUCCH resource index {} exceeds the maximum allowed {}.", r_pucch, 16);

  unsigned prb_index_first  = rb_bwp_offset + (r_pucch / nof_cs);
  unsigned prb_index_second = N_bwp_size - 1 - prb_index_first;
  if (r_pucch >= 8) {
    prb_index_second = rb_bwp_offset + ((r_pucch - 8) / nof_cs);
    prb_index_first  = N_bwp_size - 1 - prb_index_second;
  }

  return {prb_index_first, prb_index_second};
}

unsigned ocudu::get_pucch_default_cyclic_shift(unsigned r_pucch, unsigned nof_cs)
{
  ocudu_assert(r_pucch < 16, "The PUCCH resource index {} exceeds the maximum allowed {}.", r_pucch, 16);
  ocudu_assert(nof_cs > 0, "The number of cyclic shift must be greater than zero.");
  unsigned result = r_pucch % nof_cs;
  if (r_pucch >= 8) {
    result = (r_pucch - 8) % nof_cs;
  }
  return result;
}

/// TS38.213 Table 9.2.1-1.
static const std::array<pucch_default_resource, 16> resource_table = {
    {{pucch_format::FORMAT_0, 12, 2, 0, {0, 3}},
     {pucch_format::FORMAT_0, 12, 2, 0, {0, 4, 8}},
     {pucch_format::FORMAT_0, 12, 2, 3, {0, 4, 8}},
     {pucch_format::FORMAT_1, 10, 4, 0, {0, 6}},
     {pucch_format::FORMAT_1, 10, 4, 0, {0, 3, 6, 9}},
     {pucch_format::FORMAT_1, 10, 4, 2, {0, 3, 6, 9}},
     {pucch_format::FORMAT_1, 10, 4, 4, {0, 3, 6, 9}},
     {pucch_format::FORMAT_1, 4, 10, 0, {0, 6}},
     {pucch_format::FORMAT_1, 4, 10, 0, {0, 3, 6, 9}},
     {pucch_format::FORMAT_1, 4, 10, 2, {0, 3, 6, 9}},
     {pucch_format::FORMAT_1, 4, 10, 4, {0, 3, 6, 9}},
     {pucch_format::FORMAT_1, 0, 14, 0, {0, 6}},
     {pucch_format::FORMAT_1, 0, 14, 0, {0, 3, 6, 9}},
     {pucch_format::FORMAT_1, 0, 14, 2, {0, 3, 6, 9}},
     {pucch_format::FORMAT_1, 0, 14, 4, {0, 3, 6, 9}},
     {pucch_format::FORMAT_1, 0, 14, 0, {0, 3, 6, 9}}}};

pucch_default_resource ocudu::get_pucch_default_resource(unsigned index, unsigned N_bwp_size)
{
  ocudu_assert(index < resource_table.size(),
               "PUCCH resource index {} exceeds the number of elements {}.",
               index,
               resource_table.size());
  pucch_default_resource result = resource_table[index];

  // Handle PRB offset for index 15.
  if (index == 15) {
    result.rb_bwp_offset = N_bwp_size / 4;
  }

  return result;
}

unsigned ocudu::get_pucch_default_nof_edge_prbs(unsigned row_index, unsigned N_bwp_size)
{
  // Row 15 places the common resources at N_bwp/4 from the edges, leaving the edges free. The DU cell config validator
  // rejects the dedicated resources that reach them.
  if (row_index == 15) {
    return 0;
  }
  const pucch_default_resource res    = get_pucch_default_resource(row_index, N_bwp_size);
  const unsigned               nof_cs = res.cs_indexes.size();
  // Resources r_PUCCH = 0..7 hop from the low edge, and 8..15 mirror them from the high edge.
  static constexpr unsigned nof_res_per_edge = 8;
  return res.rb_bwp_offset + divide_ceil(nof_res_per_edge, nof_cs);
}
