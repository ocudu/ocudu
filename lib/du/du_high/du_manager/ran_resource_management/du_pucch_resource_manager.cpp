// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "du_pucch_resource_manager.h"
#include "ocudu/ran/csi_report/csi_report_config_helpers.h"
#include "ocudu/ran/csi_report/csi_report_on_pucch_helpers.h"
#include "ocudu/ran/csi_rs/csi_meas_config.h"
#include "ocudu/ran/pucch/pucch_configuration.h"
#include "ocudu/ran/resource_allocation/ofdm_symbol_range.h"
#include "ocudu/ran/serv_cell_index.h"
#include "ocudu/scheduler/config/cell_bwp_res_config.h"
#include "ocudu/scheduler/config/csi_helper.h"
#include "ocudu/scheduler/config/pucch_resource_builder_params.h"
#include "ocudu/scheduler/config/pucch_resource_generator.h"
#include "ocudu/scheduler/config/sched_cell_config_helpers.h"
#include "ocudu/scheduler/config/serving_cell_config.h"
#include "ocudu/scheduler/config/serving_cell_config_builder.h"
#include "ocudu/scheduler/config/serving_cell_config_factory.h"
#include "ocudu/scheduler/config/ue_bwp_config.h"
#include <limits>
#include <numeric>
#include <utility>

using namespace ocudu;
using namespace odu;

du_pucch_resource_manager::du_pucch_resource_manager(unsigned max_pucch_grants_per_slot_) :
  // Leave 1 PUCCH grant for HARQ ACKs.
  max_pucch_grants_per_slot(max_pucch_grants_per_slot_ - 1U)
{
  ocudu_assert(max_pucch_grants_per_slot_ > 0, "At least one PUCCH grant per slot is required");
}

