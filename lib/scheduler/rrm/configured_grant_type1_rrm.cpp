// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/scheduler/rrm/configured_grant_type1_rrm.h"
#include "ocudu/adt/format.h"
#include "ocudu/ran/resource_allocation/resource_allocation_frequency.h"
#include "ocudu/scheduler/config/pucch_guardbands.h"
#include "ocudu/scheduler/config/ran_cell_config_helper.h"
#include "ocudu/scheduler/scheduler_configurator.h"
#include "ocudu/scheduler/support/rb_helper.h"

using namespace ocudu;

// Builds the CG allocation grid: one CRB bitmap per slot offset within the CG period, pre-filled with the CRBs taken
// by the PUCCH guardbands.
// NOTE: the grid does not mark the PRACH occasions. The slot offsets that collide with one, as well as those that are
// not full-UL slots, are left out of cell_context::usable_cg_offsets and therefore never allocated.
static std::vector<crb_bitmap> build_alloc_grid(const ran_cell_config& cell_cfg)
{
  ocudu_assert(cell_cfg.init_bwp.cg_cfg.has_value() and cell_cfg.init_bwp.cg_cfg.value().periodicity.has_value(),
               "Configured Grant must be configured and with a period set");
  const auto cg_period_sl = static_cast<unsigned>(cell_cfg.init_bwp.cg_cfg.value().periodicity.value());

  const crb_bitmap pucch_crbs = compute_pucch_crbs(cell_cfg);
  ocudu_assert(pucch_crbs.size() == cell_cfg.ul_cfg_common.init_ul_bwp.generic_params.crbs.length(),
               "PUCCH CRB bitmap size mismatch with BWP size");

  return std::vector<crb_bitmap>(cg_period_sl, pucch_crbs);
}

configured_grant_type1_rrm::cell_context::cell_context(const ran_cell_config& cell_cfg_) :
  cell_cfg(cell_cfg_),
  default_cg_config(config_helpers::make_default_cell_cg_config(cell_cfg_)),
  nof_rbs_per_ue(config_helpers::compute_nof_cg_prbs_per_ue(cell_cfg_, default_cg_config)),
  usable_cg_offsets(config_helpers::compute_cg_usable_slot_offsets(cell_cfg_)),
  cg_alloc_grid(build_alloc_grid(cell_cfg_)),
  nof_rbs_allocated(cg_alloc_grid.size(), 0U)
{
}

std::optional<unsigned> configured_grant_type1_rrm::cell_context::find_optimal_cg_offset() const
{
  const unsigned max_cg_rbs = cell_cfg.init_bwp.cg_cfg.value().max_nof_cell_cg_rbs;

  // Pick the usable offset with the fewest RBs already used for CG. The offsets are in increasing order, and the
  // comparison is strict, so ties are broken in favour of the earliest offset.
  std::optional<unsigned> optimal_offset;
  for (unsigned offset : usable_cg_offsets) {
    // Not enough room left at this offset to fit one more UE.
    if (nof_rbs_allocated[offset] + nof_rbs_per_ue > max_cg_rbs or cg_alloc_grid[offset].all()) {
      continue;
    }
    if (not optimal_offset.has_value() or nof_rbs_allocated[offset] < nof_rbs_allocated[optimal_offset.value()]) {
      optimal_offset = offset;
    }
  }

  return optimal_offset;
}

void configured_grant_type1_rrm::add_cell(du_cell_index_t cell_idx, const ran_cell_config& cell_cfg)
{
  // NOTE: the consistency of the CG parameters (i.e. that the number of RBs required per UE does not exceed the
  // maximum number of RBs reserved for CG in the cell) is enforced by the DU cell configuration validator, before it is
  // fed to this function.
  cells.emplace(cell_idx, cell_cfg);
}

void configured_grant_type1_rrm::rem_cell(du_cell_index_t cell_idx)
{
  cells.erase(cell_idx);
}

