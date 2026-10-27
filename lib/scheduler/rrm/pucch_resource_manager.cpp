// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/scheduler/rrm/pucch_resource_manager.h"
#include "ocudu/adt/format.h"
#include "ocudu/ran/csi_report/csi_report_on_pucch_helpers.h"
#include "ocudu/ran/csi_rs/csi_meas_config.h"
#include "ocudu/ran/pucch/pucch_configuration.h"
#include "ocudu/ran/pucch/pucch_uci_bits.h"
#include "ocudu/ran/resource_allocation/ofdm_symbol_range.h"
#include "ocudu/ran/tdd/tdd_ul_dl_config.h"
#include "ocudu/scheduler/config/cell_bwp_res_config.h"
#include "ocudu/scheduler/config/csi_helper.h"
#include "ocudu/scheduler/config/pucch_resource_builder_params.h"
#include "ocudu/scheduler/config/pucch_resource_generator.h"
#include "ocudu/scheduler/config/sched_cell_config_helpers.h"
#include "ocudu/scheduler/config/serving_cell_config.h"
#include "ocudu/scheduler/config/serving_cell_config_builder.h"
#include "ocudu/scheduler/config/ue_bwp_config.h"
#include "ocudu/scheduler/rrm/du_srs_resource.h"
#include <algorithm>
#include <limits>
#include <numeric>
#include <utility>

using namespace ocudu;

/// Lists the slot offsets where a periodic PUCCH with the given symbols fits the UL symbols and does not overlap the
/// SRS.
static std::vector<unsigned>
get_periodic_pucch_slot_offsets(const ran_cell_config& cell_cfg, unsigned period_slots, ofdm_symbol_range pucch_syms)
{
  const cyclic_prefix   cp      = cell_cfg.ul_cfg_common.init_ul_bwp.generic_params.cp;
  std::vector<unsigned> offsets = get_ul_slot_offsets(cell_cfg.tdd_cfg, period_slots, pucch_syms, cp);
  if (not cell_cfg.tdd_cfg.has_value() or cell_cfg.init_bwp.srs_cfg.srs_type_enabled == srs_type::disabled) {
    return offsets;
  }

  // In a partially-UL slot, the SRS can use more symbols than the SRS max_nof_symbols. The PUCCH symbol budget leaves
  // only the last max_nof_symbols symbols free.
  const tdd_ul_dl_config_common& tdd_cfg      = *cell_cfg.tdd_cfg;
  const auto                     overlaps_srs = [&](unsigned offset) {
    return is_tdd_partial_ul_slot(tdd_cfg, offset) and
           pucch_syms.overlaps(get_srs_symbols_in_partial_ul_slot(get_active_tdd_ul_symbols(tdd_cfg, offset, cp),
                                                                  cell_cfg.init_bwp.srs_cfg.nof_symbols));
  };
  offsets.erase(std::remove_if(offsets.begin(), offsets.end(), overlaps_srs), offsets.end());
  return offsets;
}

pucch_resource_manager::pucch_resource_manager(unsigned max_pucch_grants_per_slot_) :
  // Leave 1 PUCCH grant for HARQ ACKs.
  max_pucch_grants_per_slot(max_pucch_grants_per_slot_ - 1U)
{
  ocudu_assert(max_pucch_grants_per_slot_ > 0, "At least one PUCCH grant per slot is required");
}

