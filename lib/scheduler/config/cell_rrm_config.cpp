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

  const unsigned bwp_size_rbs     = cell_cfg.ul_cfg_common.init_ul_bwp.generic_params.crbs.length();
  const unsigned pucch_res_common = cell_cfg.ul_cfg_common.init_ul_bwp.pucch_cfg_common.value().pucch_resource_common;

  cell_pucch_rrm_config pucch;
  pucch.res.resources = config_helpers::generate_cell_pucch_res_list(
      params, bwp_size_rbs, config_helpers::get_dedicated_pucch_rb_start(params, pucch_res_common, bwp_size_rbs));

  for (unsigned cfg_id = 0; cfg_id != params.nof_cell_res_set_configs; ++cfg_id) {
    cell_pucch_resources::harq_resource_set_config sets;
    for (unsigned pri = 0; pri != params.res_set_size.value(); ++pri) {
      sets.set_0.push_back(params.harq_res_id<0>(pucch_resource_set_config_id(cfg_id), pri));
      sets.set_1.push_back(params.harq_res_id<1>(pucch_resource_set_config_id(cfg_id), pri));
    }
    pucch.res.harq.push_back(sets);
  }

  const bool using_02 = params.format_01() == pucch_format::FORMAT_0 and params.format_234() == pucch_format::FORMAT_2;
  for (unsigned sr_id = 0; sr_id != params.nof_cell_sr_resources; ++sr_id) {
    const pucch_sr_resource_id id{static_cast<uint8_t>(sr_id)};
    pucch.res.sr.push_back({params.sr_res_id(id), using_02 ? std::optional{params.sr_f2_res_id(id)} : std::nullopt});
  }
  for (unsigned csi_id = 0; csi_id != params.nof_cell_csi_resources; ++csi_id) {
    const pucch_csi_resource_id id{static_cast<uint8_t>(csi_id)};
    pucch.res.csi.push_back({params.csi_res_id(id), using_02 ? std::optional{params.csi_f0_res_id(id)} : std::nullopt});
  }

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