// The logic of this class to assign the CG resources is to first select the offset with the minimum number of RBs used
// for CG. Once the offset is chosen, the class allocates a set of contiguous RBs to the UE.
bool configured_grant_type1_rrm::build_ue_cg_config(ue_cell_config& ue_cell_cfg)
{
  // Skip cells without CG configured (not registered in the CG resource manager).
  if (not cells.contains(ue_cell_cfg.serv_cell_cfg.cell_index)) {
    return true;
  }

  auto&                  cell     = cells[ue_cell_cfg.serv_cell_cfg.cell_index];
  const ran_cell_config& cell_cfg = cell.cell_cfg;

  // A UE that already holds a CG allocation already has RBs reserved for it in the grid. Allocating a second one
  // would overwrite the offset and VRBs recorded in the UE config, and the RBs of the first allocation would stay
  // marked in cg_alloc_grid for good, as reset_ue_cg_config() can only release what the UE config still points at.
  // NOTE: this mirrors reset_ue_cg_config(), which is a no-op for a UE that holds no CG allocation.
  if (ue_cell_cfg.serv_cell_cfg.ul_config->init_ul_bwp.cg_cfg.has_value() or ue_cell_cfg.init_bwp().ul.cg.has_value()) {
    return true;
  }

  // Find the optimal CG offset (the offset with the min number of RBs for CG).
  const std::optional<unsigned> offset = cell.find_optimal_cg_offset();
  if (offset == std::nullopt) {
    return false;
  }
  const unsigned offset_val = offset.value();

  // Choose RBs.
  crb_interval cg_rbs = rb_helper::find_empty_interval_of_length(cell.cg_alloc_grid[offset_val], cell.nof_rbs_per_ue);

  if (cg_rbs.length() < cell.nof_rbs_per_ue) {
    return false;
  }

  // After this point, the allocation cannot fail.

  // Update the BWP configuration for this UE with the allocated offset and RBs.
  ue_cell_cfg.init_bwp().ul.cg.emplace();
  ue_cell_cfg.init_bwp().ul.cg.value().cg_offset = offset_val;

  const unsigned bwp_crb_start              = cell_cfg.ul_cfg_common.init_ul_bwp.generic_params.crbs.start();
  ue_cell_cfg.init_bwp().ul.cg.value().vrbs = rb_helper::crb_to_vrb_ul_non_interleaved(
      crb_interval{cg_rbs.start() + bwp_crb_start, cg_rbs.stop() + bwp_crb_start}, bwp_crb_start);
  // NOTE: CS-RNTI is allocated by the MAC layer's RNTI manager during UE reconfiguration and stored back via
  // set_cs_rnti().

  // Build the full CG configuration from the cell-level defaults. This populates all fields of cg_configuration,
  // including rrc_configured_ul_grant_cfg, from the cg_builder_params stored in the cell config.
  cg_configuration ue_cg_cfg = cell.default_cg_config;

  ocudu_assert(ue_cg_cfg.rrc_configured_ul_grant_cfg.has_value(),
               "rrc_configured_ul_grant must be set for a Type 1 CG");

  // Set the per-UE parameters.
  ue_cg_cfg.rrc_configured_ul_grant_cfg.value().time_domain_offset = offset_val;
  ue_cg_cfg.rrc_configured_ul_grant_cfg.value().freq_domain_res =
      ra_frequency_type1_configuration{cell_cfg.ul_cfg_common.init_ul_bwp.generic_params.crbs.length(),
                                       ue_cell_cfg.init_bwp().ul.cg.value().vrbs.start(),
                                       ue_cell_cfg.init_bwp().ul.cg.value().vrbs.length()};

  // > Common parameters: fill serving_cell_cfg with the full CG configuration.
  ue_cell_cfg.serv_cell_cfg.ul_config->init_ul_bwp.cg_cfg.emplace(ue_cg_cfg);

  // Register the used resources in the grid and counters.
  cell.nof_rbs_allocated[offset_val] += cg_rbs.length();
  cell.cg_alloc_grid[offset_val].fill(cg_rbs.start(), cg_rbs.stop());

  return true;
}

void configured_grant_type1_rrm::reset_ue_cg_config(ue_cell_config& ue_cell_cfg)
{
  if (not cells.contains(ue_cell_cfg.serv_cell_cfg.cell_index)) {
    return;
  }
  if (not ue_cell_cfg.serv_cell_cfg.ul_config.has_value() or
      not ue_cell_cfg.serv_cell_cfg.ul_config->init_ul_bwp.cg_cfg.has_value() or
      not ue_cell_cfg.init_bwp().ul.cg.has_value()) {
    return;
  }

  auto&       cell     = cells[ue_cell_cfg.serv_cell_cfg.cell_index];
  const auto& cell_cfg = cell.cell_cfg;

  // Recover the allocated CRBs from the UE BWP config.
  const auto&        ue_cg         = ue_cell_cfg.init_bwp().ul.cg;
  const unsigned     bwp_crb_start = cell_cfg.ul_cfg_common.init_ul_bwp.generic_params.crbs.start();
  const crb_interval cg_crbs       = rb_helper::vrb_to_crb_ul_non_interleaved(ue_cg.value().vrbs, bwp_crb_start);
  // Convert to BWP-relative indices used by the allocation grid.
  const unsigned crb_start = cg_crbs.start() - bwp_crb_start;
  const unsigned crb_stop  = cg_crbs.stop() - bwp_crb_start;

  // Remove the CRBs from the allocation grid and counters.
  const unsigned offset = ue_cg.value().cg_offset;
  cell.cg_alloc_grid[offset].fill(crb_start, crb_stop, false);
  ocudu_assert(cell.nof_rbs_allocated[offset] >= ue_cg.value().vrbs.length(),
               "nof_rbs_allocated underflow at slot offset={}",
               offset);
  cell.nof_rbs_allocated[offset] -= ue_cg.value().vrbs.length();

  // Reset the CG configuration.
  ue_cell_cfg.init_bwp().ul.cg.reset();
  ue_cell_cfg.serv_cell_cfg.ul_config->init_ul_bwp.cg_cfg.reset();
}