void pucch_resource_manager::add_cell(du_cell_index_t        cell_idx,
                                      const ran_cell_config& cell_cfg,
                                      const cell_rrm_config& rrm_cfg)
{
  ocudu_assert(not cells.contains(cell_idx), "Cell index={} already configured", cell_idx);

  cell_resource_context cell_ctx;

  cell_ctx.cell_params = cell_cfg;
  cell_ctx.pucch_res   = &rrm_cfg.pucch;
  {
    // TODO: The CSI report is the only part of the cell BWP resources that the manager reads.
    const cell_bwp_res_config cell_bwp_cfg = make_cell_bwp_res_config(cell_cfg, rrm_cfg);
    if (cell_bwp_cfg.dl.csi.has_value() and cell_bwp_cfg.dl.csi->pucch_report() != nullptr) {
      cell_ctx.csi_report = *cell_bwp_cfg.dl.csi->pucch_report();
    }
  }

  // Compute SR period and SR configuration list.
  // TODO: Handle more than one SR period.
  cell_ctx.sr_period_slots = sr_periodicity_to_slot(cell_cfg.init_bwp.pucch.sr_period);
  for (unsigned res = 0; res != cell_ctx.pucch_res->res.sr.size(); ++res) {
    const pucch_resource& sr_res =
        cell_ctx.pucch_res->res.get(cell_ctx.pucch_res->res.get_sr(pucch_sr_resource_id(res)).res_id);
    for (unsigned offset : get_periodic_pucch_slot_offsets(cell_cfg, cell_ctx.sr_period_slots, sr_res.syms)) {
      cell_ctx.free_sr_configs.emplace_back(periodic_pucch_config{res, offset});
    }
  }

  // Compute CSI period and CSI configuration list (if periodic CSI reporting is configured).
  // TODO: Handle more than one CSI report period.
  if (cell_ctx.pucch_csi_report() != nullptr) {
    cell_ctx.csi_period_slots = to_underlying(cell_ctx.cell_params.init_bwp.csi->csi_rs_period);
    // Compute the LCM of SR and CSI periods, as they might not be multiples of each other.
    cell_ctx.lcm_csi_sr_period = std::lcm(cell_ctx.sr_period_slots, cell_ctx.csi_period_slots);
    for (unsigned res = 0; res != cell_ctx.pucch_res->res.csi.size(); ++res) {
      const pucch_resource& csi_res =
          cell_ctx.pucch_res->res.get(cell_ctx.pucch_res->res.get_csi(pucch_csi_resource_id(res)).res_id);
      for (unsigned offset : get_periodic_pucch_slot_offsets(cell_cfg, cell_ctx.csi_period_slots, csi_res.syms)) {
        cell_ctx.free_csi_configs.emplace_back(periodic_pucch_config{res, offset});
      }
    }
  } else {
    cell_ctx.lcm_csi_sr_period = cell_ctx.sr_period_slots;
    cell_ctx.csi_period_slots  = 0;
  }

  cell_ctx.periodic_pucchs_per_slot.resize(cell_ctx.lcm_csi_sr_period, 0);
  cells.emplace(cell_idx, std::move(cell_ctx));
}

void pucch_resource_manager::rem_cell(du_cell_index_t cell_idx)
{
  ocudu_assert(cells.contains(cell_idx), "Cell index={} has not been configured", cell_idx);

  cells.erase(cell_idx);
}

