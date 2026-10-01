// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/scheduler/config/cell_bwp_res_config.h"
#include "ocudu/adt/format.h"
#include "ocudu/scheduler/config/bwp_builder_params.h"
#include "ocudu/scheduler/config/pucch_resource_generator.h"
#include "ocudu/scheduler/config/ran_cell_config.h"

using namespace ocudu;

static cell_dl_bwp_res_config make_cell_dl_bwp_res_config(const ran_cell_config& cell_cfg)
{
  cell_dl_bwp_res_config res;
  if (cell_cfg.init_bwp.pdcch_cfg.has_value()) {
    res.ded_pdcchs.push_back(*cell_cfg.init_bwp.pdcch_cfg);
  }
  return res;
}

cell_bwp_res_config ocudu::make_cell_bwp_res_config(const ran_cell_config& cell_cfg)
{
  const unsigned bwp_size_rbs     = cell_cfg.ul_cfg_common.init_ul_bwp.generic_params.crbs.length();
  const unsigned pucch_res_common = cell_cfg.ul_cfg_common.init_ul_bwp.pucch_cfg_common.value().pucch_resource_common;
  return cell_bwp_res_config{
      .dl = make_cell_dl_bwp_res_config(cell_cfg),
      .ul = {.pucch = {
                 .common = config_helpers::generate_cell_common_pucch_res_list(pucch_res_common, bwp_size_rbs),
                 .dedicated =
                     config_helpers::generate_cell_pucch_res_list(cell_cfg.init_bwp.pucch.resources, bwp_size_rbs),

             }}};
}
