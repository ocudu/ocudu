// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "pdcch_resource_allocator_impl.h"
#include "../support/pdcch/pdcch_pdu_parameters.h"
#include "pdcch_slot_resource_allocator.h"
#include "ocudu/ran/pdcch/pdcch_candidates.h"

using namespace ocudu;

pdcch_resource_allocator_impl::pdcch_resource_allocator_impl(const cell_configuration& cell_cfg_) : cell_cfg(cell_cfg_)
{
  for (const search_space_configuration& ss : cell_cfg.params.dl_cfg_common.init_dl_bwp.pdcch_common.search_spaces) {
    pdcch_common_candidates.emplace(ss.get_id());
    const coreset_configuration& cs_cfg =
        (ss.get_coreset_id() == to_coreset_id(0))
            ? (*cell_cfg.params.dl_cfg_common.init_dl_bwp.pdcch_common.coreset0)
            : cell_cfg.params.dl_cfg_common.init_dl_bwp.pdcch_common.common_coreset.value();

    for (unsigned lidx = 0; lidx != NOF_AGGREGATION_LEVELS; ++lidx) {
      const aggregation_level aggr_lvl            = aggregation_index_to_level(lidx);
      const unsigned          nof_candidates      = ss.get_nof_candidates()[lidx];
      pdcch_candidate_info&   aggr_lvl_candidates = pdcch_common_candidates[ss.get_id()][lidx];
      aggr_lvl_candidates.candidates              = pdcch_candidates_common_ss_get_lowest_cce(
          pdcch_candidates_common_ss_configuration{aggr_lvl, nof_candidates, cs_cfg.get_nof_cces()});
    }
  }

  for (unsigned i = 0; i != SLOT_ALLOCATOR_RING_SIZE; ++i) {
    slot_records[i] = std::make_unique<pdcch_slot_allocator>();
  }
}

pdcch_resource_allocator_impl::~pdcch_resource_allocator_impl() = default;

void pdcch_resource_allocator_impl::slot_indication(slot_point sl_tx)
{
  ocudu_sanity_check(not last_sl_ind.valid() or sl_tx == last_sl_ind + 1, "Detected skipped slot");

  // Update Slot.
  last_sl_ind = sl_tx;

  // Clear old records.
  slot_records[(last_sl_ind - 1).to_uint() % slot_records.size()]->clear();
}

pdcch_dl_information* pdcch_resource_allocator_impl::alloc_dl_pdcch_common(cell_slot_resource_allocator& slot_alloc,
                                                                           rnti_t                        rnti,
                                                                           search_space_id               ss_id,
                                                                           aggregation_level             aggr_lvl)
{
  // Find Common BWP and CORESET configurations.
  const sched_search_space_config& ss_cfg = *cell_cfg.init_bwp.dl.pdcch().search_spaces()[ss_id];
  ocudu_sanity_check(ss_cfg.cfg().is_common_search_space(), "Invalid searchSpace type");

  return alloc_dl_pdcch_helper(slot_alloc,
                               rnti,
                               cell_cfg.init_bwp.dl.cfg(),
                               ss_cfg.cs(),
                               ss_cfg.cfg(),
                               aggr_lvl,
                               pdcch_common_candidates[ss_id][to_aggregation_level_index(aggr_lvl)].candidates);
}

pdcch_ul_information* pdcch_resource_allocator_impl::alloc_ul_pdcch_common(cell_slot_resource_allocator& slot_alloc,
                                                                           rnti_t                        rnti,
                                                                           search_space_id               ss_id,
                                                                           aggregation_level             aggr_lvl)
{
  // Find Common BWP and CORESET configurations.
  const sched_search_space_config& ss_cfg = *cell_cfg.init_bwp.dl.pdcch().search_spaces()[ss_id];

  return alloc_ul_pdcch_helper(slot_alloc,
                               rnti,
                               cell_cfg.init_bwp.ul.cfg(),
                               ss_cfg.cs(),
                               ss_cfg.cfg(),
                               aggr_lvl,
                               pdcch_common_candidates[ss_id][to_aggregation_level_index(aggr_lvl)].candidates);
}

pdcch_dl_information* pdcch_resource_allocator_impl::alloc_dl_pdcch_ue(cell_slot_resource_allocator& slot_alloc,
                                                                       rnti_t                        rnti,
                                                                       const ue_cell_configuration&  user,
                                                                       search_space_id               ss_id,
                                                                       aggregation_level             aggr_lvl)
{
  // Find Common or UE-specific BWP and CORESET configurations.
  const search_space_info& ss_cfg     = user.search_space(ss_id);
  const bwp_configuration& bwp_cfg    = ss_cfg.bwp->dl.cfg();
  span<const uint8_t>      candidates = ss_cfg.get_pdcch_candidates(aggr_lvl, slot_alloc.slot);

  return alloc_dl_pdcch_helper(slot_alloc, rnti, bwp_cfg, *ss_cfg.coreset, *ss_cfg.cfg, aggr_lvl, candidates);
}