bool pucch_resource_manager::alloc_resources(ue_cell_config& cell_cfg)
{
  auto& serv_cell_cfg    = cell_cfg.serv_cell_cfg;
  auto& cell_ctx         = cells[serv_cell_cfg.cell_index];
  auto& free_sr_configs  = cell_ctx.free_sr_configs;
  auto& free_csi_configs = cell_ctx.free_csi_configs;

  // Verify where there are SR and CSI resources to allocate a new UE.
  const csi_report_configuration* csi_report = cell_ctx.pucch_csi_report();
  if (free_sr_configs.empty() or (csi_report != nullptr and free_csi_configs.empty())) {
    return false;
  }

  // Size of the periodic CSI report of the UE.
  std::optional<unsigned> csi_report_nof_bits;
  if (csi_report != nullptr) {
    csi_report_nof_bits = get_csi_report_pucch_size(*csi_report).part1_size.value();
  }

  std::optional<periodic_pucch_config> sr_cfg;
  std::optional<periodic_pucch_config> csi_cfg;
  for (auto sr_cfg_it = free_sr_configs.begin(); sr_cfg_it != free_sr_configs.end(); ++sr_cfg_it) {
    // Skip this SR configuration if it exceeds the limit of PUCCH grants.
    if (offset_exceeds_limit(cell_ctx, sr_cfg_it->offset, false)) {
      continue;
    }

    if (not csi_report_nof_bits.has_value()) {
      // No CSI report to allocate. Allocation successful.
      sr_cfg = *sr_cfg_it;
      free_sr_configs.erase(sr_cfg_it);
      break;
    }

    auto optimal_res_it = get_compatible_csi_cfg(cell_ctx, *sr_cfg_it, free_csi_configs, *csi_report_nof_bits);

    if (optimal_res_it != free_csi_configs.end()) {
      // Allocation successful. Remove SR and CSI resources assigned to this UE from the lists of free resources.
      sr_cfg = *sr_cfg_it;
      free_sr_configs.erase(sr_cfg_it);
      csi_cfg = *optimal_res_it;
      free_csi_configs.erase(optimal_res_it);
      break;
    }
  }

  if (not sr_cfg.has_value()) {
    // No suitable configuration found. Allocation failed.
    return false;
  }

  // Update the PUCCH grants-per-slot counters.
  for (auto offset :
       compute_periodic_uci_slot_offsets(cell_ctx, sr_cfg->offset, csi_cfg.has_value() ? csi_cfg->offset : 0)) {
    ocudu_assert(offset < static_cast<unsigned>(cell_ctx.periodic_pucchs_per_slot.size()),
                 "Index exceeds the size of the PUCCH grants vector");
    ++cell_ctx.periodic_pucchs_per_slot[offset];
  }

  // Record the allocated SR and CSI resources.
  ue_pucch_rrm_config& pucch_rrm = cell_cfg.rrm.pucch;
  pucch_rrm.res_set_cfg_id       = pucch_resource_set_config_id(cell_ctx.ue_idx % cell_ctx.pucch_res->res.harq.size());
  pucch_rrm.sr_res_id            = pucch_sr_resource_id(sr_cfg->res);
  pucch_rrm.sr_offset            = sr_cfg->offset;
  auto& csi_rrm                  = cell_cfg.rrm.csi;
  if (csi_cfg.has_value()) {
    csi_rrm.periodic_report.emplace(
        ue_periodic_csi_rrm_config{.res_id = pucch_csi_resource_id(csi_cfg->res), .offset = csi_cfg->offset});
  } else {
    csi_rrm.periodic_report.reset();
  }

  // Fill the serving cell config with the CSI-related configuration.
  serv_cell_cfg.csi_meas_cfg = config_helpers::build_csi_meas_config(cell_ctx.cell_params, cell_cfg.rrm);

  // Disable PUCCH repetition until \c update_resources is called with the UE's reported capabilities.
  apply_rep_factor_capabilities(cell_cfg, std::nullopt);

  ++cell_ctx.ue_idx;
  return true;
}

void pucch_resource_manager::dealloc_resources(const ue_cell_config& cell_cfg)
{
  const auto& pucch_rrm = cell_cfg.rrm.pucch;
  const auto& csi_rrm   = cell_cfg.rrm.csi;
  auto&       cell_ctx  = cells[cell_cfg.serv_cell_cfg.cell_index];

  // Return SR resource/offset to the free list.
  cell_ctx.free_sr_configs.emplace_back(periodic_pucch_config{pucch_rrm.sr_res_id.value(), pucch_rrm.sr_offset});

  ocudu_assert((cell_ctx.pucch_csi_report() != nullptr) == csi_rrm.periodic_report.has_value(),
               "Periodic CSI report configuration presence mismatch between cell and UE");
  // Return CSI resource/offset to the free list if periodic CSI is configured for this cell.
  if (cell_ctx.pucch_csi_report() != nullptr) {
    cell_ctx.free_csi_configs.emplace_back(
        periodic_pucch_config{csi_rrm.periodic_report->res_id.value(), csi_rrm.periodic_report->offset});
  }

  // Remove the SR and CSI offsets from the PUCCH grants-per-slot counter.
  for (auto offset : compute_periodic_uci_slot_offsets(
           cell_ctx, pucch_rrm.sr_offset, csi_rrm.periodic_report.has_value() ? csi_rrm.periodic_report->offset : 0)) {
    ocudu_assert(offset < cell_ctx.periodic_pucchs_per_slot.size(),
                 "Index exceeds the size of the PUCCH grants vector");
    ocudu_assert(cell_ctx.periodic_pucchs_per_slot[offset] != 0,
                 "Periodic PUCCH grants per slot counter is already at zero for offset={}",
                 offset);
    --cell_ctx.periodic_pucchs_per_slot[offset];
  }
}

