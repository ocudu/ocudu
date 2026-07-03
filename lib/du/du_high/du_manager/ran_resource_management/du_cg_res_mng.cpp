// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "du_cg_res_mng.h"
#include "du_ue_resource_config.h"
#include "ocudu/ocudulog/ocudulog.h"
#include "ocudu/ran/prach/prach_time_mapping.h"
#include "ocudu/ran/resource_allocation/resource_allocation_frequency.h"
#include "ocudu/ran/serv_cell_index.h"
#include "ocudu/scheduler/config/pucch_guardbands.h"
#include "ocudu/scheduler/config/pucch_resource_generator.h"
#include "ocudu/scheduler/config/serving_cell_config_factory.h"
#include "ocudu/scheduler/support/rb_helper.h"
#include "ocudu/security/security.h"

using namespace ocudu;
using namespace odu;

static circular_vector<crb_bitmap> build_alloc_grid(const du_cell_config& cell_cfg)
{
  circular_vector<crb_bitmap> alloc_grid;

  const auto                                ul_scs = cell_cfg.ran.ul_cfg_common.init_ul_bwp.generic_params.scs;
  const prach_helper::preamble_slot_mapping td_mapper(
      cell_cfg.ran.dl_carrier.band,
      ul_scs,
      cell_cfg.ran.ul_cfg_common.init_ul_bwp.rach_cfg_common.value().rach_cfg_generic.prach_config_index);

  const unsigned prach_period_sl =
      get_nof_slots_per_subframe(ul_scs) * static_cast<unsigned>(NOF_SUBFRAMES_PER_FRAME) * td_mapper.sfn_period();
  ocudu_assert(prach_period_sl > 0, "PRACH opportunities period must be positive");

  ocudu_assert(cell_cfg.ran.init_bwp.cg_cfg.has_value() and
                   cell_cfg.ran.init_bwp.cg_cfg.value().periodicity.has_value(),
               "Configured Grant must be configured and with a period set");
  const auto cg_period_sl = static_cast<unsigned>(cell_cfg.ran.init_bwp.cg_cfg.value().periodicity.value());

  const unsigned alloc_grid_slot_size = std::lcm(prach_period_sl, cg_period_sl);

  // Initialize the grid as LCM between CG and PRACH periods, with all CRBs free.
  const unsigned bwp_nof_crbs = cell_cfg.ran.ul_cfg_common.init_ul_bwp.generic_params.crbs.length();
  alloc_grid.resize(alloc_grid_slot_size, crb_bitmap(bwp_nof_crbs));

  // Add PUCCH guardbands.
  ocudu_assert(cell_cfg.ran.ul_cfg_common.init_ul_bwp.pucch_cfg_common.has_value(),
               "PUCCH Config Common not configured");
  const std::vector<pucch_resource> cell_pucch_res_list = config_helpers::generate_cell_pucch_res_list(
      cell_cfg.ran.init_bwp.pucch.resources, cell_cfg.ran.ul_cfg_common.init_ul_bwp.generic_params.crbs.length());
  crb_bitmap pucch_crbs =
      compute_pucch_crbs(cell_cfg.ran.ul_cfg_common.init_ul_bwp.generic_params.crbs,
                         cell_cfg.ran.ul_cfg_common.init_ul_bwp.pucch_cfg_common.value().pucch_resource_common,
                         cell_pucch_res_list);

  // Fill each element of allocation grid with the PUCCH guardbands.
  ocudu_assert(pucch_crbs.size() == bwp_nof_crbs, "PUCCH CRB bitmap size mismatch with BWP size");
  for (unsigned n = 0; n != alloc_grid_slot_size; ++n) {
    alloc_grid[n] |= pucch_crbs;
    if (td_mapper.has_prach_occasion(slot_point(ul_scs, n))) {
      alloc_grid[n].fill(0, bwp_nof_crbs);
    }
  }

  return alloc_grid;
}

du_cg_type1_res_mng::cell_context::cell_context(const du_cell_config& cell_cfg_) :
  cell_cfg(cell_cfg_),
  tdd_ul_dl_cfg_common(cell_cfg_.ran.tdd_cfg),
  cg_alloc_grid(build_alloc_grid(cell_cfg_)),
  nof_rbs_allocated(cg_alloc_grid.size(), 0U)
{
}