pdcch_ul_information* pdcch_resource_allocator_impl::alloc_ul_pdcch_ue(cell_slot_resource_allocator& slot_alloc,
                                                                       rnti_t                        rnti,
                                                                       const ue_cell_configuration&  user,
                                                                       search_space_id               ss_id,
                                                                       aggregation_level             aggr_lvl)
{
  // Find Common or UE-specific BWP and CORESET configurations.
  const search_space_info&         ss_cfg     = user.search_space(ss_id);
  const bwp_configuration&         bwp_cfg    = ss_cfg.bwp->ul.cfg();
  span<const pdcch_candidate_type> candidates = ss_cfg.get_pdcch_candidates(aggr_lvl, slot_alloc.slot);

  return alloc_ul_pdcch_helper(slot_alloc, rnti, bwp_cfg, *ss_cfg.coreset, *ss_cfg.cfg, aggr_lvl, candidates);
}

pdcch_ul_information* pdcch_resource_allocator_impl::alloc_ul_pdcch_helper(cell_slot_resource_allocator&     slot_alloc,
                                                                           rnti_t                            rnti,
                                                                           const bwp_configuration&          bwp_cfg,
                                                                           const sched_coreset_config&       cs_cfg,
                                                                           const search_space_configuration& ss_cfg,
                                                                           aggregation_level                 aggr_lvl,
                                                                           span<const pdcch_candidate_type>  candidates)
{
  if (not pdcch_helper::is_pdcch_monitoring_active(slot_alloc.slot, ss_cfg)) {
    // PDCCH monitoring is not active in this slot.
    ++slot_alloc.result.failed_attempts.ul_pdcch;
    if (ss_cfg.is_common_search_space()) {
      ++slot_alloc.result.failed_attempts.common_ul_pdcch;
    }
    return nullptr;
  }

  // Create PDCCH list element.
  pdcch_ul_information& pdcch = slot_alloc.result.dl.ul_pdcchs.emplace_back();
  pdcch.ctx.bwp_cfg           = &bwp_cfg;
  pdcch.ctx.coreset_cfg       = &cs_cfg.cfg();
  pdcch.ctx.rnti              = rnti;
  pdcch.ctx.cces.ncce         = 0; // Note: Value is set during the PDCCH allocation step.
  // [Implementation-defined] We allocate the DCI on the SearchSpace starting from symbols 0.
  pdcch.ctx.starting_symbol   = 0;
  pdcch.ctx.cces.aggr_lvl     = aggr_lvl;
  pdcch.ctx.n_id_pdcch_data   = get_scrambling_n_ID(cell_cfg.params.pci, cs_cfg.cfg(), ss_cfg);
  pdcch.ctx.n_rnti_pdcch_data = get_scrambling_n_RNTI(rnti, cs_cfg.cfg(), ss_cfg);
  pdcch.ctx.n_id_pdcch_dmrs   = get_N_ID_dmrs(cell_cfg.params.pci, cs_cfg.cfg());
  pdcch.ctx.context.ss_id     = ss_cfg.get_id();
  pdcch.ctx.context.dci_format =
      (ss_cfg.is_common_search_space() ||
       (std::get<search_space_configuration::ue_specific_dci_format>(ss_cfg.get_monitored_dci_formats()) ==
        search_space_configuration::ue_specific_dci_format::f0_0_and_f1_0))
          ? "0_0"
          : "0_1";
  // Populate power offsets.
  if (not cell_cfg.nzp_csi_rs_list.empty() and cell_cfg.nzp_csi_rs_list.front().pwr_ctrl_offset_ss_db.has_value()) {
    // [Implementation-defined] It is assumed that same powerControlOffset and powerControlOffsetSS is configured in
    // NZP-CSI-RS-Resource across all resources.
    pdcch.ctx.tx_pwr.pwr_ctrl_offset_ss = cell_cfg.nzp_csi_rs_list.front().pwr_ctrl_offset_ss_db.value();
  }

  // Allocate a position for UL PDCCH in CORESET.
  pdcch_slot_allocator& pdcch_alloc = get_pdcch_slot_alloc(slot_alloc.slot);
  if (not pdcch_alloc.alloc_pdcch(pdcch.ctx, slot_alloc, cs_cfg, ss_cfg, candidates)) {
    slot_alloc.result.dl.ul_pdcchs.pop_back();
    ++slot_alloc.result.failed_attempts.ul_pdcch;
    if (ss_cfg.is_common_search_space()) {
      ++slot_alloc.result.failed_attempts.common_ul_pdcch;
    }
    return nullptr;
  }
  return &pdcch;
}

