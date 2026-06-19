// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/scheduler/config/pusch_td_resource_indices.h"
#include "../../config/ue_configuration.h"
#include "../pucch/pucch_k1_helper.h"
#include "pusch_default_time_allocation.h"
#include "ocudu/ocudulog/logger.h"
#include "ocudu/ocudulog/ocudulog.h"

using namespace ocudu;

using pusch_index_list = static_vector<unsigned, pusch_constants::MAX_NOF_PUSCH_TD_RES_ALLOCS>;

/// Get minimum value for k1 given the common and dedicated configurations.
/// When a search space is provided, its k1 candidates take precedence over the cell-level dl-DataToUL-ACK list.
static unsigned get_min_k1(span<const uint8_t> dl_data_to_ul_ack, const search_space_info* ss_info)
{
  const span<const uint8_t> k1_candidates =
      ss_info != nullptr ? get_k1_candidates(ss_info->get_dl_dci_format(), dl_data_to_ul_ack) : dl_data_to_ul_ack;
  return *std::min_element(k1_candidates.begin(), k1_candidates.end());
}

namespace {

class dl_heavy_td_resources_idx_builder
{
public:
  dl_heavy_td_resources_idx_builder(const tdd_ul_dl_config_common&                            tdd_cfg_common,
                                    const std::vector<pusch_time_domain_resource_allocation>& pusch_td_list,
                                    unsigned                                                  min_k2_) :
    tdd_cfg(tdd_cfg_common),
    pusch_td_alloc_list(pusch_td_list),
    min_k2(min_k2_),
    nof_slot(tdd_cfg.pattern1.dl_ul_tx_period_nof_slots +
             (tdd_cfg.pattern2.has_value() ? tdd_cfg.pattern2.value().dl_ul_tx_period_nof_slots : 0U)),
    dl_min_k2_vec(nof_slot, 0)
  {
    tdd_patterns.emplace_back(tdd_cfg.pattern1);
    if (tdd_cfg.pattern2.has_value()) {
      tdd_patterns.emplace_back(tdd_cfg.pattern2.value());
    }
  }

