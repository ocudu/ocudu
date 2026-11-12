// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/scheduler/rrm/cell_rrm_config.h"
#include "ocudu/scheduler/config/pucch_resource_generator.h"
#include "ocudu/scheduler/config/ran_cell_config.h"
#include "ocudu/scheduler/config/time_domain_resource_helper.h"

using namespace ocudu;

static cell_pucch_rrm_config make_cell_pucch_rrm_config(const ran_cell_config& cell_cfg)
{
  const pucch_resource_builder_params& params = cell_cfg.init_bwp.pucch.resources;

  cell_pucch_rrm_config pucch;
  pucch.res = config_helpers::generate_cell_pucch_res_list(cell_cfg);

  pucch.sr_period         = cell_cfg.init_bwp.pucch.sr_period;
  pucch.max_code_rate_234 = params.max_code_rate_234();
  for (uint8_t k1 :
       time_domain_resource_helper::generate_k1_candidates(cell_cfg.tdd_cfg, cell_cfg.init_bwp.pucch.min_k1)) {
    pucch.dl_data_to_ul_ack.push_back(k1);
  }

  return pucch;
}

cell_rrm_config ocudu::make_cell_rrm_config(const ran_cell_config& cell_cfg)
{
  return cell_rrm_config{.pucch = make_cell_pucch_rrm_config(cell_cfg)};
}
