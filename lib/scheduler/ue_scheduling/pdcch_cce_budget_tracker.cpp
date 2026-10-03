// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "pdcch_cce_budget_tracker.h"
#include "../cell/resource_grid.h"
#include "../config/cell_configuration.h"
#include "../config/time_domain_mapper.h"
#include "../support/pdcch/pdcch_mapping.h"

using namespace ocudu;

std::vector<pdcch_cce_budget_tracker::k2_list>
ocudu::compute_pusch_k2s_per_pdcch_slot(const cell_configuration& cell_cfg)
{
  const ul_time_domain_mapper&                      td_mapper     = cell_cfg.init_bwp.ul.td_mapper();
  span<const pusch_time_domain_resource_allocation> pusch_td_list = td_mapper.common_pusch_td_resources();
  const unsigned nof_slots = cell_cfg.is_tdd() ? nof_slots_per_tdd_period(*cell_cfg.params.tdd_cfg) : 1;
  std::vector<pdcch_cce_budget_tracker::k2_list> k2s_per_slot(nof_slots);
  for (unsigned sl_idx = 0; sl_idx != nof_slots; ++sl_idx) {
    auto& k2s = k2s_per_slot[sl_idx];
    for (uint8_t td_res_idx : td_mapper.common_pusch_td_res_indices(sl_idx)) {
      const uint8_t k2 = pusch_td_list[td_res_idx].k2;
      if (std::find(k2s.begin(), k2s.end(), k2) == k2s.end()) {
        k2s.push_back(k2);
      }
    }
    if (not cell_cfg.is_tdd() and k2s.size() > 1) {
      // [Implementation-defined] In FDD, the PDCCH CCEs are equally divided between DL and the PUSCH slot with the
      // smallest k2.
      k2s.resize(1);
    }
  }
  return k2s_per_slot;
}

std::vector<uint8_t> ocudu::compute_nof_pusch_slots_per_pdcch_slot(const cell_configuration& cell_cfg)
{
  std::vector<uint8_t> nof_pusch_slots;
  for (const auto& k2s : compute_pusch_k2s_per_pdcch_slot(cell_cfg)) {
    nof_pusch_slots.push_back(k2s.size());
  }
  return nof_pusch_slots;
}

/// \brief Finds the configuration of the given CORESET among the cell common and UE-dedicated configs.
static const coreset_configuration*
find_coreset_cfg(const cell_configuration& cell_cfg, const pdcch_config& ded_pdcch_cfg, coreset_id cs_id)
{
  const auto& common = cell_cfg.params.dl_cfg_common.init_dl_bwp.pdcch_common;
  if (common.coreset0.has_value() and common.coreset0->get_id() == cs_id) {
    return &common.coreset0.value();
  }
  if (common.common_coreset.has_value() and common.common_coreset->get_id() == cs_id) {
    return &common.common_coreset.value();
  }
  const auto it = std::find_if(ded_pdcch_cfg.coresets.begin(),
                               ded_pdcch_cfg.coresets.end(),
                               [cs_id](const coreset_configuration& cs) { return cs.get_id() == cs_id; });
  return it != ded_pdcch_cfg.coresets.end() ? &*it : nullptr;
}

/// Computes the CRBs spanned by the given CORESET.
static crb_bitmap get_coreset_crb_mask(const cell_configuration& cell_cfg, const coreset_configuration& cs_cfg)
{
  const bwp_configuration& bwp_cfg = cell_cfg.params.dl_cfg_common.init_dl_bwp.generic_params;
  crb_bitmap               crbs(MAX_NOF_PRBS);
  for (unsigned ncce = 0, nof_cces = cs_cfg.get_nof_cces(); ncce != nof_cces; ++ncce) {
    for (uint16_t prb :
         pdcch_helper::cce_to_prb_mapping(bwp_cfg, cs_cfg, cell_cfg.params.pci, aggregation_level::n1, ncce)) {
      crbs.set(prb_to_crb(bwp_cfg.crbs, prb));
    }
  }
  return crbs;
}