std::optional<unsigned> du_cg_type1_res_mng::cell_context::find_optimal_cg_offset()
{
  constexpr unsigned max_metric      = crb_bitmap::max_size();
  const auto         weight_function = [&](unsigned offset) -> unsigned {
    if (tdd_ul_dl_cfg_common.has_value() and not is_tdd_full_ul_slot(tdd_ul_dl_cfg_common.value(), offset)) {
      return max_metric;
    }
    if (cg_alloc_grid[offset].all()) {
      return max_metric;
    }
    const auto& cg_cfg = cell_cfg.ran.init_bwp.cg_cfg.value();
    if (nof_rbs_allocated[offset] + cg_cfg.nof_rbs > cg_cfg.max_nof_cell_cg_rbs) {
      return max_metric;
    }

    return nof_rbs_allocated[offset];
  };

  const auto cg_period_sl = static_cast<unsigned>(cell_cfg.ran.init_bwp.cg_cfg.value().periodicity.value());

  // This is the vector with the metrics of all slots, repeated up to LCM of CG period and PRACH period.
  std::vector<unsigned> metric_values_extended(cg_alloc_grid.size(), max_metric);
  for (unsigned n = 0; n != cg_alloc_grid.size(); ++n) {
    metric_values_extended[n] = weight_function(n);
  }

  // Compute the CG offset metrics as the max of all periodic repetitions of the CG offset within the LCM of CG period
  // and PRACH period.
  std::vector<unsigned> offset_metrics(cg_period_sl, 0);
  for (unsigned n = 0; n != cg_alloc_grid.size(); ++n) {
    offset_metrics[n % cg_period_sl] = std::max(offset_metrics[n % cg_period_sl], metric_values_extended[n]);
  }

  const auto optimal_offset_it = std::min_element(offset_metrics.begin(), offset_metrics.end());
  auto       optimal_idx       = static_cast<unsigned>(std::distance(offset_metrics.begin(), optimal_offset_it));
  return *optimal_offset_it == max_metric ? std::nullopt : std::optional(optimal_idx);
}

void du_cg_type1_res_mng::add_cell(du_cell_index_t cell_idx, const du_cell_config& cell_cfg)
{
  cells.emplace(cell_idx, cell_cfg);
}

void du_cg_type1_res_mng::rem_cell(du_cell_index_t cell_idx)
{
  cells.erase(cell_idx);
}

// The logic of this class to assign the CG resources is to fist select the offset with the minimum number of RBs used
// for CG. Once the offset is chosen, the class allocates a set of contiguous RBs to the UE.
bool du_cg_type1_res_mng::alloc_resources(cell_group_config& cell_grp_cfg)
{
  ocudulog::fetch_basic_logger("DU-MNG").debug("Allocating CG config");

  auto& cell_cfg_ded = cell_grp_cfg.cells.at(SERVING_PCELL_IDX);

  // Skip cells without CG configured (not registered in the CG resource manager).
  if (not cells.contains(cell_cfg_ded.serv_cell_cfg.cell_index)) {
    return true;
  }

  auto&                 cell     = cells[cell_cfg_ded.serv_cell_cfg.cell_index];
  const du_cell_config& cell_cfg = cell.cell_cfg;

  // Fine the optimal CG offset (the offset with the min number of RBs for CG).
  const std::optional<unsigned> offset = cell.find_optimal_cg_offset();
  if (offset == std::nullopt) {
    return false;
  }
  const unsigned offset_val = offset.value();

  // Choose RBs.
  crb_interval cg_rbs = rb_helper::find_empty_interval_of_length(cell.cg_alloc_grid[offset_val],
                                                                 cell_cfg.ran.init_bwp.cg_cfg.value().nof_rbs);

  if (cg_rbs.length() < cell_cfg.ran.init_bwp.cg_cfg.value().nof_rbs) {
    return false;
  }

  // After this point, the allocation cannot fail.

  // Update the BWP configuration for this UE with the allocated offset and RBs.
  cell_cfg_ded.init_bwp().ul.cg.emplace();
  cell_cfg_ded.init_bwp().ul.cg.value().cg_offset = offset_val;

  const unsigned bwp_crb_start               = cell_cfg.ran.ul_cfg_common.init_ul_bwp.generic_params.crbs.start();
  cell_cfg_ded.init_bwp().ul.cg.value().vrbs = rb_helper::crb_to_vrb_ul_non_interleaved(
      crb_interval{cg_rbs.start() + bwp_crb_start, cg_rbs.stop() + bwp_crb_start}, bwp_crb_start);
  // NOTE: CS-RNTI is allocated by the MAC layer's RNTI manager during UE reconfiguration and stored back via
  // set_cs_rnti().

  // Build the full CG configuration from the cell-level defaults. This populates all fields of cg_configuration,
  // including rrc_configured_ul_grant_cfg, from the cg_builder_params stored in the cell config.
  cg_configuration ue_cg_cfg =
      config_helpers::make_default_ue_cell_config(cell_cfg.ran, cell_cfg_ded.serv_cell_cfg.cell_index)
          .serv_cell_cfg.ul_config.value()
          .init_ul_bwp.cg_cfg.value();

  ocudu_assert(ue_cg_cfg.rrc_configured_ul_grant_cfg.has_value(),
               "rrc_configured_ul_grant must be set for a Type 1 CG");

  // Compute PUSCH symbols to avoid overlapping with SRS.
  const ofdm_symbol_range non_srs_symbols =
      cell_cfg.ran.init_bwp.srs_cfg.srs_type_enabled != srs_type::disabled
          ? ofdm_symbol_range{0,
                              NOF_OFDM_SYM_PER_SLOT_NORMAL_CP - cell_cfg.ran.init_bwp.srs_cfg.max_nof_symbols.value()}
          : ofdm_symbol_range{0, NOF_OFDM_SYM_PER_SLOT_NORMAL_CP};

  ocudu_assert(cell_cfg.ran.ul_cfg_common.init_ul_bwp.pusch_cfg_common.has_value(),
               "PUSCH Config Common must be configured.");
  const auto& td_res        = cell_cfg.ran.ul_cfg_common.init_ul_bwp.pusch_cfg_common.value().pusch_td_alloc_list;
  unsigned    cg_td_res_idx = 0U;
  // PUSCH time-domain resources are sorted by increasing k2 first, then by decreasing symbols .stop().
  for (unsigned n = 0, sz = td_res.size(); n != sz; ++n) {
    if (td_res[n].symbols.stop() <= non_srs_symbols.stop()) {
      cg_td_res_idx = n;
      break;
    }
  }

  // Set the per-UE parameters.
  ue_cg_cfg.rrc_configured_ul_grant_cfg.value().time_domain_offset     = offset_val;
  ue_cg_cfg.rrc_configured_ul_grant_cfg.value().time_domain_allocation = cg_td_res_idx;
  ue_cg_cfg.rrc_configured_ul_grant_cfg.value().freq_domain_res =
      ra_frequency_type1_configuration{cell_cfg.ran.ul_cfg_common.init_ul_bwp.generic_params.crbs.length(),
                                       cell_cfg_ded.init_bwp().ul.cg.value().vrbs.start(),
                                       cell_cfg_ded.init_bwp().ul.cg.value().vrbs.length()};

  // > Common parameters: fill serving_cell_cfg with the full CG configuration.
  cell_cfg_ded.serv_cell_cfg.ul_config->init_ul_bwp.cg_cfg.emplace(ue_cg_cfg);

  // Register the used resources in the grid and counters.
  // NOTE: as the offset is bounded with the CG period, if the PRACH period is larger, we register this for all
  // occurrences of the offset with the vector size.
  const auto cg_period_sl = static_cast<unsigned>(cell_cfg.ran.init_bwp.cg_cfg.value().periodicity.value());
  for (unsigned n = offset_val, sz = cell.nof_rbs_allocated.size(); n < sz; n += cg_period_sl) {
    cell.nof_rbs_allocated[n] += cg_rbs.length();
    cell.cg_alloc_grid[n].fill(cg_rbs.start(), cg_rbs.stop());
  }

  return true;
}

