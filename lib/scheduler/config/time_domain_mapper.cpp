// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "time_domain_mapper.h"
#include "ocudu/scheduler/config/pusch_td_resource_indices.h"
#include "ocudu/support/ocudu_assert.h"

using namespace ocudu;

/// \brief Computes the list circularly indexed by slot containing the list of indices into \c pdsch_td_candidates
/// that are applicable PDSCH TD resource candidates for a PDSCH scheduled by a PDCCH in each slot.
static std::vector<std::vector<uint8_t>>
generate_pdsch_td_res_indices_per_tdd_slot(span<const pdsch_time_domain_resource_allocation> pdsch_td_candidates,
                                           const std::optional<tdd_ul_dl_config_common>&     tdd_cfg,
                                           cyclic_prefix                                     cp)
{
  if (not tdd_cfg.has_value()) {
    // In FDD every slot is a full DL slot, so all candidates apply.
    std::vector<uint8_t> all_indices;
    all_indices.reserve(pdsch_td_candidates.size());
    for (unsigned idx = 0, sz = pdsch_td_candidates.size(); idx < sz; ++idx) {
      all_indices.push_back(static_cast<uint8_t>(idx));
    }
    return {all_indices};
  }

  const tdd_ul_dl_config_common&    tdd              = *tdd_cfg;
  const unsigned                    tdd_period_slots = nof_slots_per_tdd_period(tdd);
  std::vector<std::vector<uint8_t>> result(tdd_period_slots);

  for (unsigned slot_idx = 0; slot_idx < tdd_period_slots; ++slot_idx) {
    // A candidate applies if it ends at the last DL symbol of the slot: the number of symbols per slot in a full DL
    // slot, or the last DL symbol in a special slot.
    const uint8_t last_dl_symbol = get_active_tdd_dl_symbols(tdd, slot_idx, cp).stop();
    for (unsigned idx = 0, sz = pdsch_td_candidates.size(); idx < sz; ++idx) {
      if (pdsch_td_candidates[idx].symbols.stop() == last_dl_symbol) {
        result[slot_idx].push_back(static_cast<uint8_t>(idx));
      }
    }
  }

  return result;
}

dl_time_domain_mapper::dl_time_domain_mapper(const dl_time_domain_builder_params& params)
{
  using builder_params = dl_time_domain_builder_params;

  if (const auto* auto_res = std::get_if<builder_params::auto_resources>(&params.params)) {
    // Derive PDSCH TD resources assuming CORESETs start at symbol 0, so PDSCH starts after the CORESET.
    pdsch_td_res_list = time_domain_resource_helper::generate_dedicated_pdsch_td_res_list(
        params.tdd_cfg, params.cp, auto_res->coreset_max_dur);
  } else {
    const auto& explicit_res = std::get<builder_params::explicit_resources>(params.params);
    ocudu_assert(not explicit_res.pdsch_td_res_list.empty(), "Explicit PDSCH TD resource list must not be empty.");
    pdsch_td_res_list = explicit_res.pdsch_td_res_list;
  }

  // Generate the PDSCH TD resource index candidates for a PDSCH scheduled in each slot (handles both FDD and TDD).
  pdsch_td_res_indices_per_slot =
      generate_pdsch_td_res_indices_per_tdd_slot(pdsch_td_res_list, params.tdd_cfg, params.cp);
}

