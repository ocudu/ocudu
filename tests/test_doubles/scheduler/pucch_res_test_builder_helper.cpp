// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "pucch_res_test_builder_helper.h"
#include "ocudu/scheduler/config/pucch_resource_generator.h"
#include "ocudu/scheduler/config/ran_cell_config.h"
#include "ocudu/scheduler/config/sched_cell_config_helpers.h"
#include "ocudu/scheduler/scheduler_configurator.h"
#include "ocudu/support/ocudu_assert.h"

using namespace ocudu;

pucch_res_builder_test_helper::pucch_res_builder_test_helper(unsigned max_pucchs_per_slot) :
  pucch_res_mgr(max_pucchs_per_slot)
{
}

void pucch_res_builder_test_helper::setup(const ran_cell_config& cell_cfg)
{
  pucch_res_mgr.add_cell(to_du_cell_index(0), cell_cfg);
}

bool pucch_res_builder_test_helper::add_build_new_ue_pucch_cfg(ue_cell_config& ue_cell_cfg)
{
  ocudu_assert(pucch_res_mgr.contains(ue_cell_cfg.serv_cell_cfg.cell_index),
               "PUCCH Resource Manager does not contain the cell index of the UE's serving cell configuration");

  // Create a temporary struct that will be fed to the function alloc_resources().
  odu::cell_group_config cell_group_cfg;
  cell_group_cfg.cells.emplace(SERVING_PCELL_IDX, ue_cell_cfg);
  const bool alloc_outcome = pucch_res_mgr.alloc_resources(cell_group_cfg);
  if (not alloc_outcome) {
    return false;
  }
  // Copy the serv_cell_cfg configuration in cell_group_cfg the input serv_cell_cfg.
  ue_cell_cfg = cell_group_cfg.cells.at(SERVING_PCELL_IDX);
  return true;
}

void pucch_res_builder_test_helper::remove_ue_pucch_cfg(ue_cell_config& ue_cell_cfg)
{
  ocudu_assert(pucch_res_mgr.contains(ue_cell_cfg.serv_cell_cfg.cell_index),
               "PUCCH Resource Manager does not contain the cell index of the UE's serving cell configuration");
  odu::cell_group_config cell_group_cfg;
  cell_group_cfg.cells.emplace(SERVING_PCELL_IDX, ue_cell_cfg);
  pucch_res_mgr.dealloc_resources(cell_group_cfg);
}
