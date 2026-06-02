// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "pucch_collision_manager.h"
#include "../support/bwp_helpers.h"
#include "../support/pucch/pucch_collision.h"
#include "ocudu/adt/bounded_bitset.h"
#include "ocudu/adt/expected.h"
#include "ocudu/adt/static_vector.h"
#include "ocudu/ran/pucch/pucch_constants.h"
#include "ocudu/ran/resource_allocation/rb_interval.h"
#include "ocudu/scheduler/config/cell_bwp_res_config.h"
#include "ocudu/scheduler/resource_grid_util.h"
#include <algorithm>

using namespace ocudu;
using namespace ocudu::detail;

// Returns the pucch_resource associated to a given column in the collision/multiplexing region matrices.
static const pucch_resource& get_res(const cell_pucch_res_config& cell_resources, unsigned res_idx)
{
  if (res_idx < pucch_constants::MAX_NOF_CELL_COMMON_PUCCH_RESOURCES) {
    return cell_resources.common[res_idx];
  }
  return cell_resources.dedicated[res_idx - pucch_constants::MAX_NOF_CELL_COMMON_PUCCH_RESOURCES];
}

/// Returns the index of a given PUCCH resource in the collision/multiplexing region matrices.
static unsigned get_res_idx(const cell_pucch_res_config& cell_resources, const pucch_res_id_t& res_id)
{
  if (res_id.is_cmn()) {
    ocudu_assert(res_id.cmn().r_pucch < cell_resources.common.size(),
                 "Invalid r_pucch for common PUCCH resource: {}",
                 res_id.cmn().r_pucch);
    return res_id.cmn().r_pucch;
  }
  ocudu_assert(res_id.ded().cell_res_id < cell_resources.dedicated.size(),
               "PUCCH cell resource ID {} exceeds the size of the dedicated cell resource list ({})",
               res_id.ded().cell_res_id,
               cell_resources.dedicated.size());
  return res_id.ded().cell_res_id + pucch_constants::MAX_NOF_CELL_COMMON_PUCCH_RESOURCES;
}

// Computes a grant_info for one hop of a pucch_resource, for use with the UL resource grid.
static grant_info pucch_hop_grant(const pucch_resource& res, const bwp_configuration& bwp_cfg, bool first_hop)
{
  const unsigned half = res.syms.length() / 2;
  if (!res.second_hop_prb.has_value()) {
    return {bwp_cfg.scs, res.syms, prb_to_crb(bwp_cfg, res.prbs())};
  }
  if (first_hop) {
    return {bwp_cfg.scs, {res.syms.start(), res.syms.start() + half}, prb_to_crb(bwp_cfg, res.prbs())};
  }
  return {bwp_cfg.scs,
          {res.syms.start() + half, res.syms.stop()},
          prb_to_crb(bwp_cfg, prb_interval::start_and_len(*res.second_hop_prb, res.prbs().length()))};
}

namespace ocudu::detail {

collision_matrix make_collision_matrix(const cell_pucch_res_config& cell_resources)
{
  collision_matrix matrix(cell_resources.nof_total_res(),
                          bounded_bitset<pucch_constants::MAX_NOF_TOT_CELL_RESOURCES>(cell_resources.nof_total_res()));

  for (size_t i = 0; i != cell_resources.nof_total_res(); ++i) {
    // A resource always collides with itself.
    matrix[i].set(i);

    // Note: The collision matrix is symmetric.
    for (size_t j = i + 1; j != cell_resources.nof_total_res(); ++j) {
      if (pucch_resources_collide(get_res(cell_resources, i), get_res(cell_resources, j))) {
        matrix[i].set(j);
        matrix[j].set(i);
      }
    }
  }

  return matrix;
}

mux_regions_matrix make_mux_regions_matrix(const cell_pucch_res_config& cell_resources)
{
  struct mux_region {
    pucch_format                                                format;
    prb_interval                                                prbs;
    ofdm_symbol_range                                           syms;
    std::optional<uint16_t>                                     second_hop_prb;
    bounded_bitset<pucch_constants::MAX_NOF_TOT_CELL_RESOURCES> members;

    // Check if a given resource belongs to this multiplexing region.
    bool does_resource_belong(const pucch_resource& res) const
    {
      return res.format() == format and res.prbs() == prbs and res.syms == syms and
             res.second_hop_prb == second_hop_prb;
    }
  };

  static_vector<mux_region, pucch_constants::MAX_NOF_TOT_CELL_RESOURCES> tmp_regions;

  for (size_t i = 0; i != cell_resources.nof_total_res(); ++i) {
    const auto& res = get_res(cell_resources, i);

    // Find if the resource belongs to an existing multiplexing region.
    auto* region_it = std::find_if(tmp_regions.begin(), tmp_regions.end(), [&res](const mux_region& region) {
      return region.does_resource_belong(res);
    });

    if (region_it == tmp_regions.end()) {
      // If the multiplexing region does not exist yet, create it.
      region_it = &tmp_regions.emplace_back(
          mux_region{res.format(),
                     res.prbs(),
                     res.syms,
                     res.second_hop_prb,
                     bounded_bitset<pucch_constants::MAX_NOF_TOT_CELL_RESOURCES>(cell_resources.nof_total_res())});
    }

    // Add the resource to the multiplexing region.
    region_it->members.set(i);
  }

  mux_regions_matrix mux_regions;
  for (const auto& record : tmp_regions) {
    // Return only multiplexing regions with more than one resource.
    if (record.members.count() < 2) {
      continue;
    }
    mux_regions.push_back(record.members);
  }
  return mux_regions;
}

} // namespace ocudu::detail

