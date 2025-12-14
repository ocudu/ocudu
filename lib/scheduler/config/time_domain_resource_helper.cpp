// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/scheduler/config/time_domain_resource_helper.h"
#include "ocudu/ran/pusch/pusch_constants.h"
#include "ocudu/ran/tdd/tdd_ul_dl_config.h"
#include "ocudu/scheduler/config/bwp_configuration.h"
#include "ocudu/scheduler/config/serving_cell_config.h"
#include "ocudu/scheduler/sched_consts.h"
#include <set>

using namespace ocudu;

static_vector<uint8_t, 8> time_domain_resource_helper::generate_k1_candidates(const tdd_ul_dl_config_common& tdd_cfg,
                                                                              uint8_t                        min_k1)
{
  // TS38.213, 9.1.2.1 - "If a UE is provided dl-DataToUL-ACK, the UE does not expect to be indicated by DCI format 1_0
  // a slot timing value for transmission of HARQ-ACK information that does not belong to the intersection of the set
  // of slot timing values {1, 2, 3, 4, 5, 6, 7, 8} and the set of slot timing values provided by dl-DataToUL-ACK for
  // the active DL BWP of a corresponding serving cell.

  const unsigned tdd_period   = nof_slots_per_tdd_period(tdd_cfg);
  unsigned       nof_dl_slots = tdd_cfg.pattern1.nof_dl_slots + (tdd_cfg.pattern1.nof_dl_symbols > 0 ? 1 : 0);
  if (tdd_cfg.pattern2.has_value()) {
    nof_dl_slots += tdd_cfg.pattern2->nof_dl_slots + (tdd_cfg.pattern2->nof_dl_symbols > 0 ? 1 : 0);
  }

  // Fill list of k1 candidates, prioritizing low k1 values to minimize latency, and avoiding having DL slots without
  // a k1 value to choose from that corresponds to an UL slot. We reuse std::set to ensure the ordering of k1.
  std::set<uint8_t>    result_set;
  std::vector<uint8_t> next_k1_for_dl_slot(nof_dl_slots, min_k1);
  unsigned             prev_size = 0;
  do {
    prev_size = result_set.size();
    for (unsigned idx = 0, dl_idx = 0; idx < tdd_period and result_set.size() < MAX_K1_CANDIDATES; ++idx) {
      if (has_active_tdd_dl_symbols(tdd_cfg, idx)) {
        for (unsigned k1 = next_k1_for_dl_slot[dl_idx]; k1 <= SCHEDULER_MAX_K1; ++k1) {
          if (has_active_tdd_ul_symbols(tdd_cfg, idx + k1)) {
            result_set.insert(k1);
            next_k1_for_dl_slot[dl_idx] = k1 + 1;
            break;
          }
        }
        dl_idx++;
      }
    }
  } while (prev_size != result_set.size() and result_set.size() < 8);

  // Results are stored in ascending order to reduce the latency with which UCI is scheduled.
  static_vector<uint8_t, 8> result(result_set.size());
  std::copy(result_set.begin(), result_set.end(), result.begin());
  return result;
}

static_vector<uint8_t, 8>
time_domain_resource_helper::generate_k1_candidates(const std::optional<tdd_ul_dl_config_common>& tdd_cfg,
                                                    uint8_t                                       min_k1)
{
  if (tdd_cfg.has_value()) {
    return generate_k1_candidates(*tdd_cfg, min_k1);
  }
  return {min_k1};
}

/// Helper function to determine the start of the PDSCH symbols, considering a CORESET config.
static uint8_t calc_min_pdsch_symbol(const coreset_configuration&       cs_cfg,
                                     const pdcch_config_common&         common_pdcch_cfg,
                                     const std::optional<pdcch_config>& ded_pdcch_cfg)
{
  // Note: TS 38.213, 10.1 - "A UE does not expect any two PDCCH monitoring occasions on an active DL BWP, for a
  // same search space set or for different search space sets, in a same CORESET to be separated by a non-zero
  // number of symbols that is smaller than the CORESET duration." -> This means that all searchSpaces of the same
  // CORESET need to share the same symbol start.

  for (const search_space_configuration& ss_cfg : common_pdcch_cfg.search_spaces) {
    if (ss_cfg.get_coreset_id() == cs_cfg.get_id()) {
      return ss_cfg.get_first_symbol_index();
    }
  }
  if (ded_pdcch_cfg.has_value()) {
    for (const search_space_configuration& ss_cfg : ded_pdcch_cfg->search_spaces) {
      if (ss_cfg.get_coreset_id() == cs_cfg.get_id()) {
        return ss_cfg.get_first_symbol_index();
      }
    }
  }
  return 0;
}

