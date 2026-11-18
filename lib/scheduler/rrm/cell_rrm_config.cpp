// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/scheduler/rrm/cell_rrm_config.h"
#include "ocudu/scheduler/config/csi_helper.h"
#include "ocudu/scheduler/config/pucch_resource_generator.h"
#include "ocudu/scheduler/config/ran_cell_config.h"
#include "ocudu/scheduler/config/ran_cell_config_helper.h"
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

/// Makes the PDSCH time domain allocation list of the UEs with repetitions.
static std::vector<pdsch_time_domain_resource_allocation> make_pdsch_rep_td_alloc_list(const ran_cell_config& cell_cfg)
{
  // See TS 38.331, \c maxNrofDL-Allocations.
  static constexpr unsigned max_nof_dl_allocs = 16;

  const auto& common_list = cell_cfg.dl_cfg_common.init_dl_bwp.pdsch_common.pdsch_td_alloc_list;
  if (common_list.size() >= max_nof_dl_allocs) {
    return {};
  }

  // The repetition entry is the shortest common entry that ends at the last symbol of the slot, so it is for full DL
  // slots only.
  const unsigned symbols_per_slot = get_nsymb_per_slot(cell_cfg.dl_cfg_common.init_dl_bwp.generic_params.cp);
  std::optional<pdsch_time_domain_resource_allocation> base_alloc;
  for (const auto& alloc : common_list) {
    if (alloc.symbols.stop() == symbols_per_slot and
        (not base_alloc.has_value() or alloc.symbols.length() < base_alloc->symbols.length())) {
      base_alloc = alloc;
    }
  }
  if (not base_alloc.has_value()) {
    return {};
  }

  // The list is the common list, plus the repetition entry.
  std::vector<pdsch_time_domain_resource_allocation> list(common_list.begin(), common_list.end());
  list.push_back(*base_alloc);
  return list;
}

static cell_csi_rrm_config make_cell_csi_rrm_config(const ran_cell_config& cell_cfg)
{
  cell_csi_rrm_config csi;
  if (cell_cfg.init_bwp.csi.has_value()) {
    const csi_helper::csi_meas_config_builder_params csi_params =
        config_helpers::make_csi_meas_config_builder_params(cell_cfg);
    csi.zp_csi_rs_res_list = csi_helper::make_periodic_zp_csi_rs_resource_list(csi_params);
    csi.p_zp_csi_rs_res    = csi_helper::make_periodic_zp_csi_rs_resource_set(csi_params);
  }
  return csi;
}

static cell_pdsch_rrm_config make_cell_pdsch_rrm_config(const ran_cell_config& cell_cfg)
{
  const pdsch_builder_params& params = cell_cfg.init_bwp.pdsch;

  cell_pdsch_rrm_config pdsch;
  pdsch.x_ov_head                        = params.x_ov_head;
  pdsch.dmrs_type_a.additional_positions = params.additional_positions;
  pdsch.rbg_sz                           = params.rbg_sz;

  // According to TS 38.214 Section 5.1.2.3, the PRB bundle size must match the VRB-to-PRB mapping type.
  switch (params.interleaving_bundle_size) {
    case vrb_to_prb::mapping_type::non_interleaved:
      // > If $P'_{BWP,i}$ is determined as "wideband", the UE is not expected to be scheduled with non-contiguous
      // > PRBs and the UE may assume that the same precoding is applied to the allocated resource.
      pdsch.prb_bundle_size = pdsch_prb_bundle_size::wideband;
      break;
    case vrb_to_prb::mapping_type::interleaved_n2:
      // > When a UE is configured with nominal RBG size = 2 for bandwidth part i according to clause 5.1.2.2.1, or
      // > when a UE is configured with interleaving unit of 2 for VRB to PRB mapping provided by the higher layer
      // > parameter vrb-ToPRB-Interleaver given by PDSCH-Config for bandwidth part i, the UE is not expected to be
      // > configured with $P'_{BWP,i} = 4$.
      break;
    case vrb_to_prb::mapping_type::interleaved_n4:
      pdsch.prb_bundle_size = pdsch_prb_bundle_size::n4;
      break;
  }

  // The cell has no repetitions when the app does not enable them.
  if (cell_cfg.init_bwp.pdsch.max_nof_repetitions > 1) {
    pdsch.rep_td_alloc_list = make_pdsch_rep_td_alloc_list(cell_cfg);
  }
  return pdsch;
}

std::vector<pdsch_time_domain_resource_allocation>
ocudu::make_ue_pdsch_td_alloc_list(span<const pdsch_time_domain_resource_allocation> cell_rep_list,
                                   unsigned                                          nof_repetitions)
{
  if (nof_repetitions <= 1 or cell_rep_list.empty()) {
    return {};
  }

  std::vector<pdsch_time_domain_resource_allocation> list(cell_rep_list.begin(), cell_rep_list.end());
  list.back().rep_number = static_cast<uint8_t>(nof_repetitions);
  return list;
}

cell_rrm_config ocudu::make_cell_rrm_config(const ran_cell_config& cell_cfg)
{
  return cell_rrm_config{.pucch = make_cell_pucch_rrm_config(cell_cfg),
                         .pdsch = make_cell_pdsch_rrm_config(cell_cfg),
                         .csi   = make_cell_csi_rrm_config(cell_cfg),
                         .pusch = {.x_ov_head = cell_cfg.init_bwp.pusch.x_ov_head}};
}