pucch_collision_manager::pucch_collision_manager(const cell_configuration& cell_cfg_) :
  cell_cfg(cell_cfg_),
  col_matrix(make_collision_matrix(cell_cfg.bwp_res[to_bwp_id(0)].ul().pucch)),
  mux_matrix(make_mux_regions_matrix(cell_cfg.bwp_res[to_bwp_id(0)].ul().pucch)),
  mux_region_lookup(build_mux_region_lookup(mux_matrix)),
  // The ring must be at least 1 slot larger than the max. UL allocation delay, to take into account the current slot.
  slots_ctx(get_allocator_ring_size_gt_min(get_max_slot_ul_alloc_delay(cell_cfg.ntn_cs_koffset) + 1),
            {slot_context(cell_cfg)})
{
}

void pucch_collision_manager::slot_indication(slot_point sl_tx)
{
  ocudu_sanity_check(not last_sl_ind.valid() or sl_tx == last_sl_ind + 1, "Detected a skipped slot");

  // Clear previous slot context.
  slots_ctx[(sl_tx - 1).count()].clear();
  // Update last slot indication.
  last_sl_ind = sl_tx;
}

void pucch_collision_manager::stop()
{
  // Clear all slot contexts.
  for (auto& ctx : slots_ctx) {
    ctx.clear();
  }
  last_sl_ind = {};
}

error_type<pucch_alloc_failure> pucch_collision_manager::can_alloc(cell_slot_resource_allocator& slot_alloc,
                                                                   const pucch_resource&         res,
                                                                   rnti_t                        rnti) const
{
  ocudu_sanity_check(slot_alloc.slot < last_sl_ind + slots_ctx.size(),
                     "PUCCH resource ring-buffer accessed too far into the future");

  const auto&    ctx     = slots_ctx[slot_alloc.slot.count()];
  const auto&    bwp_cfg = cell_cfg.params.ul_cfg_common.init_ul_bwp.generic_params;
  const unsigned res_idx = get_res_idx(cell_cfg.bwp_res[to_bwp_id(0)].ul().pucch, res.res_id);

  // Check if the resource is already in use.
  if (ctx.owners[res_idx] != rnti_t::INVALID_RNTI) {
    if (ctx.owners[res_idx] == rnti) {
      return make_unexpected(pucch_alloc_failure::ALREADY_ALLOCATED);
    }
    return make_unexpected(pucch_alloc_failure::RESOURCE_IN_USE);
  }

  // Check for PUCCH-to-other UL grant collisions using the resource grids.
  const grant_info                first_hop = pucch_hop_grant(res, bwp_cfg, true);
  const std::optional<grant_info> second_hop =
      res.second_hop_prb.has_value() ? std::optional{pucch_hop_grant(res, bwp_cfg, false)} : std::nullopt;
  if (slot_alloc.ul_res_grid.collides(first_hop, &ctx.pucch_res_grid)) {
    return make_unexpected(pucch_alloc_failure::UL_GRANT_COLLISION);
  }
  if (second_hop.has_value() and slot_alloc.ul_res_grid.collides(*second_hop, &ctx.pucch_res_grid)) {
    return make_unexpected(pucch_alloc_failure::UL_GRANT_COLLISION);
  }

  // Check for PUCCH-to-PUCCH collisions using the collision matrix.
  const auto& row = col_matrix[res_idx];
  if ((row & ctx.current_state).any()) {
    return make_unexpected(pucch_alloc_failure::PUCCH_COLLISION);
  }

  // If all checks passed, the resource can be allocated.
  return default_success_t();
}