/// \brief Computes the list of valid PUCCH k1 values that can be used for a given DL slot.
///
/// For TDD, the returned vector is circularly indexed by slot within the TDD period (size = TDD period length). For
/// FDD, the vector contains a single entry (list) that applies to every DL slot.
///
/// The valid k1 values per slot are derived as follows:
/// - \b FDD: all values in \c dl_data_to_ul_ack are valid for every slot.
/// - \b TDD UL-heavy (more full-UL slots than DL slots): all values in \c dl_data_to_ul_ack are valid for every DL
///   slot; UL slots and non-DL slots get an empty list.
/// - \b TDD DL-heavy (nof DL slots >= full-UL slots): for each DL slot, only k1 values that are >= the slot's
///   assigned k2 (from \c pusch_td_resource_indices_per_slot) are valid, ensuring that a PUSCH cannot be scheduled
///   before any PUCCH carrying the HARQ-ACK scheduled for the same slot. DL slots with no assigned PUSCH carry the full
///   k1 list. UL slots get an empty list.
///
/// \param[in] dl_data_to_ul_ack   List of candidate k1 values (dl-DataToUL-ACK), in ascending order.
/// \param[in] tdd_cfg_common      TDD UL/DL configuration. If absent, the cell operates in FDD mode.
/// \param[in] pusch_td_alloc_list PUSCH time-domain resource allocation table for the cell.
/// \param[in] pusch_td_resource_indices_per_slot Per-slot list of indices into \c pusch_td_alloc_list that are
/// applicable for PUSCH scheduling in that slot, as produced by \c get_pusch_td_resource_indices_per_slot(). Used in
/// the DL-heavy TDD case to determine the minimum k2 assigned to each DL slot and, hence, the minimum valid k1.
/// \return Vector of valid k1 lists, one entry per slot in the TDD period (or one entry for FDD). Empty inner lists
///   indicate slots for which no PUCCH k1 scheduling is applicable (UL or non-DL slots).
static std::vector<static_vector<uint8_t, pucch_td_helper::MAX_K1_CANDIDATES>>
get_pucch_k1_list_per_slot(span<const uint8_t>                                       dl_data_to_ul_ack,
                           const std::optional<tdd_ul_dl_config_common>&             tdd_cfg_common,
                           const std::vector<pusch_time_domain_resource_allocation>& pusch_td_alloc_list,
                           const std::vector<static_vector<uint8_t, pusch_constants::MAX_NOF_PUSCH_TD_RES_ALLOCS>>&
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

ul_time_domain_mapper::ul_time_domain_mapper(const ul_time_domain_builder_params& params)
{
  using builder_params = ul_time_domain_builder_params;

  // PUSCH.
  if (const auto* auto_res = std::get_if<builder_params::pusch_auto_resources>(&params.pusch_params)) {
    // Derive PUSCH TD resources.
    // - [Implementation-defined] Ensure a k2 value less than or equal to the minimum k1 configured for the BWP exists
    // as the first entry of the list. This way PDSCH(s) are scheduled before PUSCH and all DL slots are filled with
    // PDSCH and all UL slots are filled with PUSCH under heavy load. It also ensures that correct DAI value goes in
    // the UL PDCCH of DCI Format 0_1.
    pusch_td_res_list = time_domain_resource_helper::generate_dedicated_pusch_td_res_list(
        params.tdd_cfg, params.cp, auto_res->min_k2, auto_res->max_srs_symbols, auto_res->symbols_per_srs);
  } else {
    const auto& explicit_res = std::get<builder_params::pusch_explicit_resources>(params.pusch_params);
    ocudu_assert(not explicit_res.pusch_td_res_list.empty(), "Explicit PUSCH TD resource list must not be empty.");
    pusch_td_res_list = explicit_res.pusch_td_res_list;
  }

  // PUCCH / k1.
  // Minimum k1 for the common (fallback) candidate pool; defaults to the full {1,...,8} set.
  if (const auto* auto_res = std::get_if<builder_params::pucch_auto_resources>(&params.pucch_params)) {
    dedicated_k1_list = time_domain_resource_helper::generate_k1_candidates(params.tdd_cfg, auto_res->min_k1);
    min_k1_val        = auto_res->min_k1;
  } else {
    const auto& explicit_res = std::get<builder_params::pucch_explicit_resources>(params.pucch_params);
    ocudu_assert(not explicit_res.k1_candidates.empty(), "Explicit k1 candidate list must not be empty.");
    ocudu_assert(explicit_res.k1_candidates.size() <= pucch_td_helper::MAX_K1_CANDIDATES,
                 "Number of explicit k1 candidates ({}) exceeds the maximum ({}).",
                 explicit_res.k1_candidates.size(),
                 pucch_td_helper::MAX_K1_CANDIDATES);
    dedicated_k1_list.assign(explicit_res.k1_candidates.begin(), explicit_res.k1_candidates.end());
    min_k1_val = *std::min_element(explicit_res.k1_candidates.begin(), explicit_res.k1_candidates.end());
  }

  // Generate the PUSCH TD resource index candidates for each slot (handles both FDD and TDD).
  pusch_td_res_indices_per_slot =
      get_pusch_td_resource_indices_per_slot(params.scs, params.tdd_cfg, pusch_td_res_list, min_k1_val);

  // Generate the dedicated and common k1 candidates valid for a PDSCH transmitted in each slot.
  common_k1_list = pucch_td_helper::get_common_k1_candidates(min_k1_val);
  dedicated_k1_per_slot =
      get_pucch_k1_list_per_slot(dedicated_k1_list, params.tdd_cfg, pusch_td_res_list, pusch_td_res_indices_per_slot);
  common_k1_per_slot =
      get_pucch_k1_list_per_slot(common_k1_list, params.tdd_cfg, pusch_td_res_list, pusch_td_res_indices_per_slot);
}

std::optional<uint8_t> ul_time_domain_mapper::find_pusch_td_res_index(slot_point        pdcch_slot,
                                                                      slot_point        pusch_slot,
                                                                      ofdm_symbol_range usable_symbols,
                                                                      unsigned          ntn_cs_koffset) const
{
  std::optional<uint8_t> best;
  for (uint8_t idx : pusch_td_res_indices(pdcch_slot.count())) {
    const pusch_time_domain_resource_allocation& pusch_td_res = pusch_td_res_list[idx];
    if (pdcch_slot + pusch_td_res.k2 + ntn_cs_koffset != pusch_slot) {
      continue;
    }
    if (not usable_symbols.contains(pusch_td_res.symbols)) {
      continue;
    }
    if (not best.has_value() or pusch_td_res_list[*best].symbols.length() < pusch_td_res.symbols.length()) {
      best = idx;
    }
  }
  return best;
}
