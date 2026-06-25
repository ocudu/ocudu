// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "pucch_allocator_helpers.h"
#include "ocudu/ran/pucch/pucch_configuration.h"
#include "ocudu/scheduler/config/pucch_default_resource.h"
#include "ocudu/scheduler/config/pucch_resource_builder_params.h"
#include "ocudu/support/ocudu_assert.h"

using namespace ocudu;

template <unsigned ResourceSetId>
const pucch_resource& pucch_helper::get_harq_resource(const ue_cell_configuration& ue_cfg, unsigned pri)
{
  const auto& res_params = ue_cfg.cell_cfg_common.params.init_bwp.pucch.resources;
  const auto& cell_res   = ue_cfg.cell_cfg_common.bwp_res[to_bwp_id(0)].ul().pucch;
  const auto& ue_bwp     = *ue_cfg.init_bwp().ul.ue_cfg();
  if (pri < res_params.res_set_size.value()) {
    return cell_res.get_ded(res_params.harq_res_id<ResourceSetId>(ue_bwp.pucch.res_set_cfg_id, pri));
  }
  ocudu_assert(res_params.format_01() == pucch_format::FORMAT_0 and res_params.format_234() == pucch_format::FORMAT_2,
               "Extra PUCCH HARQ-ACK resources are only present when using F0+F2");
  if (ResourceSetId == 0) {
    if (pri == res_params.res_set_size.value()) {
      return cell_res.get_ded(res_params.sr_res_id(ue_bwp.pucch.sr_res_id));
    }
    ocudu_assert(pri == res_params.res_set_size.value() + 1, "Invalid PRI={} for Resource Set 0", pri);
    return cell_res.get_ded(res_params.csi_f0_res_id(ue_bwp.periodic_csi_report->pucch_res_id));
  }

  if (pri == res_params.res_set_size.value()) {
    return cell_res.get_ded(res_params.sr_f2_res_id(ue_bwp.pucch.sr_res_id));
  }
  ocudu_assert(pri == res_params.res_set_size.value() + 1, "Invalid PRI={} for Resource Set 1", pri);
  return cell_res.get_ded(res_params.csi_res_id(ue_bwp.periodic_csi_report->pucch_res_id));
}

template const pucch_resource& pucch_helper::get_harq_resource<0>(const ue_cell_configuration&, unsigned);
template const pucch_resource& pucch_helper::get_harq_resource<1>(const ue_cell_configuration&, unsigned);

const pucch_resource& pucch_helper::get_sr_resource(const ue_cell_configuration& ue_cfg)
{
  const auto& res_params = ue_cfg.cell_cfg_common.params.init_bwp.pucch.resources;
  const auto& cell_res   = ue_cfg.cell_cfg_common.bwp_res[to_bwp_id(0)].ul().pucch;
  const auto& ue_bwp     = *ue_cfg.init_bwp().ul.ue_cfg();
  return cell_res.get_ded(res_params.sr_res_id(ue_bwp.pucch.sr_res_id));
}

const pucch_resource& pucch_helper::get_csi_resource(const ue_cell_configuration& ue_cfg)
{
  const auto& ue_bwp = *ue_cfg.init_bwp().ul.ue_cfg();
  ocudu_assert(ue_bwp.periodic_csi_report.has_value(),
               "CSI resource requested, but periodic CSI reporting is not configured for the UE");
  const auto& res_params = ue_cfg.cell_cfg_common.params.init_bwp.pucch.resources;
  const auto& cell_res   = ue_cfg.cell_cfg_common.bwp_res[to_bwp_id(0)].ul().pucch;
  return cell_res.get_ded(res_params.csi_res_id(ue_bwp.periodic_csi_report->pucch_res_id));
}

const pucch_resource& pucch_helper::get_common_resource(const cell_configuration&      cell_cfg,
                                                        const dci_context_information& dci_info,
                                                        unsigned                       d_pri)
{
  // Get N_CCE (nof_coreset_cces) and n_{CCE,0} (start_cce_idx), as per TS 38.213, Section 9.2.1.
  const unsigned nof_coreset_cces = dci_info.coreset_cfg->get_nof_cces();
  const unsigned start_cce_idx    = dci_info.cces.ncce;

  // r_PUCCH, as per Section 9.2.1, TS 38.213.
  const unsigned r_pucch = get_pucch_default_resource_index(start_cce_idx, nof_coreset_cces, d_pri);
  ocudu_assert(r_pucch < 16, "r_PUCCH must be less than 16");
  return cell_cfg.bwp_res[to_bwp_id(0)].ul().pucch.get_cmn(r_pucch);
}
