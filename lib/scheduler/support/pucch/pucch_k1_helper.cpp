// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "pucch_k1_helper.h"
#include "ocudu/ran/pusch/pusch_time_domain_resource.h"
#include "ocudu/ran/tdd/tdd_ul_dl_config.h"

using namespace ocudu;

std::vector<static_vector<uint8_t, 8>> ocudu::get_pucch_k1_list_per_slot(
    span<const uint8_t>                                       dl_data_to_ul_ack,
    const std::optional<tdd_ul_dl_config_common>&             tdd_cfg_common,
    const std::vector<pusch_time_domain_resource_allocation>& pusch_td_alloc_list,
    const std::vector<static_vector<unsigned, pusch_constants::MAX_NOF_PUSCH_TD_RES_ALLOCS>>&
        pusch_td_resource_indices_per_slot)
{
  ocudu_assert(not dl_data_to_ul_ack.empty(), "dl_data_to_ul_ack cannot be empty");

  std::vector<static_vector<uint8_t, 8>> pucch_k1_list_per_slot;

  // In FDD, we return a vector with 1 value (i.e, one k1 list) only, which applies to all DL slots.
  if (not tdd_cfg_common.has_value()) {
    pucch_k1_list_per_slot.assign(1, {dl_data_to_ul_ack.begin(), dl_data_to_ul_ack.end()});
    return pucch_k1_list_per_slot;
  }

  const unsigned nof_dl_slots      = nof_dl_slots_per_tdd_period(tdd_cfg_common.value());
  const unsigned nof_full_ul_slots = nof_full_ul_slots_per_tdd_period(tdd_cfg_common.value());
  const unsigned nof_slots         = nof_slots_per_tdd_period(tdd_cfg_common.value());

  pucch_k1_list_per_slot.reserve(nof_slots);

  // TDD UL-heavy: all k1 are considered valid for each DL slot.
  if (nof_dl_slots < nof_full_ul_slots) {
    for (unsigned sl_idx = 0; sl_idx != nof_slots; ++sl_idx) {
      if (has_active_tdd_dl_symbols(tdd_cfg_common.value(), sl_idx)) {
        pucch_k1_list_per_slot.emplace_back(dl_data_to_ul_ack.begin(), dl_data_to_ul_ack.end());
      } else {
        pucch_k1_list_per_slot.emplace_back(static_vector<uint8_t, 8>{});
      }
    }
    return pucch_k1_list_per_slot;
  }

  // TDD DL-heavy.
  const auto* min_k1_it = std::min_element(dl_data_to_ul_ack.begin(),
                                           dl_data_to_ul_ack.end(),
                                           [](const unsigned lhs, const unsigned rhs) { return lhs < rhs; });
  ocudu_sanity_check(min_k1_it != dl_data_to_ul_ack.end(),
                     "The min of a non-empty vector of unsigned cannot not exist");

  for (unsigned sl_idx = 0; sl_idx != nof_slots; ++sl_idx) {
    if (has_active_tdd_dl_symbols(tdd_cfg_common.value(), sl_idx)) {
      // If there is no k2 candidate for this DL slot, then we can use all available k1.
      if (pusch_td_resource_indices_per_slot[sl_idx].empty()) {
        pucch_k1_list_per_slot.emplace_back(
            static_vector<uint8_t, 8>{dl_data_to_ul_ack.begin(), dl_data_to_ul_ack.end()});
        continue;
      }

      // Get k2 for this DL slot.
      ocudu_assert(pusch_td_resource_indices_per_slot[sl_idx].front() < pusch_td_alloc_list.size(),
                   "Index out of bounds");
      const unsigned min_k2 = pusch_td_alloc_list[pusch_td_resource_indices_per_slot[sl_idx].front()].k2;

      // NOTE: dl_data_to_ul_ack contains k1 values sorted in ascending order.
      const auto* slot_min_k1_it = std::find_if(dl_data_to_ul_ack.begin(),
                                                dl_data_to_ul_ack.end(),
                                                [min_k2](const unsigned k1_val) { return k1_val >= min_k2; });
      ocudu_sanity_check(
          slot_min_k1_it != dl_data_to_ul_ack.end(),
          "There must be at a k1 value that is greater than or equal to k2. Check if TDD config is supported");
      pucch_k1_list_per_slot.emplace_back(static_vector<uint8_t, 8>{slot_min_k1_it, dl_data_to_ul_ack.end()});
    }
    // No k1 candidate for UL slots.
    else {
      pucch_k1_list_per_slot.emplace_back(static_vector<uint8_t, 8>{});
    }
  }
  return pucch_k1_list_per_slot;
}