  // Compute the index of applicable TDD resource for each DL slot.
  std::vector<static_vector<unsigned, pusch_constants::MAX_NOF_PUSCH_TD_RES_ALLOCS>> compute_td_res_indices_per_slot()
  {
    struct pattern_element {
      explicit pattern_element(unsigned elem_ = 0, unsigned pattern_idx_ = 0) :
        element(elem_), ref_pattern_idx(pattern_idx_)
      {
      }
      // This is a general unsigned, that can represent a distance from certain slot or a slot index, depending on where
      // this struct is used.
      unsigned element = 0;
      // NOTE: pattern_idx is the pattern index of the refence slot. For example, a sl_idx which is DL could belong to
      // pattern 0, while the reference slot is an UL slot that belongs to pattern 1.
      unsigned ref_pattern_idx = 0;
    };

    // For each of TDD pattern, find the DL slot that is closest to the TDD pattern's last UL slot and such that
    // k2 >= min k2.
    static_vector<std::optional<pattern_element>, MAX_NOF_TDD_PATTERNS> dl_sl_min_k;
    for (unsigned pattern_idx = 0, p_sz = tdd_patterns.size(); pattern_idx != p_sz; ++pattern_idx) {
      if (tdd_patterns[pattern_idx].nof_ul_slots == 0) {
        dl_sl_min_k.emplace_back(std::nullopt);
        continue;
      }
      const unsigned ul_slot_idx =
          pattern_idx == 0U ? tdd_patterns[pattern_idx].dl_ul_tx_period_nof_slots - 1 : nof_slot - 1;
      const unsigned pdcch_slot = find_closest_viable_dl_slot(ul_slot_idx);
      dl_sl_min_k.emplace_back(pattern_element(pdcch_slot, pattern_idx));
    }

    // For each of the DL slot above, find the distance from the DL slot to last UL slot preceding it.
    static_vector<pattern_element, MAX_NOF_TDD_PATTERNS> dl_to_ul_dist;
    for (auto dl_sl : dl_sl_min_k) {
      if (not dl_sl.has_value()) {
        continue;
      }
      const unsigned ul_sl = get_distance_from_prev_ul_slot(dl_sl->element);
      // NOTE: dl_sl.ref_pattern_idx refers to the UL slot that was computed in the previous for loop and might not
      // correspond to \ref ul_sl's pattern.
      dl_to_ul_dist.emplace_back(ul_sl, dl_sl->ref_pattern_idx);
    }

    // Sort the TDD patterns based on the distance from the DL slot to last UL slot preceding it. The rationale behind
    // this is that the distance in question determines how difficult it is for a TDD pattern to find viable DL slots
    // PDCCHs allocation without the need to jump to the previous TDD pattern. The closest the DL slot to the UL slot
    // preceding it, the more difficult it is to assign PDCCH (DL) slots to the TDD pattern's UL slots.
    std::sort(
        dl_to_ul_dist.begin(), dl_to_ul_dist.end(), [this](const pattern_element& lhs, const pattern_element& rhs) {
          // If the distance to the previous UL slot is the same, then give priority to the TDD pattern with the more UL
          // slots.
          if (lhs.element == rhs.element) {
            return this->tdd_patterns[lhs.ref_pattern_idx].nof_ul_slots >
                   this->tdd_patterns[rhs.ref_pattern_idx].nof_ul_slots;
          }
          return lhs.element < rhs.element;
        });

    // For each TDD pattern, find the closest DL slot that has not been assigned any k2 value.
    for (auto dl_sl : dl_to_ul_dist) {
      for (unsigned ul_sl_cnt = 0U, nof_ul_slots = tdd_patterns[dl_sl.ref_pattern_idx].nof_ul_slots;
           ul_sl_cnt != nof_ul_slots;
           ++ul_sl_cnt) {
        const unsigned ul_slot_idx = dl_sl.ref_pattern_idx == 0U
                                         ? tdd_patterns[dl_sl.ref_pattern_idx].dl_ul_tx_period_nof_slots - ul_sl_cnt - 1
                                         : nof_slot - ul_sl_cnt - 1;
        const unsigned pdcch_slot  = find_closest_viable_dl_slot(ul_slot_idx, true);
        ocudu_assert(pdcch_slot < dl_min_k2_vec.size(), "");
        dl_min_k2_vec[pdcch_slot] = get_k2(ul_slot_idx, pdcch_slot);
      }
    }

    std::vector<static_vector<unsigned, pusch_constants::MAX_NOF_PUSCH_TD_RES_ALLOCS>>
        pusch_td_resource_indices_per_slot(nof_slot);

    for (unsigned sl_idx = 0; sl_idx != nof_slot; ++sl_idx) {
      // Skip slots that were not assigned a PDCCH (0 is used as "not assigned" sentinel; k2 >= 1 is always guaranteed
      // by get_k2() since ul_slot != dl_slot).
      if (dl_min_k2_vec[sl_idx] == 0) {
        continue;
      }
      const unsigned k2 = dl_min_k2_vec[sl_idx];

      for (unsigned td_res_i = 0, sz = pusch_td_alloc_list.size(); td_res_i != sz; ++td_res_i) {
        const auto& td_res = pusch_td_alloc_list[td_res_i];
        if (td_res.k2 == k2) {
          pusch_td_resource_indices_per_slot[sl_idx].emplace_back(td_res_i);
        }
      }
    }

    return pusch_td_resource_indices_per_slot;
  }

private:
  static constexpr unsigned MAX_NOF_TDD_PATTERNS = 2;

  unsigned get_distance_from_prev_ul_slot(unsigned sl_idx) const
  {
    ocudu_assert(has_active_tdd_dl_symbols(tdd_cfg, sl_idx), "This is function must be called for DL slot only");

    if (tdd_patterns.size() == 1) {
      return sl_idx + 1;
    }

    const unsigned dl_sl_patter_idx = sl_idx >= tdd_cfg.pattern1.dl_ul_tx_period_nof_slots ? 1U : 0U;

    if (dl_sl_patter_idx == 0) {
      if (tdd_patterns[(dl_sl_patter_idx + 1) % tdd_patterns.size()].nof_ul_slots != 0U) {
        return sl_idx + 1;
      }
      return tdd_patterns[(dl_sl_patter_idx + 1) % tdd_patterns.size()].dl_ul_tx_period_nof_slots + sl_idx - 1;
    }

    if (tdd_patterns[(dl_sl_patter_idx + 1) % tdd_patterns.size()].nof_ul_slots != 0U) {
      return sl_idx - tdd_patterns[(dl_sl_patter_idx + 1) % tdd_patterns.size()].dl_ul_tx_period_nof_slots + 1;
    }
    return sl_idx + 1;
  }

