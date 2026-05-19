// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/scheduler/config/cell_bwp_res_config.h"
#include "../support/pucch/pucch_default_resource.h"
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

/// Builds the 16 PUCCH resources of the default common table (TS 38.213 Table 9.2.1-1) for the initial UL BWP.
static std::vector<pucch_resource> make_common_pucch_resources(const ran_cell_config& cell_cfg)
{
  if (not cell_cfg.ul_cfg_common.init_ul_bwp.pucch_cfg_common.has_value()) {
    return {};
  }
  const unsigned               n_bwp_size  = cell_cfg.ul_cfg_common.init_ul_bwp.generic_params.crbs.length();
  const unsigned               row_index   = cell_cfg.ul_cfg_common.init_ul_bwp.pucch_cfg_common->pucch_resource_common;
  const pucch_default_resource default_res = get_pucch_default_resource(row_index, n_bwp_size);
  const unsigned               nof_cs      = default_res.cs_indexes.size();

  static constexpr unsigned   nof_common_resources = 16;
  std::vector<pucch_resource> out;
  out.reserve(nof_common_resources);
  for (unsigned r_pucch = 0; r_pucch != nof_common_resources; ++r_pucch) {
    const auto     prbs = get_pucch_default_prb_index(r_pucch, default_res.rb_bwp_offset, nof_cs, n_bwp_size);
    const uint8_t  cs   = default_res.cs_indexes[get_pucch_default_cyclic_shift(r_pucch, nof_cs)];
    pucch_resource res{};
    res.res_id         = pucch_res_id_t::make_cmn(r_pucch);
    res.starting_prb   = prbs.first;
    res.second_hop_prb = prbs.second;
    res.syms           = ofdm_symbol_range::start_and_len(default_res.first_symbol_index, default_res.nof_symbols);
    res.rep_factor     = pucch_repetition_factor::n1;
    switch (default_res.format) {
      case pucch_format::FORMAT_0:
        res.format_params = pucch_resource::f0_config{.initial_cyclic_shift = cs};
        break;
      case pucch_format::FORMAT_1:
        // TS 38.213 Section 9.2.1: OCC index 0 is used for the common PUCCH resources.
        res.format_params = pucch_resource::f1_config{.initial_cyclic_shift = cs, .time_domain_occ = 0};
        break;
      default:
        // The default common table only contains Format 0 and Format 1 entries.
        ocudu_assertion_failure("Unexpected PUCCH format in default common table.");
        break;
    }
    out.push_back(res);
  }
  return out;
}

cell_bwp_res_config ocudu::make_cell_bwp_res_config(const ran_cell_config& cell_cfg)
{
  return cell_bwp_res_config{.dl = make_cell_dl_bwp_res_config(cell_cfg),
                             .ul = {.pucch = {.resources = config_helpers::generate_cell_pucch_res_list(
                                                  cell_cfg.init_bwp.pucch.resources,
                                                  cell_cfg.ul_cfg_common.init_ul_bwp.generic_params.crbs.length()),
                                              .common_resources = make_common_pucch_resources(cell_cfg)}}};
}