pdcch_dl_information* pdcch_resource_allocator_impl::alloc_dl_pdcch_helper(cell_slot_resource_allocator&     slot_alloc,
                                                                           rnti_t                            rnti,
                                                                           const bwp_configuration&          bwp_cfg,
                                                                           const sched_coreset_config&       cs_cfg,
                                                                           const search_space_configuration& ss_cfg,
                                                                           aggregation_level                 aggr_lvl,
                                                                           span<const pdcch_candidate_type>  candidates)
{
  if (not pdcch_helper::is_pdcch_monitoring_active(slot_alloc.slot, ss_cfg)) {
    // PDCCH monitoring is not active in this slot.
    ++slot_alloc.result.failed_attempts.dl_pdcch;
    if (ss_cfg.is_common_search_space()) {
      ++slot_alloc.result.failed_attempts.common_dl_pdcch;
    }
    return nullptr;
  }

  // Create PDCCH list element.
  pdcch_dl_information& pdcch = slot_alloc.result.dl.dl_pdcchs.emplace_back();
  pdcch.ctx.bwp_cfg           = &bwp_cfg;
  pdcch.ctx.coreset_cfg       = &cs_cfg.cfg();
  pdcch.ctx.rnti              = rnti;
  pdcch.ctx.cces.ncce         = 0;
  // [Implementation-defined] We allocate the DCI on the SearchSpace starting from symbols 0.
  pdcch.ctx.starting_symbol   = 0;
  pdcch.ctx.cces.aggr_lvl     = aggr_lvl;
  pdcch.ctx.n_id_pdcch_data   = get_scrambling_n_ID(cell_cfg.params.pci, cs_cfg.cfg(), ss_cfg);
  pdcch.ctx.n_rnti_pdcch_data = get_scrambling_n_RNTI(rnti, cs_cfg.cfg(), ss_cfg);
  pdcch.ctx.n_id_pdcch_dmrs   = get_N_ID_dmrs(cell_cfg.params.pci, cs_cfg.cfg());
  pdcch.ctx.context.ss_id     = ss_cfg.get_id();
  pdcch.ctx.context.dci_format =
      (ss_cfg.is_common_search_space() ||
       (std::get<search_space_configuration::ue_specific_dci_format>(ss_cfg.get_monitored_dci_formats()) ==
        search_space_configuration::ue_specific_dci_format::f0_0_and_f1_0))
          ? "1_0"
          : "1_1";
  // Populate power offsets.
  if (not cell_cfg.nzp_csi_rs_list.empty() and cell_cfg.nzp_csi_rs_list.front().pwr_ctrl_offset_ss_db.has_value()) {
    // [Implementation-defined] It is assumed that same powerControlOffset and powerControlOffsetSS is configured in
    // NZP-CSI-RS-Resource across all resources.
    pdcch.ctx.tx_pwr.pwr_ctrl_offset_ss = cell_cfg.nzp_csi_rs_list.front().pwr_ctrl_offset_ss_db.value();
  }

  // Allocate a position for DL PDCCH in CORESET.
  pdcch_slot_allocator& pdcch_alloc = get_pdcch_slot_alloc(slot_alloc.slot);
  if (not pdcch_alloc.alloc_pdcch(pdcch.ctx, slot_alloc, cs_cfg, ss_cfg, candidates)) {
    slot_alloc.result.dl.dl_pdcchs.pop_back();
    ++slot_alloc.result.failed_attempts.dl_pdcch;
    if (ss_cfg.is_common_search_space()) {
      ++slot_alloc.result.failed_attempts.common_dl_pdcch;
    }
    return nullptr;
  }
  return &pdcch;
}

bool pdcch_resource_allocator_impl::cancel_last_pdcch(cell_slot_resource_allocator& slot_alloc)
{
  pdcch_slot_allocator& pdcch_alloc = get_pdcch_slot_alloc(slot_alloc.slot);
  return pdcch_alloc.cancel_last_pdcch(slot_alloc);
}

void pdcch_resource_allocator_impl::stop()
{
  for (auto& sl_record : slot_records) {
    sl_record->clear();
  }
  last_sl_ind = {};
}

pdcch_slot_allocator& pdcch_resource_allocator_impl::get_pdcch_slot_alloc(slot_point sl)
{
  ocudu_sanity_check(sl < last_sl_ind + SLOT_ALLOCATOR_RING_SIZE, "PDCCH being allocated to far into the future");
  return *slot_records[sl.to_uint() % slot_records.size()];
}
