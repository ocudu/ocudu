// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "pdcch_config_pool.h"
#include "../support/pdcch/pdcch_mapping.h"
#include "ocudu/adt/format.h"

using namespace ocudu;

static size_t total_nof_coresets(const pdcch_config_common& pdcch_common, const cell_dl_bwp_res_config& cell_res)
{
  unsigned count = 0;
  count += pdcch_common.coreset0.has_value() ? 1 : 0;
  count += pdcch_common.common_coreset.has_value() ? 1 : 0;
  for (const auto& pdcch : cell_res.ded_pdcchs) {
    count += pdcch.coresets.size();
  }
  return count;
}

static size_t total_nof_search_spaces(const pdcch_config_common& pdcch_common, const cell_dl_bwp_res_config& cell_res)
{
  unsigned count = 0;
  count += pdcch_common.search_spaces.size();
  for (const auto& pdcch : cell_res.ded_pdcchs) {
    count += pdcch.search_spaces.size();
  }
  return count;
}

pdcch_config_pool::pdcch_config_pool(pci_t                         pci,
                                     const bwp_configuration&      bwp_cfg,
                                     const pdcch_config_common&    pdcch_common,
                                     const cell_dl_bwp_res_config& cell_res)
{
  // Reserve coreset/search space pools so they don't get relocated.
  coreset_pool.reserve(total_nof_coresets(pdcch_common, cell_res));
  search_space_pool.reserve(total_nof_search_spaces(pdcch_common, cell_res));

  // Build PDCCH Common.
  slotted_id_vector<coreset_id, const sched_coreset_config*>           cs_list;
  slotted_id_vector<search_space_id, const sched_search_space_config*> ss_list;
  if (pdcch_common.coreset0.has_value()) {
    cs_list.emplace(to_coreset_id(0), &make_coreset(pci, bwp_cfg, *pdcch_common.coreset0));
  }
  if (pdcch_common.common_coreset.has_value()) {
    cs_list.emplace(pdcch_common.common_coreset->get_id(), &make_coreset(pci, bwp_cfg, *pdcch_common.common_coreset));
  }
  for (const search_space_configuration& ss : pdcch_common.search_spaces) {
    ss_list.emplace(ss.get_id(), &make_search_space(*cs_list[ss.get_coreset_id()], ss));
  }
  init_pdcch_cfg = sched_pdcch_config{cs_list,
                                      ss_list,
                                      pdcch_common.sib1_search_space_id <= MAX_SEARCH_SPACE_ID
                                          ? std::optional<search_space_id>{pdcch_common.sib1_search_space_id}
                                          : std::nullopt,
                                      pdcch_common.other_si_search_space_id,
                                      pdcch_common.paging_search_space_id,
                                      pdcch_common.ra_search_space_id <= MAX_SEARCH_SPACE_ID
                                          ? std::optional<search_space_id>{pdcch_common.ra_search_space_id}
                                          : std::nullopt};

  // Build dedicated PDCCH configs.
  ded_pdcch_cfgs.reserve(cell_res.ded_pdcchs.size());
  for (const auto& ded_cfg : cell_res.ded_pdcchs) {
    // Start with common as base config.
    cs_list = init_pdcch_cfg.coresets();
    ss_list = init_pdcch_cfg.search_spaces();

    for (const auto& cs : ded_cfg.coresets) {
      // TS 38.331, "PDCCH-Config" - In case network reconfigures control resource set with the same
      // ControlResourceSetId as used for commonControlResourceSet configured via PDCCH-ConfigCommon,
      // the configuration from PDCCH-Config always takes precedence and should not be updated by the UE based on
      // servingCellConfigCommon.
      cs_list.emplace(cs.get_id(), &make_coreset(pci, bwp_cfg, cs));
    }
    for (const auto& ss : ded_cfg.search_spaces) {
      ss_list.emplace(ss.get_id(), &make_search_space(*cs_list[ss.get_coreset_id()], ss));
    }

    ded_pdcch_cfgs.push_back(sched_pdcch_config{cs_list, ss_list});
  }
}

const sched_coreset_config&
pdcch_config_pool::make_coreset(pci_t pci, const bwp_configuration& bwp_cfg, const coreset_configuration& cs_cfg)
{
  auto it = std::find(coreset_pool.begin(), coreset_pool.end(), cs_cfg);
  if (it != coreset_pool.end()) {
    // CORESET config already exists.
    return *it;
  }

  std::array<std::vector<crb_index_list>, NOF_AGGREGATION_LEVELS> ncce_crbs;
  for (unsigned al_idx = 0; al_idx != NOF_AGGREGATION_LEVELS; ++al_idx) {
    const aggregation_level      aggr_lvl     = aggregation_index_to_level(al_idx);
    std::vector<crb_index_list>& al_crb_lists = ncce_crbs[al_idx];

    // Get PRBs for each candidate.
    // Note: Start CCE is always a multiple of L.
    const unsigned L = to_nof_cces(aggr_lvl);
    for (unsigned start_ncce = 0, nof_cces = cs_cfg.get_nof_cces(); start_ncce < nof_cces; start_ncce += L) {
      al_crb_lists.push_back(pdcch_helper::cce_to_prb_mapping(bwp_cfg, cs_cfg, pci, aggr_lvl, start_ncce));

      // Convert PRBs to CRBs.
      for (uint16_t& prb_idx : al_crb_lists.back()) {
        prb_idx = prb_to_crb(bwp_cfg.crbs, prb_idx);
      }
    }
  }

  coreset_pool.push_back(sched_coreset_config{cs_cfg, ncce_crbs});
  return coreset_pool.back();
}

const sched_search_space_config& pdcch_config_pool::make_search_space(const sched_coreset_config&       cs,
                                                                      const search_space_configuration& ss_cfg)
{
  auto it =
      std::find_if(search_space_pool.begin(),
                   search_space_pool.end(),
                   [&cs, &ss_cfg](const sched_search_space_config& e) { return e.cfg() == ss_cfg and cs == e.cs(); });
  if (it != search_space_pool.end()) {
    // Search Space config already exists.
    return *it;
  }

  search_space_pool.push_back(sched_search_space_config{cs, ss_cfg});
  return search_space_pool.back();
}