uint8_t time_domain_resource_helper::calculate_minimum_pdsch_symbol(const pdcch_config_common&         common_pdcch_cfg,
                                                                    const std::optional<pdcch_config>& ded_pdcch_cfg)
{
  // TODO: Support k0 != 0.
  // TODO: Consider SearchSpace periodicity while generating PDSCH OFDM symbol range. If there is no PDCCH, there is no
  //  PDSCH since we dont support k0 != 0 yet.

  const std::optional<coreset_configuration>& coreset0       = common_pdcch_cfg.coreset0;
  const std::optional<coreset_configuration>& common_coreset = common_pdcch_cfg.common_coreset;

  unsigned min_pdsch_symbol = 0;

  if (coreset0.has_value()) {
    min_pdsch_symbol = calc_min_pdsch_symbol(*coreset0, common_pdcch_cfg, ded_pdcch_cfg) + coreset0->duration();
  }
  if (common_coreset.has_value()) {
    min_pdsch_symbol =
        std::max(min_pdsch_symbol,
                 calc_min_pdsch_symbol(*common_coreset, common_pdcch_cfg, ded_pdcch_cfg) + common_coreset->duration());
  }
  if (ded_pdcch_cfg.has_value()) {
    for (const coreset_configuration& cs_cfg : ded_pdcch_cfg->coresets) {
      min_pdsch_symbol = std::max(min_pdsch_symbol,
                                  calc_min_pdsch_symbol(cs_cfg, common_pdcch_cfg, ded_pdcch_cfg) + cs_cfg.duration());
    }
  }

  return min_pdsch_symbol;
}

std::vector<pdsch_time_domain_resource_allocation>
time_domain_resource_helper::generate_dedicated_pdsch_td_res_list(const tdd_ul_dl_config_common& tdd_cfg,
                                                                  cyclic_prefix                  cp,
                                                                  uint8_t                        min_pdsch_symbol)
{
  // See TS 38.214, Table 5.1.2.1-1: Valid S and L combinations.
  static constexpr unsigned pdsch_mapping_typeA_min_L_value = 3;

  const unsigned nof_ofdm_symbols =
      cp == cyclic_prefix::NORMAL ? NOF_OFDM_SYM_PER_SLOT_NORMAL_CP : NOF_OFDM_SYM_PER_SLOT_EXTENDED_CP;

  // Generate OFDM symbol list considering \c min_pdsch_symbol as first symbol and taking into account the number of
  // symbols of special slots being high enough to accommodate \c pdsch_mapping_typeA_min_L_value symbols.
  std::set<ofdm_symbol_range> pdsch_symbols;
  pdsch_symbols.emplace(min_pdsch_symbol, nof_ofdm_symbols);
  if (tdd_cfg.pattern1.nof_dl_symbols >= min_pdsch_symbol + pdsch_mapping_typeA_min_L_value) {
    // There are enough DL symbols in pattern 1 special slot for PDSCH.
    pdsch_symbols.emplace(min_pdsch_symbol, tdd_cfg.pattern1.nof_dl_symbols);
  }
  if (tdd_cfg.pattern2.has_value() and
      tdd_cfg.pattern2->nof_dl_symbols >= min_pdsch_symbol + pdsch_mapping_typeA_min_L_value) {
    // There are enough DL symbols in pattern 2 special slot for PDSCH.
    pdsch_symbols.emplace(min_pdsch_symbol, tdd_cfg.pattern2->nof_dl_symbols);
  }

  // Make PDSCH time domain resource allocation.
  // Note: k0 > 0 not supported yet.
  std::vector<pdsch_time_domain_resource_allocation> result;
  result.reserve(pdsch_symbols.size());
  for (const auto& symbs : pdsch_symbols) {
    result.push_back(
        pdsch_time_domain_resource_allocation{.k0 = 0, .map_type = sch_mapping_type::typeA, .symbols = symbs});
  }

  // Sort PDSCH time domain resource allocations in:
  // - ascending order of k0
  // - descending or OFDM symbol range length to always choose the resource which occupies most of the DL symbols in
  // a slot.
  std::sort(result.begin(),
            result.end(),
            [](const pdsch_time_domain_resource_allocation& lhs, const pdsch_time_domain_resource_allocation& rhs) {
              return lhs.k0 < rhs.k0 or (lhs.k0 == rhs.k0 and lhs.symbols.length() > rhs.symbols.length());
            });

  return result;
}

