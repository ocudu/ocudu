// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/ran/ssb/ssb_helper.h"
#include "ocudu/adt/format.h"
#include "ocudu/ran/band_helper.h"
#include "ocudu/ran/frame_types.h"
#include "ocudu/ran/ssb/ssb_mapping.h"

using namespace ocudu;

std::vector<unsigned>
ssb_helper::get_occupied_slot_offsets(const ssb_configuration& ssb_cfg, nr_band band, subcarrier_spacing common_scs)
{
  const ssb_pattern_case pattern_case            = band_helper::get_ssb_pattern(band, ssb_cfg.scs);
  const unsigned         nof_ssb_slots_per_sf    = get_nof_slots_per_subframe(ssb_cfg.scs);
  const unsigned         nof_common_slots_per_sf = get_nof_slots_per_subframe(common_scs);

  std::vector<unsigned> slots;
  for (unsigned ssb_idx = 0, sz = ssb_cfg.ssb_beams.get_L_max(); ssb_idx != sz; ++ssb_idx) {
    if (!ssb_cfg.ssb_beams.is_transmitted(ssb_idx)) {
      continue;
    }
    const unsigned l_first = ssb_get_l_first(pattern_case, static_cast<ssb_id_t>(ssb_idx));
    const unsigned common_slot =
        l_first * nof_common_slots_per_sf / (NOF_OFDM_SYM_PER_SLOT_NORMAL_CP * nof_ssb_slots_per_sf);
    if (std::find(slots.begin(), slots.end(), common_slot) == slots.end()) {
      slots.push_back(common_slot);
    }
  }
  return slots;
}
