// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/ran/pdsch/pdsch_dmrs_symbol_mask.h"

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