void du_pucch_resource_manager::add_cell(du_cell_index_t cell_idx, const ran_cell_config& cell_cfg)
{
  ocudu_assert(not cells.contains(cell_idx), "Cell index={} already configured", cell_idx);

  cell_resource_context cell_ctx;

  cell_ctx.cell_params  = cell_cfg;
  cell_ctx.cell_bwp_cfg = make_cell_bwp_res_config(cell_cfg);
  // TODO: remove these after we get rid of \c serving_cell_config.
  if (cell_ctx.cell_params.init_bwp.csi.has_value() and
      not cell_ctx.cell_params.init_bwp.csi->enable_aperiodic_report) {
    cell_ctx.default_csi_report_cfg =
        config_helpers::make_default_ue_cell_config(cell_cfg).serv_cell_cfg.csi_meas_cfg.value().csi_report_cfg_list[0];
  }

  // Compute SR period and SR configuration list.
  // TODO: Handle more than one SR period.
  cell_ctx.sr_period_slots = sr_periodicity_to_slot(cell_cfg.init_bwp.pucch.sr_period);
  for (unsigned res = 0; res < cell_ctx.cell_params.init_bwp.pucch.resources.nof_cell_sr_resources; ++res) {
    for (unsigned offset = 0; offset != cell_ctx.sr_period_slots; ++offset) {
      if (cell_cfg.tdd_cfg.has_value()) {
        const tdd_ul_dl_config_common& tdd_cfg = *cell_cfg.tdd_cfg;
        const unsigned slot_index = offset % (NOF_SUBFRAMES_PER_FRAME * get_nof_slots_per_subframe(tdd_cfg.ref_scs));
        if (get_active_tdd_ul_symbols(tdd_cfg, slot_index, cyclic_prefix::NORMAL).length() !=
            NOF_OFDM_SYM_PER_SLOT_NORMAL_CP) {
          // UL disabled for this slot.
          continue;
        }
      }
      cell_ctx.free_sr_configs.emplace_back(periodic_pucch_config{res, offset});
    }
  }

  // Compute CSI period and CSI configuration list (if periodic CSI reporting is configured).
  // TODO: Handle more than one CSI report period.
  if (cell_ctx.default_csi_report_cfg.has_value()) {
    cell_ctx.csi_period_slots = csi_resource_periodicity_to_uint(cell_ctx.cell_params.init_bwp.csi->csi_rs_period);
    // Compute the LCM of SR and CSI periods, as they might not be multiples of each other.
    cell_ctx.lcm_csi_sr_period = std::lcm(cell_ctx.sr_period_slots, cell_ctx.csi_period_slots);
    for (unsigned res = 0; res < cell_ctx.cell_params.init_bwp.pucch.resources.nof_cell_csi_resources; ++res) {
      for (unsigned offset = 0; offset != cell_ctx.csi_period_slots; ++offset) {
        if (cell_cfg.tdd_cfg.has_value()) {
          const tdd_ul_dl_config_common& tdd_cfg = *cell_cfg.tdd_cfg;
          const unsigned slot_index = offset % (NOF_SUBFRAMES_PER_FRAME * get_nof_slots_per_subframe(tdd_cfg.ref_scs));
          if (get_active_tdd_ul_symbols(tdd_cfg, slot_index, cyclic_prefix::NORMAL).length() !=
              NOF_OFDM_SYM_PER_SLOT_NORMAL_CP) {
            // UL disabled for this slot.
            continue;
          }
        }
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

void du_pucch_resource_manager::rem_cell(du_cell_index_t cell_idx)
{
  ocudu_assert(cells.contains(cell_idx), "Cell index={} has not been configured", cell_idx);

  cells.erase(cell_idx);
}

bool du_pucch_resource_manager::alloc_resources(cell_group_config& cell_grp_cfg)
{
  auto& cell_cfg         = cell_grp_cfg.cells.at(SERVING_PCELL_IDX);
  auto& serv_cell_cfg    = cell_cfg.serv_cell_cfg;
  auto& cell_ctx         = cells[serv_cell_cfg.cell_index];
  auto& free_sr_configs  = cell_ctx.free_sr_configs;
  auto& free_csi_configs = cell_ctx.free_csi_configs;

  // Verify where there are SR and CSI resources to allocate a new UE.
  if (free_sr_configs.empty() or (cell_ctx.default_csi_report_cfg.has_value() and free_csi_configs.empty())) {
    disable_pucch_cfg(serv_cell_cfg, cell_ctx);
    return false;
  }

  std::optional<periodic_pucch_config> sr_cfg;
  std::optional<periodic_pucch_config> csi_cfg;
  for (auto sr_cfg_it = free_sr_configs.begin(); sr_cfg_it != free_sr_configs.end(); ++sr_cfg_it) {
    // Skip this SR configuration if it exceeds the limit of PUCCH grants.
    if (offset_exceeds_limit(cell_ctx, sr_cfg_it->offset, false)) {
      continue;
    }

    if (not cell_ctx.default_csi_report_cfg.has_value()) {
      // No CSI report to allocate. Allocation successful.
      sr_cfg = *sr_cfg_it;
      free_sr_configs.erase(sr_cfg_it);
      break;
    }

    serv_cell_cfg.csi_meas_cfg->csi_report_cfg_list = {*cell_ctx.default_csi_report_cfg};
    const auto csi_report_cfg                       = create_csi_report_configuration(*serv_cell_cfg.csi_meas_cfg);
    const auto csi_report_size                      = get_csi_report_pucch_size(csi_report_cfg);
    auto       optimal_res_it =
        get_compatible_csi_cfg(cell_ctx, *sr_cfg_it, free_csi_configs, csi_report_size.part1_size.value());

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
    disable_pucch_cfg(serv_cell_cfg, cell_ctx);
    return false;
  }

  // Update the PUCCH grants-per-slot counters.
  for (auto offset :
       compute_periodic_uci_slot_offsets(cell_ctx, sr_cfg->offset, csi_cfg.has_value() ? csi_cfg->offset : 0)) {
    ocudu_assert(offset < static_cast<unsigned>(cell_ctx.periodic_pucchs_per_slot.size()),
                 "Index exceeds the size of the PUCCH grants vector");
    ++cell_ctx.periodic_pucchs_per_slot[offset];
  }

  // Update the BWP configuration for this UE with the allocated SR and CSI resources.
  auto& ul_bwp                = cell_cfg.init_bwp().ul;
  ul_bwp.pucch.res_set_cfg_id = pucch_resource_set_config_id(
      cell_ctx.ue_idx % cell_ctx.cell_params.init_bwp.pucch.resources.nof_cell_res_set_configs);
  ul_bwp.pucch.sr_res_id = pucch_sr_resource_id(sr_cfg->res);
  ul_bwp.pucch.sr_offset = sr_cfg->offset;
  if (csi_cfg.has_value()) {
    ul_bwp.periodic_csi_report.emplace(ue_periodic_csi_config{
        .pucch_res_id = pucch_csi_resource_id(csi_cfg->res),
        .offset       = csi_cfg->offset,
    });
  } else {
    ul_bwp.periodic_csi_report.reset();
  }

  // Fill the serving cell config with the PUCCH-related configuration.
  serv_cell_cfg.ul_config->init_ul_bwp.pucch_cfg =
      config_helpers::build_pucch_config(cell_ctx.cell_params, cell_ctx.cell_bwp_cfg.ul, cell_cfg.init_bwp());
  serv_cell_cfg.csi_meas_cfg =
      config_helpers::build_csi_meas_config(cell_ctx.cell_params, cell_ctx.cell_bwp_cfg.ul, cell_cfg.init_bwp());

  ++cell_ctx.ue_idx;
  return true;
}

void du_pucch_resource_manager::dealloc_resources(cell_group_config& cell_grp_cfg)
{
  auto&       serv_cell_cfg = cell_grp_cfg.cells.at(SERVING_PCELL_IDX).serv_cell_cfg;
  const auto& ul_bwp        = cell_grp_cfg.cells.at(SERVING_PCELL_IDX).init_bwp().ul;
  auto&       cell_ctx      = cells[serv_cell_cfg.cell_index];

  if (not serv_cell_cfg.ul_config->init_ul_bwp.pucch_cfg.has_value()) {
    return;
  }

  // Return SR resource/offset to the free list.
  cell_ctx.free_sr_configs.emplace_back(periodic_pucch_config{ul_bwp.pucch.sr_res_id.value(), ul_bwp.pucch.sr_offset});

  ocudu_assert(cell_ctx.default_csi_report_cfg.has_value() == ul_bwp.periodic_csi_report.has_value(),
               "Periodic CSI report configuration presence mismatch between cell and UE");
  // Return CSI resource/offset to the free list if periodic CSI is configured for this cell.
  if (cell_ctx.default_csi_report_cfg.has_value()) {
    cell_ctx.free_csi_configs.emplace_back(
        periodic_pucch_config{ul_bwp.periodic_csi_report->pucch_res_id.value(), ul_bwp.periodic_csi_report->offset});
  }

  // Remove the SR and CSI offsets from the PUCCH grants-per-slot counter.
  for (auto offset : compute_periodic_uci_slot_offsets(
           cell_ctx,
           ul_bwp.pucch.sr_offset,
           ul_bwp.periodic_csi_report.has_value() ? ul_bwp.periodic_csi_report->offset : 0)) {
    ocudu_assert(offset < cell_ctx.periodic_pucchs_per_slot.size(),
                 "Index exceeds the size of the PUCCH grants vector");
    ocudu_assert(cell_ctx.periodic_pucchs_per_slot[offset] != 0,
                 "Periodic PUCCH grants per slot counter is already at zero for offset={}",
                 offset);
    --cell_ctx.periodic_pucchs_per_slot[offset];
  }

  // Disable the PUCCH configuration in this UE. This makes sure the DU will exit this function immediately when it
  // gets called again for the same UE (upon destructor's call).
  disable_pucch_cfg(serv_cell_cfg, cell_ctx);
}

std::vector<du_pucch_resource_manager::periodic_pucch_config>::const_iterator
du_pucch_resource_manager::get_compatible_csi_cfg(const cell_resource_context&              cell_ctx,
                                                  const periodic_pucch_config&              sr_cfg,
                                                  const std::vector<periodic_pucch_config>& free_csi_list,
                                                  unsigned                                  csi_report_size) const
{
  const pucch_resource& sr_res = cell_ctx.cell_bwp_cfg.ul.pucch.get_ded(
      cell_ctx.cell_params.init_bwp.pucch.resources.sr_res_id(pucch_sr_resource_id(sr_cfg.res)));
  const unsigned max_pucch_payload = cell_ctx.cell_params.init_bwp.pucch.resources.max_payload_234();
  const auto     is_compatible     = [&](const periodic_pucch_config& csi_cfg) {
    // Ensure the max PUCCH grants limit is not exceeded.
    if (offset_exceeds_limit(cell_ctx, csi_cfg.offset, true)) {
      return false;
    }

    // Ensure the CSI and SR resources collide in OFDM symbols, so they are multiplexed.
    const pucch_resource& csi_res = cell_ctx.cell_bwp_cfg.ul.pucch.get_ded(
        cell_ctx.cell_params.init_bwp.pucch.resources.csi_res_id(pucch_csi_resource_id(csi_cfg.res)));
    if (not csi_res.syms.overlaps(sr_res.syms)) {
      return false;
    }

    // Ensure the SR and CSI reports can be sent together in the same PUCCH if their offsets collide.
    const unsigned nof_sr_bits        = csi_helper::are_sr_and_csi_pucchs_scheduled_together(
                                     cell_ctx.sr_period_slots, sr_cfg.offset, cell_ctx.csi_period_slots, csi_cfg.offset)
                                                    ? 1
                                                    : 0;
    const unsigned candidate_uci_bits = nof_sr_bits + csi_report_size;
    return candidate_uci_bits <= max_pucch_payload;
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

bool du_pucch_resource_manager::offset_exceeds_limit(const cell_resource_context& cell_ctx,
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

std::set<unsigned> du_pucch_resource_manager::compute_periodic_uci_slot_offsets(const cell_resource_context& cell_ctx,
                                                                                unsigned                     sr_offset,
                                                                                unsigned                     csi_offset)
{
  std::set<unsigned> slot_offsets;
  for (unsigned off = sr_offset; off < cell_ctx.lcm_csi_sr_period; off += cell_ctx.sr_period_slots) {
    slot_offsets.emplace(off);
  }

  if (cell_ctx.default_csi_report_cfg.has_value()) {
    for (unsigned off = csi_offset; off < cell_ctx.lcm_csi_sr_period; off += cell_ctx.csi_period_slots) {
      slot_offsets.emplace(off);
    }
  }
  return slot_offsets;
}

void du_pucch_resource_manager::disable_pucch_cfg(serving_cell_config&         serv_cell_cfg,
                                                  const cell_resource_context& cell_ctx)
{
  serv_cell_cfg.ul_config->init_ul_bwp.pucch_cfg.reset();
  if (cell_ctx.default_csi_report_cfg.has_value()) {
    serv_cell_cfg.csi_meas_cfg.value().csi_report_cfg_list.clear();
  }
}
