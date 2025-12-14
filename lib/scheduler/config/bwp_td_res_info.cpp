// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "bwp_td_res_info.h"
#include <array>

using namespace ocudu;

static std::vector<std::vector<pusch_time_domain_resource_allocation>>
generate_all_k2_candidates_per_tdd_slot(span<const pusch_time_domain_resource_allocation> pusch_td_candidates,
                                        const tdd_ul_dl_config_common&                    tdd_cfg)
{
  const unsigned                                                  tdd_period_slots = nof_slots_per_tdd_period(tdd_cfg);
  std::vector<std::vector<pusch_time_domain_resource_allocation>> result(tdd_period_slots);

  for (unsigned slot_idx = 0; slot_idx < tdd_period_slots; ++slot_idx) {
    if (not has_active_tdd_dl_symbols(tdd_cfg, slot_idx)) {
      // It is not a DL slot, so no PDCCH is available to schedule PUSCH.
      continue;
    }
    // Check which candidates apply.
    for (unsigned idx = 0, sz = pusch_td_candidates.size(); idx < sz; ++idx) {
      const auto& candidate = pusch_td_candidates[idx];
      if (is_tdd_full_ul_slot(tdd_cfg, slot_idx + candidate.k2)) {
        result[slot_idx].push_back(candidate);
      }
    }
  }

  return result;
}

static std::vector<static_vector<uint8_t, time_domain_resource_helper::MAX_K1_CANDIDATES>>
generate_k1_candidates_per_tdd_slot(span<const uint8_t> k1_candidates, const tdd_ul_dl_config_common& tdd_cfg)
{
  const unsigned tdd_period_slots = nof_slots_per_tdd_period(tdd_cfg);
  std::vector<static_vector<uint8_t, time_domain_resource_helper::MAX_K1_CANDIDATES>> result(tdd_period_slots);

  for (unsigned slot_idx = 0; slot_idx < tdd_period_slots; ++slot_idx) {
    if (not has_active_tdd_dl_symbols(tdd_cfg, slot_idx)) {
      // It is not a DL slot, so no PDSCH can be transmitted in this slot.
      continue;
    }
    // A k1 candidate applies if the HARQ-ACK it points to lands on a full UL slot.
    for (uint8_t k1 : k1_candidates) {
      if (is_tdd_full_ul_slot(tdd_cfg, slot_idx + k1)) {
        result[slot_idx].push_back(k1);
      }
    }
  }

  return result;
}

// Compute HARQ-ACK timing values available for DCI format 1_0, considering the min_k1, and as per TS38.213, 9.1.2.1.
static span<const uint8_t> common_k1_candidate_pool(uint8_t min_k1)
{
  static std::array<uint8_t, time_domain_resource_helper::MAX_K1_CANDIDATES> pool = {1, 2, 3, 4, 5, 6, 7, 8};
  return span<const uint8_t>(pool.data() + (min_k1 - 1), pool.size() - (min_k1 - 1));
}

bwp_td_res_info::bwp_td_res_info(const bwp_td_res_info_builder_params& params)
{
  // Derive PDSCH TD resources assuming CORESETs start at symbol 0, so PDSCH starts after the CORESET.
  pdsch_td_res_list = time_domain_resource_helper::generate_dedicated_pdsch_td_res_list(
      params.tdd_cfg, params.cp, params.coreset_max_dur);

  if (not params.tdd_cfg.has_value()) {
    // In FDD mode, only one k1 and k2 value exist.
    dedicated_k1_candidates.push_back(params.min_k1);
    pusch_td_res_list     = {pusch_time_domain_resource_allocation{
        params.min_k2, sch_mapping_type::typeA, ofdm_symbol_range{0, NOF_OFDM_SYM_PER_SLOT_NORMAL_CP}}};
    pusch_td_res_per_slot = std::vector<std::vector<pusch_time_domain_resource_allocation>>{1, pusch_td_res_list};
    dedicated_k1_per_slot = {dedicated_k1_candidates};
    // In FDD every slot is UL, so all common candidates are valid.
    auto common_k1s = common_k1_candidate_pool(params.min_k1);
    common_k1_per_slot.resize(1);
    common_k1_per_slot[0].assign(common_k1s.begin(), common_k1s.end());
    return;
  }
  // It is TDD mode.

  dedicated_k1_candidates = time_domain_resource_helper::generate_k1_candidates(*params.tdd_cfg, params.min_k1);

  // - [Implementation-defined] Ensure k2 value which is less than or equal to minimum value of k1(s) exist in the
  // first entry of list. This way PDSCH(s) are scheduled before PUSCH and all DL slots are filled with PDSCH and
  // all UL slots are filled with PUSCH under heavy load. It also ensures that correct DAI value goes in the UL
  // PDCCH of DCI Format 0_1.
  pusch_td_res_list =
      time_domain_resource_helper::generate_dedicated_pusch_td_res_list(*params.tdd_cfg, params.cp, params.min_k2);

  // Generate PUSCH candidates for each slot within a TDD period.
  pusch_td_res_per_slot = generate_all_k2_candidates_per_tdd_slot(pusch_td_res_list, *params.tdd_cfg);

  // Generate the k1 candidates valid for a PDSCH transmitted in each slot within a TDD period.
  dedicated_k1_per_slot = generate_k1_candidates_per_tdd_slot(dedicated_k1_candidates, *params.tdd_cfg);
  common_k1_per_slot    = generate_k1_candidates_per_tdd_slot(common_k1_candidate_pool(params.min_k1), *params.tdd_cfg);
}