  unsigned find_closest_viable_dl_slot(unsigned ul_slot, bool check_dl_slot_is_free = false) const
  {
    unsigned dl_slot = ul_slot >= min_k2 ? ul_slot - min_k2 : ul_slot + nof_slot - min_k2;
    ocudu_assert(dl_slot < dl_min_k2_vec.size(), "dl_slot out of range");
    while (not has_active_tdd_dl_symbols(tdd_cfg, dl_slot) or (check_dl_slot_is_free and dl_min_k2_vec[dl_slot] != 0)) {
      if (dl_slot > 0) {
        --dl_slot;
      } else {
        dl_slot = nof_slot + dl_slot - 1;
      }
    }
    return dl_slot;
  }

  unsigned get_k2(unsigned ul_slot, unsigned dl_slot) const
  {
    ocudu_assert(ul_slot != dl_slot, "Not expected");
    unsigned k2 = ul_slot > dl_slot ? ul_slot - dl_slot : ul_slot + nof_slot - dl_slot;
    // With the usage of 2 \ref pattern_element for the computation of DL/UL slots, we can't tell the difference among
    // k2s that differ for nof_slot. Therefore, if the resulting k2 is less than min k2, we need to add nof_slot.
    while (k2 < min_k2) {
      k2 += nof_slot;
    }
    return k2;
  }

  const tdd_ul_dl_config_common&                            tdd_cfg;
  const std::vector<pusch_time_domain_resource_allocation>& pusch_td_alloc_list;
  const unsigned                                            min_k2;
  const unsigned                                            nof_slot;
  static_vector<tdd_ul_dl_pattern, MAX_NOF_TDD_PATTERNS>    tdd_patterns;
  std::vector<unsigned>                                     dl_min_k2_vec;
};
} // anonymous namespace

static span<const pusch_time_domain_resource_allocation>
get_pusch_time_domain_resource_table(const pusch_config_common& pusch_cfg_common, const search_space_info* ss_info)
{
  return ss_info != nullptr ? ss_info->pusch_time_domain_list : pusch_cfg_common.pusch_td_alloc_list;
}

/// Determine PUSCH TD resources for the FDD mode.
static pusch_index_list get_fdd_pusch_td_resource_indices(const pusch_config_common& pusch_cfg_common,
                                                          span<const uint8_t>        dl_data_to_ul_ack,
                                                          const search_space_info*   ss_info)
{
  const unsigned min_k1                 = get_min_k1(dl_data_to_ul_ack, ss_info);
  auto           pusch_time_domain_list = get_pusch_time_domain_resource_table(pusch_cfg_common, ss_info);

  pusch_index_list result;
  for (unsigned i = 0; i != pusch_time_domain_list.size(); ++i) {
    if (pusch_time_domain_list[i].k2 <= min_k1) {
      result.push_back(i);
    }
  }
  return result;
}

static bool is_dl_enabled_slot(slot_point slot, const std::optional<tdd_ul_dl_config_common>& tdd_cfg_common)
{
  if (not tdd_cfg_common.has_value()) {
    return true;
  }

  return has_active_tdd_dl_symbols(tdd_cfg_common.value(), slot.count());
}