void pucch_resource_manager::update_resources(ue_cell_config& cell_cfg, const ue_capability_summary& ue_caps)
{
  apply_rep_factor_capabilities(cell_cfg, ue_caps);
}

void pucch_resource_manager::apply_rep_factor_capabilities(ue_cell_config&                             cell_cfg,
                                                           const std::optional<ue_capability_summary>& ue_caps)
{
  const cell_resource_context& cell_ctx = cells[cell_cfg.serv_cell_cfg.cell_index];

  bool f1_3_4_supported      = false;
  bool f0_2_supported        = false;
  bool dynamic_rep_supported = false;
  if (ue_caps.has_value()) {
    const nr_band band = cell_ctx.cell_params.ul_carrier.band;

    f1_3_4_supported   = ue_caps->pucch_repeat_f1_3_4_supported;
    const auto band_it = ue_caps->bands.find(band);
    f0_2_supported     = band_it != ue_caps->bands.end() and band_it->second.pucch_repeat_f0_2_r17_supported;
    // The UE must indicate slotBasedDynamicPUCCH-Rep-r17 to support the dynamic PUCCH repetition indicated via
    // pucch-RepetitionNrofSlots-r17, in addition to the format-specific repetition capability.
    dynamic_rep_supported = ue_caps->slot_based_dyn_pucch_rep_r17_supported;
  }

  // Enable the repetition for the formats that the UE supports.
  cell_cfg.rrm.pucch.rep_f1_3_4_enabled = dynamic_rep_supported and f1_3_4_supported;
  cell_cfg.rrm.pucch.rep_f0_2_enabled   = dynamic_rep_supported and f0_2_supported;
}

