// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/scheduler/rrm/configured_grant_type2_rrm.h"
#include "ocudu/adt/format.h"
#include "ocudu/scheduler/config/ran_cell_config_helper.h"
#include "ocudu/scheduler/scheduler_configurator.h"

using namespace ocudu;

// Computes how many UEs the cell can serve with a Type-2 CG: the number of CG resources that fit in the frequency
// domain, times the number of slot offsets within the CG period that are usable for CG. This mirrors the size of the
// resource pool that the scheduler's cg_type2_resource_manager builds for the same cell.
static unsigned compute_max_nof_cg_ues(const ran_cell_config& cell_cfg)
{
  return config_helpers::compute_cg_type2_freq_resources(cell_cfg).size() *
         config_helpers::compute_cg_usable_slot_offsets(cell_cfg).size();
}

configured_grant_type2_rrm::cell_context::cell_context(const ran_cell_config& cell_cfg_) :
  default_cg_config(config_helpers::make_default_cell_cg_config(cell_cfg_)),
  max_nof_ues(compute_max_nof_cg_ues(cell_cfg_))
{
}

void configured_grant_type2_rrm::add_cell(du_cell_index_t cell_idx, const ran_cell_config& cell_cfg)
{
  // NOTE: the consistency of the CG parameters (i.e. that the number of RBs required per UE does not exceed the
  // maximum number of RBs reserved for CG in the cell) is enforced by the DU cell configuration validator, before it is
  // fed to this function.
  cells.emplace(cell_idx, cell_cfg);
}

void configured_grant_type2_rrm::rem_cell(du_cell_index_t cell_idx)
{
  cells.erase(cell_idx);
}

bool configured_grant_type2_rrm::build_ue_cg_config(ue_cell_config& ue_cell_cfg)
{
  // Skip cells without CG configured (not registered in the CG resource manager).
  if (not cells.contains(ue_cell_cfg.serv_cell_cfg.cell_index)) {
    return true;
  }

  auto& cell = cells[ue_cell_cfg.serv_cell_cfg.cell_index];

  // Nothing to build for a UE with no UL configuration, which cannot carry a CG at all.
  //
  // Nothing to build either for a UE that already holds a CG configuration: it has already been counted against the
  // cell, and counting it twice would leave nof_ues above the number of UEs actually using the cell, with that
  // capacity never recovered. Unlike an underflow, an overcount violates no local invariant, so nothing downstream
  // can detect it.
  if (not ue_cell_cfg.serv_cell_cfg.ul_config.has_value() or
      ue_cell_cfg.serv_cell_cfg.ul_config->init_ul_bwp.cg_cfg.has_value()) {
    return true;
  }

  // The cell cannot host more UEs than the number of Type-2 CG resources it has; refuse the allocation, so that the
  // UE is not admitted with a CG configuration that the scheduler would not be able to activate.
  if (cell.nof_ues >= cell.max_nof_ues) {
    return false;
  }

  // Build the full CG configuration from the cell-level defaults. This populates all of cg_configuration, but excludes
  // rrc_configured_ul_grant_cfg, which should not be set for CG type 2.
  cg_configuration ue_cg_cfg = cell.default_cg_config;

  // NOTE: CS-RNTI is allocated by the MAC layer's RNTI manager during UE reconfiguration and stored back via
  // set_cs_rnti().

  ocudu_assert(not ue_cg_cfg.rrc_configured_ul_grant_cfg.has_value(),
               "rrc_configured_ul_grant must not be set for a Type 2 CG");

  // > Common parameters: fill serving_cell_cfg with the full CG configuration.
  ue_cell_cfg.serv_cell_cfg.ul_config->init_ul_bwp.cg_cfg.emplace(ue_cg_cfg);

  ++cell.nof_ues;

  return true;
}

void configured_grant_type2_rrm::reset_ue_cg_config(ue_cell_config& ue_cell_cfg)
{
  if (not cells.contains(ue_cell_cfg.serv_cell_cfg.cell_index)) {
    return;
  }
  if (not ue_cell_cfg.serv_cell_cfg.ul_config.has_value() or
      not ue_cell_cfg.serv_cell_cfg.ul_config->init_ul_bwp.cg_cfg.has_value()) {
    return;
  }

  auto& cell = cells[ue_cell_cfg.serv_cell_cfg.cell_index];
  ocudu_assert(cell.nof_ues != 0,
               "Number of CG UEs underflow at cell={}",
               fmt::underlying(ue_cell_cfg.serv_cell_cfg.cell_index));
  --cell.nof_ues;

  // Reset the CG configuration.
  ue_cell_cfg.serv_cell_cfg.ul_config->init_ul_bwp.cg_cfg.reset();
}