error_type<pucch_alloc_failure>
pucch_collision_manager::alloc(cell_slot_resource_allocator& slot_alloc, const pucch_resource& res, rnti_t rnti)
{
  if (auto alloc_res = can_alloc(slot_alloc, res, rnti); not alloc_res.has_value()) {
    return make_unexpected(alloc_res.error());
  }

  // Allocate the resource.
  do_alloc(slot_alloc, res, rnti);
  return default_success_t();
}

void pucch_collision_manager::do_alloc(cell_slot_resource_allocator& slot_alloc, const pucch_resource& res, rnti_t rnti)
{
  const auto&                     bwp_cfg   = cell_cfg.params.ul_cfg_common.init_ul_bwp.generic_params;
  const unsigned                  res_idx   = get_res_idx(cell_cfg.bwp_res[to_bwp_id(0)].ul().pucch, res.res_id);
  const grant_info                first_hop = pucch_hop_grant(res, bwp_cfg, true);
  const std::optional<grant_info> second_hop =
      res.second_hop_prb.has_value() ? std::optional{pucch_hop_grant(res, bwp_cfg, false)} : std::nullopt;
  auto& ctx           = slots_ctx[slot_alloc.slot.count()];
  ctx.owners[res_idx] = rnti;
  ctx.current_state.set(res_idx);
  slot_alloc.ul_res_grid.fill(first_hop);
  ctx.pucch_res_grid.fill(first_hop);
  if (second_hop.has_value()) {
    slot_alloc.ul_res_grid.fill(*second_hop);
    ctx.pucch_res_grid.fill(*second_hop);
  }
}

bool pucch_collision_manager::free(cell_slot_resource_allocator& slot_alloc, const pucch_resource& res, rnti_t rnti)
{
  ocudu_sanity_check(slot_alloc.slot < last_sl_ind + slots_ctx.size(),
                     "PUCCH resource ring-buffer accessed too far into the future");
  auto&          ctx     = slots_ctx[slot_alloc.slot.count()];
  const unsigned res_idx = get_res_idx(cell_cfg.bwp_res[to_bwp_id(0)].ul().pucch, res.res_id);
  if (ctx.owners[res_idx] != rnti or not ctx.current_state.test(res_idx)) {
    // Resource was not allocated to this UE.
    return false;
  }
  ctx.owners[res_idx] = rnti_t::INVALID_RNTI;
  ctx.current_state.reset(res_idx);

  // Check if any other resource in the same multiplexing region is still allocated.
  // If not, clear the grants in ul_res_grid and ctx.pucch_res_grid.
  if (mux_region_lookup.contains(res_idx)) {
    const auto& mux_region = mux_matrix[mux_region_lookup[res_idx]];
    if ((mux_region & ctx.current_state).any()) {
      return true;
    }
  }

  // Clear grants in ul_res_grid and ctx.pucch_res_grid.
  const auto&      bwp_cfg   = cell_cfg.params.ul_cfg_common.init_ul_bwp.generic_params;
  const grant_info first_hop = pucch_hop_grant(res, bwp_cfg, true);
  slot_alloc.ul_res_grid.clear(first_hop);
  ctx.pucch_res_grid.clear(first_hop);
  if (res.second_hop_prb.has_value()) {
    const grant_info second_hop = pucch_hop_grant(res, bwp_cfg, false);
    slot_alloc.ul_res_grid.clear(second_hop);
    ctx.pucch_res_grid.clear(second_hop);
  }
  return true;
}

pucch_collision_manager::mux_region_lookup_t
pucch_collision_manager::build_mux_region_lookup(const detail::mux_regions_matrix& mux_matrix)
{
  mux_region_lookup_t lookup;
  for (unsigned mux_region_idx = 0; mux_region_idx != mux_matrix.size(); ++mux_region_idx) {
    const auto& region = mux_matrix[mux_region_idx];
    for (unsigned res_idx = 0; res_idx != region.size(); ++res_idx) {
      if (not region.test(res_idx)) {
        continue;
      }
      ocudu_assert(not lookup.contains(res_idx), "PUCCH resource {} belongs to multiple multiplexing regions", res_idx);
      lookup.emplace(res_idx, mux_region_idx);
    }
  }

  return lookup;
}

pucch_collision_manager::slot_context::slot_context(const cell_configuration& cell_cfg) :
  current_state(cell_cfg.bwp_res[to_bwp_id(0)].ul().pucch.nof_total_res()),
  owners(cell_cfg.bwp_res[to_bwp_id(0)].ul().pucch.nof_total_res(), rnti_t::INVALID_RNTI),
  pucch_res_grid(cell_cfg.params.ul_cfg_common.freq_info_ul.scs_carrier_list)
{
}

void pucch_collision_manager::slot_context::clear()
{
  current_state.reset();
  std::fill(owners.begin(), owners.end(), rnti_t::INVALID_RNTI);
  pucch_res_grid.clear();
}