pdcch_cce_budget_tracker::pdcch_cce_budget_tracker(const cell_resource_allocator& cell_alloc_) :
  cell_alloc(cell_alloc_), k2s_per_pdcch_slot(compute_pusch_k2s_per_pdcch_slot(cell_alloc.cfg))
{
  std::bitset<MAX_NOF_CORESETS> tracked_coresets;
  for (const pdcch_config& ded_pdcch_cfg : cell_alloc.cfg.bwp_res[to_bwp_id(0)].dl().ded_pdcchs) {
    for (const search_space_configuration& ss : ded_pdcch_cfg.search_spaces) {
      const coreset_id cs_id = ss.get_coreset_id();
      if (tracked_coresets.test(cs_id)) {
        continue;
      }
      const coreset_configuration* cs_cfg = find_coreset_cfg(cell_alloc.cfg, ded_pdcch_cfg, cs_id);
      if (cs_cfg == nullptr) {
        continue;
      }
      if (tracked_coreset_res.full()) {
        continue;
      }
      tracked_coresets.set(cs_id);
      tracked_coreset_res.push_back(
          coreset_resources{get_coreset_crb_mask(cell_alloc.cfg, *cs_cfg), static_cast<uint8_t>(cs_cfg->duration())});
      total_cces += cs_cfg->get_nof_cces();
    }
  }
}

void pdcch_cce_budget_tracker::slot_indication(slot_point pdcch_slot_)
{
  pdcch_slot = pdcch_slot_;
}

const pdcch_cce_budget_tracker::k2_list& pdcch_cce_budget_tracker::current_k2s() const
{
  return k2s_per_pdcch_slot[pdcch_slot.count() % k2s_per_pdcch_slot.size()];
}

unsigned pdcch_cce_budget_tracker::share() const
{
  return total_cces / (current_k2s().size() + 1);
}

unsigned pdcch_cce_budget_tracker::nof_reserved_ul_cces(slot_point pusch_slot) const
{
  // [Implementation-defined] A share is reserved for each reachable PUSCH slot yet to be scheduled. Given that PUSCH
  // slots are scheduled in increasing order, these are the ones after the given PUSCH slot.
  unsigned nof_pending = 0;
  for (uint8_t k2 : current_k2s()) {
    const slot_point sl = pdcch_slot + k2 + cell_alloc.cfg.ntn_cs_koffset;
    if (not pusch_slot.valid() or sl > pusch_slot) {
      ++nof_pending;
    }
  }
  return nof_pending * share();
}

unsigned pdcch_cce_budget_tracker::nof_free_cces() const
{
  // Note: The resource grid also accounts for PDCCHs of other CORESETs overlapping the tracked ones.
  static constexpr unsigned NOF_REGS_PER_CCE = 6;
  const auto&               dl_grid          = cell_alloc[0].dl_res_grid;
  const bwp_configuration&  bwp_cfg          = cell_alloc.cfg.params.dl_cfg_common.init_dl_bwp.generic_params;
  unsigned                  free_regs        = 0;
  for (const coreset_resources& cs : tracked_coreset_res) {
    crb_bitmap used = dl_grid.used_crbs(bwp_cfg.scs, bwp_cfg.crbs, {0, cs.duration});
    used.resize(cs.crbs.size());
    used &= cs.crbs;
    free_regs += (cs.crbs.count() - used.count()) * cs.duration;
  }
  return free_regs / NOF_REGS_PER_CCE;
}

unsigned pdcch_cce_budget_tracker::remaining_dl_cces() const
{
  const unsigned free_cces = nof_free_cces();
  return free_cces - std::min(free_cces, nof_reserved_ul_cces(slot_point{}));
}

unsigned pdcch_cce_budget_tracker::remaining_ul_cces(slot_point pusch_slot) const
{
  const unsigned free_cces = nof_free_cces();
  return free_cces - std::min(free_cces, nof_reserved_ul_cces(pusch_slot));
}
