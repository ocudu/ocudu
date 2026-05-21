// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/ran/sib/sib_helper.h"
#include "ocudu/ran/band_helper.h"
#include "ocudu/ran/pdcch/pdcch_type0_css_coreset_config.h"
#include "ocudu/ran/pdcch/pdcch_type0_css_occasions.h"
#include "ocudu/ran/ssb/ssb_helper.h"
#include "ocudu/support/enum_utils.h"
#include <algorithm>

using namespace ocudu;

sib_helper::sib1_sched_occations sib_helper::get_occupied_slot_offsets(const ssb_configuration& ssb_cfg,
                                                                       nr_band                  band,
                                                                       subcarrier_spacing       scs_common,
                                                                       search_space0_index      ss0_idx,
                                                                       uint8_t                  coreset0_idx,
                                                                       sib1_rtx_periodicity     min_sib1_retx_period)
{
  const bool is_fr2 = band_helper::get_freq_range(band) == frequency_range::FR2;

  const pdcch_type0_css_coreset_description coreset0_params =
      pdcch_type0_css_coreset_get(band, ssb_cfg.scs, scs_common, coreset0_idx, ssb_cfg.k_ssb.value());

  const unsigned ssb_period_ms =
      std::max<unsigned>(ssb_periodicity_to_value(ssb_cfg.ssb_period), to_value(min_sib1_retx_period));

  // For patterns 2 and 3 (FR2), CORESET0 lies within the same slot as the SS/PBCH block, so the Type0-CSS monitoring
  // slots are exactly the SSB slots. Return those directly; the window period is the SSB period.
  if (coreset0_params.pattern != ssb_coreset0_mplex_pattern::mplx_pattern1) {
    return {ssb_period_ms * get_nof_slots_per_subframe(scs_common),
            ssb_helper::get_occupied_slot_offsets(ssb_cfg, band, scs_common)};
  }

  // For pattern 1, the Type0-CSS monitoring window is max(20ms, SSB period) per TS 38.213 Section 13.
  const unsigned window_period_slots = std::max(20u, ssb_period_ms) * get_nof_slots_per_subframe(scs_common);

  const pdcch_type0_css_occasion_pattern1_description occ = pdcch_type0_css_occasions_get_pattern1(
      {.is_fr2 = is_fr2, .ss0_index = ss0_idx, .nof_symb_coreset = coreset0_params.nof_symb_coreset});

  sib1_sched_occations result;
  result.window_period_slots = window_period_slots;
  for (unsigned ssb_idx = 0; ssb_idx < ssb_cfg.ssb_bitmap.size(); ++ssb_idx) {
    if (!ssb_cfg.ssb_bitmap.test(ssb_idx)) {
      continue;
    }
    // SIB1 is scheduled in slot n0+1 of the Type0-CSS window.
    const unsigned sib1_slot = (get_type0_pdcch_css_n0(occ.offset, occ.M, scs_common, ssb_idx) + 1).count();
    if (std::find(result.slot_offsets.begin(), result.slot_offsets.end(), sib1_slot) == result.slot_offsets.end()) {
      result.slot_offsets.push_back(sib1_slot);
    }
  }
  return result;
}