pusch_index_list ocudu::get_pusch_td_resource_indices(slot_point                                    pdcch_slot,
                                                      const std::optional<tdd_ul_dl_config_common>& tdd_cfg_common,
                                                      const pusch_config_common&                    pusch_cfg_common,
                                                      span<const uint8_t>                           dl_data_to_ul_ack,
                                                      const search_space_info*                      ss_info,
                                                      bool                                          is_fallback)
{
  if (not tdd_cfg_common.has_value()) {
    // FDD case.
    return get_fdd_pusch_td_resource_indices(pusch_cfg_common, dl_data_to_ul_ack, ss_info);
  }

  // TDD case.
  const unsigned min_k1                 = get_min_k1(dl_data_to_ul_ack, ss_info);
  auto           pusch_time_domain_list = get_pusch_time_domain_resource_table(pusch_cfg_common, ss_info);
  const unsigned nof_full_ul_slots      = nof_full_ul_slots_per_tdd_period(tdd_cfg_common.value());
  const unsigned nof_full_dl_slots      = nof_dl_slots_per_tdd_period(tdd_cfg_common.value());
  const bool     is_dl_heavy            = nof_full_dl_slots >= nof_full_ul_slots;

  pusch_index_list result;
  for (unsigned td_idx = 0; td_idx != pusch_time_domain_list.size(); ++td_idx) {
    const pusch_time_domain_resource_allocation& pusch_td_res = pusch_time_domain_list[td_idx];
    // [Implementation-defined] PUSCH on partial UL slots is not supported.
    if (not is_tdd_full_ul_slot(tdd_cfg_common.value(), (pdcch_slot + pusch_td_res.k2).slot_index())) {
      continue;
    }

    if (is_dl_heavy) {
      // DL-heavy case.
      // [Implementation-defined] For DL heavy TDD configuration, we allow only entries with the same k2 value that are
      // less than or equal to minimum value of k1(s); these multiple entries can have different symbols. This condition
      // the condition pusch_td_res.k2 <= min_k1 prevents allocating a PUSCH before a PUCCH for the same UE on the same
      // slot.
      // NOTE: For the fallback scheduler, this is not needed, as we don't multiplex UCI on PUSCH in that case.
      if (not is_fallback and pusch_td_res.k2 > min_k1) {
        continue;
      }
      // Stop if this entry has a different k2 than what was already collected (all accepted entries must share k2).
      if (not result.empty() and
          std::any_of(result.begin(),
                      result.end(),
                      [candidate_k2 = pusch_td_res.k2, &pusch_time_domain_list](unsigned res_idx) {
                        return candidate_k2 != pusch_time_domain_list[res_idx].k2;
                      })) {
        break;
      }
      result.push_back(td_idx);
    }
    // UL-heavy case.
    else {
      // [Implementation-defined] For UL heavy TDD configuration multiple k2 values are considered for scheduling
      // since it allows multiple UL PDCCH allocations in the same slot for same UE but with different k2 values.
      result.push_back(td_idx);
    }
  }
  return result;
}

static std::vector<pusch_index_list>
get_pusch_td_res_idx_per_slot_full_list(subcarrier_spacing             scs,
                                        const tdd_ul_dl_config_common& tdd_cfg_common,
                                        const pusch_config_common&     pusch_cfg_common,
                                        span<const uint8_t>            dl_data_to_ul_ack,
                                        const search_space_info*       ss_info)
{
  const unsigned nof_slots = nof_slots_per_tdd_period(tdd_cfg_common);

  // List circularly indexed by slot with the list of applicable PUSCH Time Domain resource indexes per slot.
  // NOTE: The list would be empty for UL slots.
  std::vector<pusch_index_list> pusch_td_list_per_slot(nof_slots);
  // Populate the initial list of applicable PUSCH time domain resources per slot.
  for (unsigned slot_idx = 0, e = nof_slots; slot_idx != e; ++slot_idx) {
    slot_point pdcch_slot{to_numerology_value(scs), slot_idx};
    if (is_dl_enabled_slot(pdcch_slot, tdd_cfg_common)) {
      pusch_td_list_per_slot[slot_idx] =
          get_pusch_td_resource_indices(pdcch_slot, tdd_cfg_common, pusch_cfg_common, dl_data_to_ul_ack, ss_info);
    }
  }
  return pusch_td_list_per_slot;
}

static std::optional<unsigned> find_td_index_with_k2(span<const pusch_time_domain_resource_allocation> pusch_res_list,
                                                     span<const unsigned>                              valid_indexes,
                                                     unsigned                                          k2)
{
  const auto* it =
      std::find_if(valid_indexes.begin(), valid_indexes.end(), [&pusch_res_list, k2](unsigned pusch_td_res_idx) {
        return pusch_res_list[pusch_td_res_idx].k2 == k2;
      });
  if (it == valid_indexes.end()) {
    return std::nullopt;
  }
  return *it;
}

