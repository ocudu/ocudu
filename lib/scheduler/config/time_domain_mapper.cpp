// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "time_domain_mapper.h"
#include "../support/pucch/pucch_k1_helper.h"
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

  // Generate the PUSCH TD resource index candidates for each slot (handles both FDD and TDD).
  pusch_td_res_indices_per_slot =
      get_pusch_td_resource_indices_per_slot(params.scs, params.tdd_cfg, pusch_td_res_list, dedicated_k1_list.front());

  // Generate the dedicated and common k1 candidates valid for a PDSCH transmitted in each slot.
  common_k1_list = pucch_td_helper::get_common_k1_candidates(common_min_k1);
  dedicated_k1_per_slot =
      get_pucch_k1_list_per_slot(dedicated_k1_list, params.tdd_cfg, pusch_td_res_list, pusch_td_res_indices_per_slot);
  common_k1_per_slot =
      get_pucch_k1_list_per_slot(common_k1_list, params.tdd_cfg, pusch_td_res_list, pusch_td_res_indices_per_slot);
}
