// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "pdcch_slot_resource_allocator.h"
#include "../cell/resource_grid.h"
#include "../support/pdcch/pdcch_mapping.h"
#include "ocudu/ran/pdcch/cce_to_prb_mapping.h"

using namespace ocudu;

pdcch_slot_allocator::pdcch_slot_allocator()
{
  dfs_tree.reserve(MAX_DL_PDCCH_PDUS_PER_SLOT + MAX_UL_PDCCH_PDUS_PER_SLOT);
  saved_dfs_tree.reserve(MAX_DL_PDCCH_PDUS_PER_SLOT + MAX_UL_PDCCH_PDUS_PER_SLOT);
}

pdcch_slot_allocator::~pdcch_slot_allocator() {}

void pdcch_slot_allocator::clear()
{
  records.clear();
  dfs_tree.clear();
  saved_dfs_tree.clear();
}

bool pdcch_slot_allocator::alloc_pdcch(dci_context_information&          pdcch_ctx,
                                       cell_slot_resource_allocator&     slot_alloc,
                                       const sched_coreset_config&       cs_cfg,
                                       const search_space_configuration& ss_cfg,
                                       span<const pdcch_candidate_type>  ss_candidates)
{
  saved_dfs_tree.clear();

  // Create an DL Allocation Record.
  alloc_record record{};
  record.is_dl            = true;
  record.pdcch_ctx        = &pdcch_ctx;
  record.cs_cfg           = &cs_cfg;
  record.ss_cfg           = &ss_cfg;
  record.pdcch_candidates = ss_candidates;

  // Try to allocate PDCCH for one of the possible CCE positions. If this operation fails, retry it, but using a
  // different permutation of past grant CCE positions.
  do {
    if (alloc_dfs_node(slot_alloc, record, 0)) {
      // Set CCE start index.
      record.pdcch_ctx->cces.ncce = dfs_tree.back().ncce;
      // DCI record was successfully allocated.
      records.push_back(record);
      // If the previous branches of the DFS changed in order for this new PDCCH to be allocated, update previous
      // PDCCH ncces
      if (not saved_dfs_tree.empty()) {
        for (unsigned i = 0; i != dfs_tree.size() - 1; ++i) {
          records[dfs_tree[i].record_index].pdcch_ctx->cces.ncce = dfs_tree[i].ncce;
        }
      }
      return true;
    }
    if (saved_dfs_tree.empty()) {
      saved_dfs_tree = dfs_tree;
    }
  } while (get_next_dfs(slot_alloc));

  // Revert steps to initial state, before dci record allocation was attempted
  dfs_tree.swap(saved_dfs_tree);

  return false;
}

bool pdcch_slot_allocator::cancel_last_pdcch(cell_slot_resource_allocator& slot_alloc)
{
  if (records.empty()) {
    return false;
  }
  auto& last_rec = records.back();

  const bool is_dl_pdcch =
      not slot_alloc.result.dl.dl_pdcchs.empty() and last_rec.pdcch_ctx == &slot_alloc.result.dl.dl_pdcchs.back().ctx;
  const bool is_ul_pdcch = not is_dl_pdcch and not slot_alloc.result.dl.ul_pdcchs.empty() and
                           last_rec.pdcch_ctx == &slot_alloc.result.dl.ul_pdcchs.back().ctx;
  if (not is_dl_pdcch and not is_ul_pdcch) {
    return false;
  }

  // Read the resources taken by the PDCCH while its PDU is still alive.
  const sched_coreset_config& cs_cfg   = *last_rec.cs_cfg;
  const crb_index_list_span pdcch_crbs = cs_cfg.candidate_crbs(dfs_tree.back().ncce, last_rec.pdcch_ctx->cces.aggr_lvl);
  const ofdm_symbol_range   symbols{0, (uint8_t)cs_cfg.cfg().duration()};
  const subcarrier_spacing  scs = last_rec.pdcch_ctx->bwp_cfg->scs;

  if (is_dl_pdcch) {
    slot_alloc.result.dl.dl_pdcchs.pop_back();
  } else {
    slot_alloc.result.dl.ul_pdcchs.pop_back();
  }

  // Clear allocation on resource grid.
  slot_alloc.dl_res_grid.clear(scs, symbols, pdcch_crbs);

  dfs_tree.pop_back();
  records.pop_back();
  return true;
}

bool pdcch_slot_allocator::alloc_dfs_node(cell_slot_resource_allocator& slot_alloc,
                                          const alloc_record&           record,
                                          unsigned                      dci_iter_index)
{
  // Get CCE location table, i.e. the current node possible leaves.
  span<const uint8_t> cce_locs = record.pdcch_candidates;
  if (dci_iter_index >= cce_locs.size()) {
    // All possible CCE position leaves have been attempted. Early return.
    return false;
  }

  // Create new tree leave.
  tree_node node{};
  node.dci_iter_index = dci_iter_index;
  node.record_index   = dfs_tree.size();

  // Find in the list of possible CCEs, a CCE that does not cause PDCCH collisions.
  for (; node.dci_iter_index < cce_locs.size(); ++node.dci_iter_index) {
    node.ncce = cce_locs[node.dci_iter_index];

    // Attempt to allocate PDCCH with provided CCE position.
    if (not allocate_cce(slot_alloc, record, node.dci_iter_index)) {
      continue;
    }

    // Allocation successful. Add node to tree.
    dfs_tree.push_back(node);
    return true;
  }
  return false;
}

bool pdcch_slot_allocator::get_next_dfs(cell_slot_resource_allocator& slot_alloc)
{
  do {
    if (dfs_tree.empty()) {
      // If we reach root, the allocation failed.
      return false;
    }
    // Attempt to re-add last tree node, but with a higher leaf index.
    uint32_t start_child_idx = dfs_tree.back().dci_iter_index + 1;
    dfs_tree.pop_back();
    while (dfs_tree.size() < records.size() and alloc_dfs_node(slot_alloc, records[dfs_tree.size()], start_child_idx)) {
      start_child_idx = 0;
    }
  } while (dfs_tree.size() < records.size());

  // Finished computation of next DFS node.
  return true;
}

bool pdcch_slot_allocator::allocate_cce(cell_slot_resource_allocator& slot_alloc,
                                        const alloc_record&           record,
                                        unsigned                      dci_iter_index)
{
  const bwp_configuration&     bwp_cfg = *record.pdcch_ctx->bwp_cfg;
  const coreset_configuration& cs_cfg  = *record.pdcch_ctx->coreset_cfg;
  const crb_index_list_span    pdcch_crbs =
      record.cs_cfg->candidate_crbs(record.pdcch_candidates[dci_iter_index], record.pdcch_ctx->cces.aggr_lvl);
  ocudu_sanity_check(pdcch_crbs.size() > 0, "PDCCH candidate CRB list is empty");
  grant_info grant;
  grant.scs = bwp_cfg.scs;

  // Check the current CCE position collides with an existing one.
  ofdm_symbol_range symbols{0, (uint8_t)cs_cfg.duration()};
  if (slot_alloc.dl_res_grid.collides(record.pdcch_ctx->bwp_cfg->scs, symbols, pdcch_crbs)) {
    // Collision detected. Try another CCE position.
    return false;
  }

  // Allocation successful.
  slot_alloc.dl_res_grid.fill(record.pdcch_ctx->bwp_cfg->scs, symbols, pdcch_crbs);

  return true;
}