std::vector<pusch_index_list>
ocudu::get_fairly_distributed_pusch_td_resource_indices(subcarrier_spacing             scs,
                                                        const tdd_ul_dl_config_common& tdd_cfg_common,
                                                        const pusch_config_common&     pusch_cfg_common,
                                                        span<const uint8_t>            dl_data_to_ul_ack,
                                                        const search_space_info*       ss_info)
{
  // List circularly indexed by slot with the list of applicable PUSCH Time Domain resource indexes per slot.
  // NOTE: The list would be empty for UL slots.
  std::vector<pusch_index_list> initial_pusch_td_list_per_slot =
      get_pusch_td_res_idx_per_slot_full_list(scs, tdd_cfg_common, pusch_cfg_common, dl_data_to_ul_ack, ss_info);

  const unsigned nof_dl_slots      = nof_dl_slots_per_tdd_period(tdd_cfg_common);
  const unsigned nof_full_ul_slots = nof_full_ul_slots_per_tdd_period(tdd_cfg_common);
  const unsigned nof_slots         = nof_slots_per_tdd_period(tdd_cfg_common);

  // Fetch the relevant PUSCH time domain resource list.
  span<const pusch_time_domain_resource_allocation> pusch_time_domain_list =
      get_pusch_time_domain_resource_table(pusch_cfg_common, ss_info);
  ocudu_sanity_check(not pusch_time_domain_list.empty(), "pusch_time_domain_list is expected to be non-empty");
  const unsigned max_k2 = pusch_time_domain_list.back().k2;

  // [Implementation-defined] Fairness is achieved by computing nof. UL PDCCHs to be scheduled per each PDCCH slot.
  // Then, iterating over UL slots finding the nearest PDCCH slot to it such that nof. UL PDCCHs at each PDCCH slot more
  // or less satisfies the earlier computed value.

  // Estimate the nof. UL PDCCHs that can be scheduled in each PDCCH slot.
  const auto nof_ul_pdcchs_per_dl_slot =
      static_cast<unsigned>(std::round(static_cast<double>(nof_full_ul_slots) / static_cast<double>(nof_dl_slots)));

  // List circularly indexed by slot with the list of applicable PUSCH Time Domain resource indexes per slot fairly
  // distributed among all the PDCCH slots.
  // NOTE: The list would be empty for UL slots.
  std::vector<pusch_index_list> final_pusch_td_list_per_slot(nof_slots);

  // Iterate from latest UL slot to earliest and find the closest PDCCH slot to that UL slot.
  // NOTE: There can be scenarios where the closest PDCCH slot may not be able to schedule PUSCH in the chosen UL slot.
  // In this case we move on next closest PDCCH slot, so on and so forth.
  for (unsigned candidate_idx = 0; candidate_idx != nof_slots; ++candidate_idx) {
    unsigned ul_slot_idx = nof_slots - candidate_idx;
    // Skip if it's not a UL slot.
    // TODO: Revisit when scheduling of PUSCH over partial UL slots is supported.
    if (not is_tdd_full_ul_slot(tdd_cfg_common, ul_slot_idx)) {
      continue;
    }
    // Flag indicating whether a valid PDCCH slot for a given UL slot is found or not.
    bool                    no_pdcch_slot_found = true;
    std::optional<unsigned> last_valid_k2;
    for (unsigned k2 = 0; k2 <= max_k2; ++k2) {
      unsigned dl_slot_idx = (nof_slots + ul_slot_idx - k2) % nof_slots;
      // Skip if it's not a DL slot.
      if (not has_active_tdd_dl_symbols(tdd_cfg_common, dl_slot_idx)) {
        continue;
      }
      // Check whether there is a PUSCH time domain resource with required k2 value for the PDCCH slot.
      std::optional<unsigned> idx =
          find_td_index_with_k2(pusch_time_domain_list, initial_pusch_td_list_per_slot[dl_slot_idx], k2);
      if (not idx.has_value()) {
        continue;
      }
      // Store PDCCH slot index at which a valid PUSCH time domain resource was found to schedule PUSCH at given UL
      // slot.
      last_valid_k2 = k2;
      // Skip if nof. PUSCH time domain resource indexes for this PDCCH slot exceed nof. UL PDCCHs that can be scheduled
      // in each PDCCH slot.
      if (final_pusch_td_list_per_slot[dl_slot_idx].size() >= nof_ul_pdcchs_per_dl_slot) {
        // Search for next PDCCH slot.
        continue;
      }
      // Store the nof. PUSCH time domain resource index for this PDCCH slot.
      final_pusch_td_list_per_slot[dl_slot_idx].push_back(*idx);
      no_pdcch_slot_found = false;
      break;
    }

    // Note: This should not happen if the config passed the validation.
    ocudu_sanity_check(
        last_valid_k2.has_value(), "Invalid TDD pattern which leads to UL slot index={} with no valid k2", ul_slot_idx);

    // [Implementation-defined] If no PDCCH slot is found (all candidates are already full), fall back to the last valid
    // PDCCH slot ignoring the per-slot UL PDCCH limit.
    if (no_pdcch_slot_found) {
      const unsigned required_k2      = last_valid_k2.value();
      const unsigned pdcch_slot_index = (ul_slot_idx + nof_slots - required_k2) % nof_slots;

      // If required_k2 is below the minimum k2 in the TD list, the PDCCH slot for this UL slot lies in the previous
      // TDD period. In that case, pick the smallest k2 that exceeds the pattern1 period length.
      std::optional<uint8_t> candidate_required_k2;
      // NOTE min_k2 must have a value, as we ensured above that pusch_time_domain_list is non-emtpy.
      const auto* min_k2 = std::min_element(pusch_time_domain_list.begin(),
                                            pusch_time_domain_list.end(),
                                            [](const auto& a, const auto& b) { return a.k2 < b.k2; });
      if (required_k2 < min_k2->k2) {
        for (const auto& pusch_time_domain : pusch_time_domain_list) {
          if (pusch_time_domain.k2 > tdd_cfg_common.pattern1.dl_ul_tx_period_nof_slots) {
            candidate_required_k2 =
                std::min(candidate_required_k2.value_or(pusch_time_domain.k2), pusch_time_domain.k2);
          }
        }
      } else {
        candidate_required_k2 = required_k2;
      }
      // If a valid PUSCH time domain resource is found for the required k2 value, then we store it.
      std::optional<unsigned> pusch_td_res_idx_for_required_k2 = std::nullopt;
      if (candidate_required_k2.has_value()) {
        auto& init_push_list = initial_pusch_td_list_per_slot[pdcch_slot_index];
        auto* it             = std::find_if(init_push_list.begin(),
                                init_push_list.end(),
                                [&pusch_time_domain_list, candidate_required_k2](unsigned pusch_td_res_idx) {
                                  return pusch_time_domain_list[pusch_td_res_idx].k2 == candidate_required_k2.value();
                                });
        if (it != init_push_list.end()) {
          pusch_td_res_idx_for_required_k2.emplace(*it);
        }
      }
      if (pusch_td_res_idx_for_required_k2.has_value()) {
        final_pusch_td_list_per_slot[pdcch_slot_index].push_back(*pusch_td_res_idx_for_required_k2);
      } else {
        ocudulog::basic_logger& logger = ocudulog::fetch_basic_logger("SCHED", false);
        logger.warning("No valid PUSCH time domain resource found for UL slot idx={}", ul_slot_idx);
      }
    }
  }

  // Sort PUSCH time domain resource indexes (ascending order) in the final list of PUSCH time domain resources
  // maintained per slot.
  for (unsigned slot_idx = 0, e = nof_slots; slot_idx != e; ++slot_idx) {
    if (has_active_tdd_dl_symbols(tdd_cfg_common, slot_idx)) {
      std::sort(final_pusch_td_list_per_slot[slot_idx].begin(), final_pusch_td_list_per_slot[slot_idx].end());
    }
  }

  return final_pusch_td_list_per_slot;
}

