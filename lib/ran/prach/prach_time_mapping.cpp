// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/ran/prach/prach_time_mapping.h"
#include "ocudu/adt/format.h"
#include "ocudu/ran/band_helper.h"
#include "ocudu/ran/prach/prach_configuration.h"
#include "ocudu/ran/prach/prach_preamble_information.h"

using namespace ocudu;
using namespace prach_helper;

preamble_slot_mapping::preamble_slot_mapping(nr_band band, subcarrier_spacing ul_scs, uint16_t prach_config_index)
{
  // Obtain the PRACH configuration.
  const prach_configuration prach_cfg = prach_configuration_get(
      band_helper::get_freq_range(band), band_helper::get_duplex_mode(band), prach_config_index);
  const bool is_fr2 = band_helper::get_freq_range(band) == frequency_range::FR2;

  // Determine whether it is a long preamble.
  use_long_preamble = is_long_preamble(prach_cfg.format);

  // With SCS 15kHz and 30kHz, only normal CP is supported.
  static constexpr unsigned nof_symbols_per_slot = NOF_OFDM_SYM_PER_SLOT_NORMAL_CP;

  // Determine reference subcarrier spacing for slot numerology as per TS 38.211 Section 6.3.3.2.
  const subcarrier_spacing scs_ref = is_fr2 ? subcarrier_spacing::kHz60 : subcarrier_spacing::kHz15;

  // Calculate ratio between the common SCS and the reference SCS.
  const unsigned scs_ratio = pow2(to_numerology_value(ul_scs) - to_numerology_value(scs_ref));

  // Convert list of the SFN occasions into the SFN occasions bitset.
  sfn_occasions.resize(prach_cfg.x);
  for (unsigned y : prach_cfg.y) {
    sfn_occasions.set(y);
  }

  // Convert the list of PRACH slots in the reference subcarrier spacing occasions to a set in the common subcarrier
  // spacing slots.
  slot_occasions.resize(NOF_SUBFRAMES_PER_FRAME * get_nof_slots_per_subframe(ul_scs));
  for (const unsigned ref_slot : prach_cfg.slots) {
    // The slot numbering must be converted from reference SCS to common SCS.
    unsigned common_scs_slot = scs_ratio * ref_slot;
    if (use_long_preamble) {
      // In the case of long preamble, the starting position of the PRACH is referenced to SCS 15 kHz. It is converted
      // to common SCS since it could fall into a different slot.
      const unsigned common_starting_symbol      = scs_ratio * prach_cfg.starting_symbol;
      const unsigned slot_offset_starting_symbol = common_starting_symbol / nof_symbols_per_slot;
      common_scs_slot += slot_offset_starting_symbol;
      // In the case of long preamble, the PRACH occasion might span multiple subframes or slots. For convenience, only
      // the starting slot is stored in the bitset.
      slot_occasions.set(common_scs_slot, true);
    } else {
      // Adjust the slot according to TS 38.211 Section 5.3.2.
      if (scs_ratio == 1) {
        slot_occasions.set(common_scs_slot, true);
      } else if (scs_ratio == 2 && prach_cfg.nof_prach_slots_within_subframe == 1) {
        slot_occasions.set(common_scs_slot + 1, true);
      } else {
        slot_occasions.set(common_scs_slot, true);
        slot_occasions.set(common_scs_slot + 1, true);
      }
    }
  }

  // Derive PRACH duration information.
  const prach_symbols_slots_duration prach_duration_info = get_prach_duration_info(prach_cfg, ul_scs);
  prach_length_slots                                     = prach_duration_info.prach_length_slots;
}