std::vector<pucch_resource_manager::periodic_pucch_config>::const_iterator
pucch_resource_manager::get_compatible_csi_cfg(const cell_resource_context&              cell_ctx,
                                               const periodic_pucch_config&              sr_cfg,
                                               const std::vector<periodic_pucch_config>& free_csi_list,
                                               unsigned                                  csi_report_size) const
{
  const pucch_resource& sr_res =
      cell_ctx.pucch_res->res.get(cell_ctx.pucch_res->res.get_sr(pucch_sr_resource_id(sr_cfg.res)).res_id);
  const unsigned max_pucch_payload = cell_ctx.cell_params.init_bwp.pucch.resources.max_payload_234();
  const auto     is_compatible     = [&](const periodic_pucch_config& csi_cfg) {
    // Ensure the max PUCCH grants limit is not exceeded.
    if (offset_exceeds_limit(cell_ctx, csi_cfg.offset, true)) {
      return false;
    }

    if (not csi_helper::are_sr_and_csi_pucchs_scheduled_together(
            cell_ctx.sr_period_slots, sr_cfg.offset, cell_ctx.csi_period_slots, csi_cfg.offset)) {
      return csi_report_size <= max_pucch_payload;
    }

    // Ensure the CSI and SR resources collide in OFDM symbols, so they are multiplexed.
    const pucch_resource& csi_res =
        cell_ctx.pucch_res->res.get(cell_ctx.pucch_res->res.get_csi(pucch_csi_resource_id(csi_cfg.res)).res_id);
    if (not csi_res.syms.overlaps(sr_res.syms)) {
      return false;
    }

    // Ensure the SR and CSI reports can be sent together in the same PUCCH.
    static constexpr unsigned sr_payload_bits = to_underlying(sr_nof_bits::one);
    return sr_payload_bits + csi_report_size <= max_pucch_payload;
  };

  // [Implementation-defined] CSI resource and report periods are the same.
  // Note: We use the latest meas CSI-RS offset as the reference to derive the CSI report offset. We want all CSI-RS
  // resources to be considered for the report.
  const unsigned csi_rs_period   = cell_ctx.csi_period_slots;
  const unsigned csi_rs_offset   = *std::max_element(cell_ctx.cell_params.init_bwp.csi->meas_csi_slot_offsets.begin(),
                                                   cell_ctx.cell_params.init_bwp.csi->meas_csi_slot_offsets.end());
  const auto     weight_function = [&](const periodic_pucch_config& candidate_csi_cfg) -> unsigned {
    // [Implementation-defined] Given that it takes some time for a UE to process a CSI-RS and integrate its estimate
    // in the following CSI report, we consider a minimum slot distance before which CSI report slot offsets should be
    // avoided.
    static constexpr unsigned MINIMUM_CSI_RS_REPORT_DISTANCE = 4;

    // Prioritize offsets equal or after the \c csi_rs_slot_offset + MINIMUM_CSI_RS_REPORT_DISTANCE.
    unsigned weight =
        (csi_rs_period + candidate_csi_cfg.offset - csi_rs_offset - MINIMUM_CSI_RS_REPORT_DISTANCE) % csi_rs_period;

    // We increase the weight if the CSI report offset collides with an SR slot offset.
    if (csi_helper::are_sr_and_csi_pucchs_scheduled_together(
            cell_ctx.sr_period_slots, sr_cfg.offset, cell_ctx.csi_period_slots, candidate_csi_cfg.offset)) {
      weight += csi_rs_period;
    }

    return weight;
  };

  auto     best        = free_csi_list.end();
  unsigned best_weight = std::numeric_limits<unsigned>::max();
  for (auto csi_cfg = free_csi_list.begin(); csi_cfg != free_csi_list.end(); ++csi_cfg) {
    if (not is_compatible(*csi_cfg)) {
      continue;
    }

    const unsigned weight = weight_function(*csi_cfg);
    if (weight < best_weight) {
      best        = csi_cfg;
      best_weight = weight;
    }
  }
  return best;
}

bool pucch_resource_manager::offset_exceeds_limit(const cell_resource_context& cell_ctx,
                                                  unsigned                     offset,
                                                  bool                         csi) const
{
  for (unsigned off = offset, period = csi ? cell_ctx.csi_period_slots : cell_ctx.sr_period_slots;
       off < cell_ctx.lcm_csi_sr_period;
       off += period) {
    ocudu_assert(off < static_cast<unsigned>(cell_ctx.periodic_pucchs_per_slot.size()),
                 "Index exceeds the size of the PUCCH grants vector");
    if (cell_ctx.periodic_pucchs_per_slot[off] >= max_pucch_grants_per_slot) {
      return true;
    }
  }
  return false;
}

std::set<unsigned> pucch_resource_manager::compute_periodic_uci_slot_offsets(const cell_resource_context& cell_ctx,
                                                                             unsigned                     sr_offset,
                                                                             unsigned                     csi_offset)
{
  std::set<unsigned> slot_offsets;
  for (unsigned off = sr_offset; off < cell_ctx.lcm_csi_sr_period; off += cell_ctx.sr_period_slots) {
    slot_offsets.emplace(off);
  }

  if (cell_ctx.pucch_csi_report() != nullptr) {
    for (unsigned off = csi_offset; off < cell_ctx.lcm_csi_sr_period; off += cell_ctx.csi_period_slots) {
      slot_offsets.emplace(off);
    }
  }
  return slot_offsets;
}