std::vector<static_vector<unsigned, pusch_constants::MAX_NOF_PUSCH_TD_RES_ALLOCS>>
ocudu::get_pusch_td_resource_indices_per_slot(subcarrier_spacing                            scs,
                                              const std::optional<tdd_ul_dl_config_common>& tdd_cfg_common,
                                              const pusch_config_common&                    pusch_cfg_common,
                                              span<const uint8_t>                           dl_data_to_ul_ack,
                                              const search_space_info*                      ss_info)
{
  // Note: [Implementation-defined] In case of FDD, we only consider one slot.
  if (not tdd_cfg_common.has_value()) {
    return {get_fdd_pusch_td_resource_indices(pusch_cfg_common, dl_data_to_ul_ack, ss_info)};
  }

  const unsigned nof_dl_slots      = nof_dl_slots_per_tdd_period(tdd_cfg_common.value());
  const unsigned nof_full_ul_slots = nof_full_ul_slots_per_tdd_period(tdd_cfg_common.value());

  // In DL-heavy case, we do not need to proceed further.
  if (nof_dl_slots >= nof_full_ul_slots) {
    const auto k2_it =
        std::min_element(pusch_cfg_common.pusch_td_alloc_list.begin(),
                         pusch_cfg_common.pusch_td_alloc_list.end(),
                         [](const pusch_time_domain_resource_allocation& lhs,
                            const pusch_time_domain_resource_allocation& rhs) { return lhs.k2 < rhs.k2; });
    ocudu_assert(k2_it != pusch_cfg_common.pusch_td_alloc_list.end(), "min k2 must exist");
    dl_heavy_td_resources_idx_builder dl_hv_builder(
        tdd_cfg_common.value(), pusch_cfg_common.pusch_td_alloc_list, k2_it->k2);
    return dl_hv_builder.compute_td_res_indices_per_slot();
  }

  // UL-heavy case
  return get_fairly_distributed_pusch_td_resource_indices(
      scs, tdd_cfg_common.value(), pusch_cfg_common, dl_data_to_ul_ack, ss_info);
}
