// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "time_domain_mapper.h"
#include "ocudu/support/ocudu_assert.h"

using namespace ocudu;

static std::vector<std::vector<pdsch_time_domain_resource_allocation>>
generate_pdsch_td_res_per_tdd_slot(span<const pdsch_time_domain_resource_allocation> pdsch_td_candidates,
                                   const std::optional<tdd_ul_dl_config_common>&     tdd_cfg,
                                   cyclic_prefix                                     cp)
{
  if (not tdd_cfg.has_value()) {
    // In FDD every slot is a full DL slot, so all candidates apply.
    return {std::vector<pdsch_time_domain_resource_allocation>(pdsch_td_candidates.begin(), pdsch_td_candidates.end())};
  }

  const tdd_ul_dl_config_common&                                  tdd              = *tdd_cfg;
  const unsigned                                                  tdd_period_slots = nof_slots_per_tdd_period(tdd);
  std::vector<std::vector<pdsch_time_domain_resource_allocation>> result(tdd_period_slots);

  for (unsigned slot_idx = 0; slot_idx < tdd_period_slots; ++slot_idx) {
    // A candidate applies if it ends at the last DL symbol of the slot: the number of symbols per slot in a full DL
    // slot, or the last DL symbol in a special slot.
    const uint8_t last_dl_symbol = get_active_tdd_dl_symbols(tdd, slot_idx, cp).stop();
    for (const auto& candidate : pdsch_td_candidates) {
      if (candidate.symbols.stop() == last_dl_symbol) {
        result[slot_idx].push_back(candidate);
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

  // Generate the PDSCH candidates for a PDSCH scheduled in each slot (handles both FDD and TDD).
  pdsch_td_res_per_slot = generate_pdsch_td_res_per_tdd_slot(pdsch_td_res_list, params.tdd_cfg, params.cp);
}

static std::vector<std::vector<pusch_time_domain_resource_allocation>>
generate_pusch_td_res_per_tdd_slot(span<const pusch_time_domain_resource_allocation> pusch_td_candidates,
                                   const std::optional<tdd_ul_dl_config_common>&     tdd_cfg)
{
  if (not tdd_cfg.has_value()) {
    // In FDD every slot can carry PUSCH, so all candidates apply.
    return {std::vector<pusch_time_domain_resource_allocation>(pusch_td_candidates.begin(), pusch_td_candidates.end())};
  }

  const unsigned                                                  tdd_period_slots = nof_slots_per_tdd_period(*tdd_cfg);
  std::vector<std::vector<pusch_time_domain_resource_allocation>> result(tdd_period_slots);

  for (unsigned slot_idx = 0; slot_idx < tdd_period_slots; ++slot_idx) {
    if (not has_active_tdd_dl_symbols(*tdd_cfg, slot_idx)) {
      // It is not a DL slot, so no PDCCH is available to schedule PUSCH.
      continue;
    }
    // Check which candidates apply.
    for (unsigned idx = 0, sz = pusch_td_candidates.size(); idx < sz; ++idx) {
      const auto& candidate = pusch_td_candidates[idx];
      if (is_tdd_full_ul_slot(*tdd_cfg, slot_idx + candidate.k2)) {
        result[slot_idx].push_back(candidate);
      }
    }
  }

  return result;
}

/// \brief Computes the list of valid PUCCH k1 values that can be used for each PDSCH slot.
///
/// For TDD, the returned vector is circularly indexed by slot within the TDD period (size = TDD period length).
/// For FDD, the vector contains a single entry (list) that applies to every PDSCH slot.
///
/// \param[in] k1_candidates List of candidate k1 values (dl-DataToUL-ACK), in ascending order.
/// \param[in] tdd_cfg       TDD UL/DL configuration. If absent, the cell operates in FDD mode.
static std::vector<static_vector<uint8_t, pucch_td_helper::MAX_K1_CANDIDATES>>
generate_k1_candidates_per_tdd_slot(span<const uint8_t>                           k1_candidates,
                                    const std::optional<tdd_ul_dl_config_common>& tdd_cfg)
{
  if (not tdd_cfg.has_value()) {
    // In FDD every slot is UL, so all candidates are valid.
    return {static_vector<uint8_t, pucch_td_helper::MAX_K1_CANDIDATES>(k1_candidates.begin(), k1_candidates.end())};
  }

  const tdd_ul_dl_config_common& tdd              = *tdd_cfg;
  const unsigned                 tdd_period_slots = nof_slots_per_tdd_period(tdd);
  std::vector<static_vector<uint8_t, pucch_td_helper::MAX_K1_CANDIDATES>> result(tdd_period_slots);

  for (unsigned slot_idx = 0; slot_idx < tdd_period_slots; ++slot_idx) {
    if (not has_active_tdd_dl_symbols(tdd, slot_idx)) {
      // It is not a DL slot, so no PDSCH can be transmitted in this slot.
      continue;
    }
    // A k1 candidate applies if the HARQ-ACK it points to lands on a full UL slot.
    for (uint8_t k1 : k1_candidates) {
      if (is_tdd_full_ul_slot(tdd, slot_idx + k1)) {
        result[slot_idx].push_back(k1);
      }
    }
  }

  return result;
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

  // Generate the PUSCH candidates for each slot (handles both FDD and TDD).
  pusch_td_res_per_slot = generate_pusch_td_res_per_tdd_slot(pusch_td_res_list, params.tdd_cfg);

  // PUCCH / k1.
  // Minimum k1 for the common (fallback) candidate pool; defaults to the full {1,...,8} set.
  uint8_t common_min_k1 = 1;
  if (const auto* auto_res = std::get_if<builder_params::pucch_auto_resources>(&params.pucch_params)) {
    dedicated_k1_list = time_domain_resource_helper::generate_k1_candidates(params.tdd_cfg, auto_res->min_k1);
    common_min_k1     = auto_res->min_k1;
  } else {
    const auto& explicit_res = std::get<builder_params::pucch_explicit_resources>(params.pucch_params);
    ocudu_assert(not explicit_res.k1_candidates.empty(), "Explicit k1 candidate list must not be empty.");
    ocudu_assert(explicit_res.k1_candidates.size() <= pucch_td_helper::MAX_K1_CANDIDATES,
                 "Number of explicit k1 candidates ({}) exceeds the maximum ({}).",
                 explicit_res.k1_candidates.size(),
                 pucch_td_helper::MAX_K1_CANDIDATES);
    dedicated_k1_list.assign(explicit_res.k1_candidates.begin(), explicit_res.k1_candidates.end());
  }

  // Generate the dedicated and common k1 candidates valid for a PDSCH transmitted in each slot.
  common_k1_list        = pucch_td_helper::get_common_k1_candidates(common_min_k1);
  dedicated_k1_per_slot = generate_k1_candidates_per_tdd_slot(dedicated_k1_list, params.tdd_cfg);
  common_k1_per_slot    = generate_k1_candidates_per_tdd_slot(common_k1_list, params.tdd_cfg);
}
