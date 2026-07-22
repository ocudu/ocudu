// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/scheduler/config/pucch_guardbands.h"
#include "ocudu/ran/pucch/pucch_configuration.h"
#include "ocudu/ran/pucch/pucch_constants.h"
#include "ocudu/scheduler/config/pucch_default_resource.h"
#include "ocudu/support/ocudu_assert.h"

using namespace ocudu;

crb_bitmap ocudu::compute_pucch_crbs(crb_interval               ul_bwp_crbs,
                                     unsigned                   pucch_res_common,
                                     span<const pucch_resource> ded_pucch_resources)
{
  // Get the parameter N_bwp_size, which is the Initial UL BWP size in PRBs, as per TS 38.213, Section 9.2.1.
  const unsigned size_ul_bwp = ul_bwp_crbs.length();

  crb_bitmap pucch_crbs(size_ul_bwp);

  // Get PUCCH common resource config from Table 9.2.1-1, TS 38.213.
  pucch_default_resource common_default_res = get_pucch_default_resource(pucch_res_common, size_ul_bwp);

  // Fill the CRB bitmap with the PRBs used by the common PUCCH resources.
  for (unsigned r_pucch = 0; r_pucch != pucch_constants::MAX_NOF_CELL_COMMON_PUCCH_RESOURCES; ++r_pucch) {
    auto prbs = get_pucch_default_prb_index(
        r_pucch, common_default_res.rb_bwp_offset, common_default_res.cs_indexes.size(), size_ul_bwp);

    pucch_crbs.fill(prbs.first, prbs.first + pucch_constants::f0::NOF_RBS);
    pucch_crbs.fill(prbs.second, prbs.second + pucch_constants::f0::NOF_RBS);
  }

  // Fill the CRB bitmap with the PRBs used by the dedicated PUCCH resources.
  for (const auto& res : ded_pucch_resources) {
    const crb_interval crbs1 = prb_to_crb(ul_bwp_crbs, res.prbs());
    pucch_crbs.fill(crbs1.start(), crbs1.stop());

    if (res.second_hop_prb.has_value()) {
      const prb_interval prbs2 = prb_interval::start_and_len(*res.second_hop_prb, res.prbs().length());
      const crb_interval crbs2 = prb_to_crb(ul_bwp_crbs, prbs2);
      pucch_crbs.fill(crbs2.start(), crbs2.stop());
    }
  }

  return pucch_crbs;
}

crb_interval ocudu::compute_srs_available_crbs(crb_interval ul_bwp_crbs, unsigned pucch_res_common)
{
  // Common PUCCH resources occupy 2 blocks of RBs, one at the low edge and one at the high edge of the UL BWP; the
  // SRS bandwidth is automatically confined to the gap in between these 2 blocks.
  const crb_bitmap pucch_crbs          = compute_pucch_crbs(ul_bwp_crbs, pucch_res_common, {});
  const unsigned   nof_rbs             = pucch_crbs.size();
  const int        low_edge_last_crb   = pucch_crbs.find_highest(0, nof_rbs / 2, true);
  const int        high_edge_first_crb = pucch_crbs.find_lowest(nof_rbs / 2, nof_rbs, true);
  ocudu_sanity_check(low_edge_last_crb >= 0 and high_edge_first_crb >= 0,
                     "The common PUCCH resource is expected to occupy both edges of the UL BWP");

  const unsigned gap_start = static_cast<unsigned>(low_edge_last_crb) + 1;
  const unsigned gap_stop  = static_cast<unsigned>(high_edge_first_crb);
  ocudu_sanity_check(gap_stop > gap_start, "No RBs available for SRS in between the common PUCCH resource blocks");

  return crb_interval{ul_bwp_crbs.start() + gap_start, ul_bwp_crbs.start() + gap_stop};
}
