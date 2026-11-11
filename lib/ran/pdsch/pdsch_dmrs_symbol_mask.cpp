// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/ran/pdsch/pdsch_dmrs_symbol_mask.h"
#include "ocudu/support/ocudu_assert.h"
#include <array>

using namespace ocudu;

dmrs_symbol_mask ocudu::pdsch_dmrs_symbol_mask_mapping_type_A_single_get(
    const pdsch_dmrs_symbol_mask_mapping_type_A_single_configuration& config)
{
  unsigned l0 = static_cast<unsigned>(config.typeA_pos);
  unsigned l1 =
      (config.lte_crs_match_around && (config.additional_position == dmrs_additional_positions::pos1 && l0 == 3) &&
       config.ue_capable_additional_dmrs_dl_alt)
          ? 12
          : 11;

  dmrs_symbol_mask mask(14);
  mask.set(l0);

  if (config.last_symbol < 8 || config.additional_position == dmrs_additional_positions::pos0) {
    return mask;
  }

  if (config.last_symbol < 10) {
    mask.set(7);
    return mask;
  }

  if (config.additional_position == dmrs_additional_positions::pos1) {
    if (config.last_symbol < 13) {
      mask.set(9);
      return mask;
    }
    mask.set(l1);
    return mask;
  }

  if (config.additional_position == dmrs_additional_positions::pos2) {
    if (config.last_symbol < 13) {
      mask.set(6);
      mask.set(9);
      return mask;
    }

    mask.set(7);
    mask.set(11);
    return mask;
  }

  if (config.last_symbol < 12) {
    mask.set(6);
    mask.set(9);
    return mask;
  }

  mask.set(5);
  mask.set(8);
  mask.set(11);

  return mask;
}

dmrs_symbol_mask ocudu::pdsch_dmrs_symbol_mask_mapping_type_B_single_get(
    const pdsch_dmrs_symbol_mask_mapping_type_B_single_configuration& config)
{
  // TS 38.211 Table 7.4.1.1.2-3, columns for PDSCH mapping type B: DMRS positions relative to the first symbol of the
  // allocation, indexed by duration (2 to 13 symbols) and by dmrs-AdditionalPosition (pos0 to pos3). Unused entries
  // are zero; position 0 (l0 itself) is always present.
  static constexpr unsigned                                                         nof_durations = 12;
  static constexpr std::array<std::array<std::array<uint8_t, 4>, 4>, nof_durations> positions     = {{
      // l_d = 2, 3, 4: l0 only.
      {{{0}, {0}, {0}, {0}}},
      {{{0}, {0}, {0}, {0}}},
      {{{0}, {0}, {0}, {0}}},
      // l_d = 5, 6, 7.
      {{{0}, {0, 4}, {0, 4}, {0, 4}}},
      {{{0}, {0, 4}, {0, 4}, {0, 4}}},
      {{{0}, {0, 4}, {0, 4}, {0, 4}}},
      // l_d = 8.
      {{{0}, {0, 6}, {0, 3, 6}, {0, 3, 6}}},
      // l_d = 9, 10.
      {{{0}, {0, 7}, {0, 4, 7}, {0, 4, 7}}},
      {{{0}, {0, 7}, {0, 4, 7}, {0, 4, 7}}},
      // l_d = 11.
      {{{0}, {0, 8}, {0, 4, 8}, {0, 3, 6, 9}}},
      // l_d = 12, 13.
      {{{0}, {0, 9}, {0, 5, 9}, {0, 3, 6, 9}}},
      {{{0}, {0, 9}, {0, 5, 9}, {0, 3, 6, 9}}},
  }};

  // For mapping type B, l0 is the first symbol of the allocation and the table positions are relative to it.
  const unsigned l0       = config.start_symbol.value();
  const unsigned duration = config.duration.value();
  ocudu_assert(
      l0 + duration <= 14, "The PDSCH allocation (start symbol {}, duration {}) exceeds the slot.", l0, duration);

  dmrs_symbol_mask mask(14);
  mask.set(l0);
  for (uint8_t pos : positions[duration - 2][static_cast<unsigned>(config.additional_position)]) {
    if (pos != 0) {
      mask.set(l0 + pos);
    }
  }
  return mask;
}