std::vector<pdsch_time_domain_resource_allocation>
time_domain_resource_helper::generate_dedicated_pdsch_td_res_list(const std::optional<tdd_ul_dl_config_common>& tdd_cfg,
                                                                  cyclic_prefix                                 cp,
                                                                  uint8_t min_pdsch_symbol)
{
  if (tdd_cfg.has_value()) {
    // TDD case.
    return generate_dedicated_pdsch_td_res_list(*tdd_cfg, cp, min_pdsch_symbol);
  }

  // FDD case.
  std::vector<pdsch_time_domain_resource_allocation> result(1);
  result[0].k0       = 0;
  result[0].map_type = sch_mapping_type::typeA;
  result[0].symbols  = {min_pdsch_symbol,
                       cp == cyclic_prefix::NORMAL ? NOF_OFDM_SYM_PER_SLOT_NORMAL_CP
                                                    : NOF_OFDM_SYM_PER_SLOT_EXTENDED_CP};
  return result;
}

std::vector<pusch_time_domain_resource_allocation>
time_domain_resource_helper::generate_dedicated_pusch_td_res_list(const tdd_ul_dl_config_common& tdd_cfg,
                                                                  cyclic_prefix                  cp,
                                                                  uint8_t                        min_k2)
{
  const unsigned symbols_per_slot  = get_nsymb_per_slot(cp);
  const unsigned tdd_period_slots  = nof_slots_per_tdd_period(tdd_cfg);
  const unsigned nof_dl_slots      = nof_dl_slots_per_tdd_period(tdd_cfg);
  const unsigned nof_full_ul_slots = nof_full_ul_slots_per_tdd_period(tdd_cfg);

  std::vector<pusch_time_domain_resource_allocation> result;
  result.reserve(pusch_constants::MAX_NOF_PUSCH_TD_RES_ALLOCS);
  for (unsigned idx = 0; idx < tdd_period_slots and result.size() < pusch_constants::MAX_NOF_PUSCH_TD_RES_ALLOCS;
       ++idx) {
    // For every slot containing DL symbols check for corresponding k2 value.
    if (not get_active_tdd_dl_symbols(tdd_cfg, idx, cp).empty()) {
      for (uint8_t k2 = min_k2; k2 <= SCHEDULER_MAX_K2 and result.size() < pusch_constants::MAX_NOF_PUSCH_TD_RES_ALLOCS;
           ++k2) {
        // TODO: Consider partial UL slots when scheduler supports it.
        if (get_active_tdd_ul_symbols(tdd_cfg, idx + k2, cp).length() == symbols_per_slot) {
          if (std::none_of(result.begin(), result.end(), [k2](const auto& res) { return res.k2 == k2; })) {
            result.emplace_back(pusch_time_domain_resource_allocation{
                k2, sch_mapping_type::typeA, ofdm_symbol_range{0, symbols_per_slot}});
          }
          // [Implementation-defined] For DL heavy (nof. of DL slots greater than nof. UL slots) TDD configuration nof.
          // k2 values are generated based on nof. DL slots i.e. one k2 value per DL slot. But in the case of UL
          // heavy TDD configuration we generate all applicable k2 value(s) for each DL slot to allow multiple UL PDCCH
          // allocations in the same slot for same UE.
          if (nof_dl_slots > nof_full_ul_slots) {
            break;
          }
        }
      }
    }
  }
  // Sorting in ascending order is performed to reduce the latency with which PUSCH is scheduled.
  std::sort(result.begin(), result.end(), [](const auto& lhs, const auto& rhs) {
    return lhs.k2 < rhs.k2 or (lhs.k2 == rhs.k2 and lhs.symbols.length() > rhs.symbols.length());
  });
  return result;
}

std::vector<pusch_time_domain_resource_allocation>
time_domain_resource_helper::generate_dedicated_pusch_td_res_list(const std::optional<tdd_ul_dl_config_common>& tdd_cfg,
                                                                  cyclic_prefix                                 cp,
                                                                  uint8_t                                       min_k2)
{
  if (tdd_cfg.has_value()) {
    // TDD case.
    return generate_dedicated_pusch_td_res_list(*tdd_cfg, cp, min_k2);
  }

  return {pusch_time_domain_resource_allocation{
      min_k2,
      sch_mapping_type::typeA,
      ofdm_symbol_range{
          0, cp == cyclic_prefix::NORMAL ? NOF_OFDM_SYM_PER_SLOT_NORMAL_CP : NOF_OFDM_SYM_PER_SLOT_EXTENDED_CP}}};
}