void du_cg_type1_res_mng::dealloc_resources(cell_group_config& cell_grp_cfg)
{
  auto& cell_cfg_ded = cell_grp_cfg.cells.at(SERVING_PCELL_IDX);

  if (not cells.contains(cell_cfg_ded.serv_cell_cfg.cell_index)) {
    return;
  }
  if (not cell_cfg_ded.serv_cell_cfg.ul_config.has_value() or
      not cell_cfg_ded.serv_cell_cfg.ul_config->init_ul_bwp.cg_cfg.has_value() or
      not cell_cfg_ded.init_bwp().ul.cg.has_value()) {
    return;
  }

  auto&       cell     = cells[cell_cfg_ded.serv_cell_cfg.cell_index];
  const auto& cell_cfg = cell.cell_cfg;

  // Recover the allocated CRBs from the UE BWP config.
  const auto&        ue_cg         = cell_cfg_ded.init_bwp().ul.cg;
  const unsigned     bwp_crb_start = cell_cfg.ran.ul_cfg_common.init_ul_bwp.generic_params.crbs.start();
  const crb_interval cg_crbs       = rb_helper::vrb_to_crb_ul_non_interleaved(ue_cg.value().vrbs, bwp_crb_start);
  // Convert to BWP-relative indices used by the allocation grid.
  const unsigned crb_start = cg_crbs.start() - bwp_crb_start;
  const unsigned crb_stop  = cg_crbs.stop() - bwp_crb_start;

  // Remove the CRBs from the allocation grid and counters for every periodic repetition.
  const auto cg_period_sl = static_cast<unsigned>(cell_cfg.ran.init_bwp.cg_cfg.value().periodicity.value());
  for (unsigned n = ue_cg.value().cg_offset, sz = cell.nof_rbs_allocated.size(); n < sz; n += cg_period_sl) {
    cell.cg_alloc_grid[n].fill(crb_start, crb_stop, false);
    ocudu_assert(
        cell.nof_rbs_allocated[n] >= ue_cg.value().vrbs.length(), "nof_rbs_allocated underflow at slot offset={}", n);
    cell.nof_rbs_allocated[n] -= ue_cg.value().vrbs.length();
  }

  // Reset the CG configuration.
  cell_cfg_ded.init_bwp().ul.cg.reset();
  cell_cfg_ded.serv_cell_cfg.ul_config->init_ul_bwp.cg_cfg.reset();
}
