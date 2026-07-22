// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "srs_resource_manager.h"
#include "ocudu/ran/srs/srs_configuration.h"
#include "ocudu/ran/tdd/tdd_ul_dl_config.h"
#include "ocudu/scheduler/config/ran_cell_config.h"

using namespace ocudu;

srs_resource_manager::srs_resource_manager(const ran_cell_config& cell_cfg) :
  srs_period(cell_cfg.init_bwp.srs_cfg.srs_period_prohib_time)
{
  ocudu_assert(cell_cfg.init_bwp.srs_cfg.srs_type_enabled == srs_type::periodic, "Cell must have periodic SRS enabled");

  for (uint16_t offset = 0, period_slots = static_cast<uint16_t>(srs_period); offset != period_slots; ++offset) {
    if (not cell_cfg.tdd_cfg.has_value() or has_active_tdd_ul_symbols(*cell_cfg.tdd_cfg, offset)) {
      free_offsets.push_back(offset);
    }
  }
  ocudu_assert(not free_offsets.empty(), "SRS period does not contain any UL slot");
}

bool srs_resource_manager::alloc_resources(ue_cell_config& ue_cell_cfg)
{
  if (free_offsets.empty()) {
    return false;
  }

  srs_config& srs_cfg = ue_cell_cfg.serv_cell_cfg.ul_config.value().init_ul_bwp.srs_cfg.value();
  srs_cfg.srs_res_set_list.front().res_type.emplace<srs_config::srs_resource_set::periodic_resource_type>();
  srs_config::srs_resource& res = srs_cfg.srs_res_list.front();
  res.res_type                  = srs_resource_type::periodic;

  const uint16_t offset = free_offsets.back();
  free_offsets.pop_back();
  res.periodicity_and_offset.emplace(srs_config::srs_periodicity_and_offset{srs_period, offset});
  return true;
}

void srs_resource_manager::dealloc_resources(ue_cell_config& ue_cell_cfg)
{
  srs_config&               srs_cfg = ue_cell_cfg.serv_cell_cfg.ul_config.value().init_ul_bwp.srs_cfg.value();
  srs_config::srs_resource& res     = srs_cfg.srs_res_list.front();
  if (not res.periodicity_and_offset.has_value()) {
    // No resource was allocated to this UE (e.g. dealloc_resources called twice for the same UE).
    return;
  }

  free_offsets.push_back(res.periodicity_and_offset->offset);
  res.periodicity_and_offset.reset();
  res.res_type = srs_resource_type::aperiodic;
}
