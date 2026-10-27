// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/scheduler/rrm/cell_rrm_config.h"
#include "ocudu/scheduler/config/pucch_resource_generator.h"
#include "ocudu/scheduler/config/ran_cell_config.h"

using namespace ocudu;

cell_rrm_config ocudu::make_cell_rrm_config(const ran_cell_config& cell_cfg)
{
  const unsigned bwp_size_rbs     = cell_cfg.ul_cfg_common.init_ul_bwp.generic_params.crbs.length();
  const unsigned pucch_res_common = cell_cfg.ul_cfg_common.init_ul_bwp.pucch_cfg_common.value().pucch_resource_common;
  return cell_rrm_config{
      .pucch = {.resources = config_helpers::generate_cell_pucch_res_list(
                    cell_cfg.init_bwp.pucch.resources,
                    bwp_size_rbs,
                    config_helpers::get_dedicated_pucch_rb_start(
                        cell_cfg.init_bwp.pucch.resources, pucch_res_common, bwp_size_rbs))},
  };
}
