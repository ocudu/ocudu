// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "prach_scheduler.h"
#include "ocudu/ocudulog/ocudulog.h"
#include "ocudu/ran/band_helper.h"
#include "ocudu/ran/frame_types.h"
#include "ocudu/ran/prach/prach_cyclic_shifts.h"
#include "ocudu/ran/prach/prach_frequency_mapping.h"
#include "ocudu/ran/prach/prach_preamble_information.h"
#include "ocudu/support/compiler.h"

using namespace ocudu;

prach_scheduler::prach_scheduler(const cell_configuration& cfg_) :
  cell_cfg(cfg_),
  logger(ocudulog::fetch_basic_logger("SCHED")),
  td_mapper(cell_cfg.params.dl_carrier.band,
            cell_cfg.params.ul_cfg_common.init_ul_bwp.generic_params.scs,
            rach_cfg_common().rach_cfg_generic.prach_config_index)
{
  // Obtain the PRACH configuration.
  prach_configuration prach_cfg =
      prach_configuration_get(band_helper::get_freq_range(cell_cfg.params.dl_carrier.band),
                              cell_cfg.paired_spectrum() ? duplex_mode::FDD : duplex_mode::TDD,
                              rach_cfg_common().rach_cfg_generic.prach_config_index);

  // With SCS 15kHz and 30kHz, only normal CP is supported.
  static constexpr unsigned nof_symbols_per_slot = NOF_OFDM_SYM_PER_SLOT_NORMAL_CP;

  prach_symbols_slots_duration prach_duration_info =
      get_prach_duration_info(prach_cfg, cell_cfg.params.ul_cfg_common.init_ul_bwp.generic_params.scs);

  // Derive PRACH duration information.
  // The information we need are not related to whether it is the last PRACH occasion.
  constexpr bool                   is_last_prach_occasion = false;
  const prach_preamble_information info =
      is_long_preamble(prach_cfg.format)
          ? get_prach_preamble_long_info(prach_cfg.format)
          : get_prach_preamble_short_info(
                prach_cfg.format,
                to_ra_subcarrier_spacing(cell_cfg.params.ul_cfg_common.init_ul_bwp.generic_params.scs),
                is_last_prach_occasion);

  // This is a safety margin that prevents PUSCH REs to be decoded by the PRACH decoder with short PRACH formats, or
  // that prevents PUSCH KOs from interference with adjacent PRACH long formats; we apply it to both side of the
  // PRACH RBs, i.e., in both [0, prb_start) and [prb_stop, crbs_stop) intervals.
  // TODO: remove them when the PHY is supports short PRACH with adjacent PUCCH/PUSCH.
  const unsigned prach_to_pusch_guardband = cell_cfg.expert_cfg.ra.nof_prach_guardbands_rbs;

  for (unsigned id_fd_ra = 0; id_fd_ra != rach_cfg_common().rach_cfg_generic.msg1_fdm; ++id_fd_ra) {
    cached_prach_occasion& cached_prach = cached_prachs.emplace_back();

    const unsigned prach_nof_prbs =
        prach_frequency_mapping_get(info.scs, cell_cfg.params.ul_cfg_common.init_ul_bwp.generic_params.scs).nof_rb_ra;
    // This is the start of the PRBs dedicated to PRACH.
    const unsigned     prb_start = rach_cfg_common().rach_cfg_generic.msg1_frequency_start + id_fd_ra * prach_nof_prbs;
    const prb_interval prach_prbs{prb_start - std::min(prach_to_pusch_guardband, prb_start),
                                  std::min(prb_start + prach_nof_prbs + prach_to_pusch_guardband,
                                           cell_cfg.params.ul_cfg_common.init_ul_bwp.generic_params.crbs.length())};
    const crb_interval crbs = prb_to_crb(cell_cfg.params.ul_cfg_common.init_ul_bwp.generic_params, prach_prbs);

    if (td_mapper.has_long_preamble()) {
      // If the PRACH preamble is longer than 1 slot, then allocate a grant for each slot that include the preamble.
      for (unsigned prach_slot_idx = 0; prach_slot_idx != td_mapper.prach_burst_length_slots(); ++prach_slot_idx) {
        // For the first slot, use the start_symbol_pusch_scs; in any other case, the preamble starts from the initial
        // symbol. For the last slot, compute the final symbol; in any other case, the preamble ends at the last slot's
        // symbol.
        const ofdm_symbol_range prach_symbols{
            prach_slot_idx == 0 ? prach_duration_info.start_symbol_pusch_scs : 0,
            prach_slot_idx < td_mapper.prach_burst_length_slots() - 1
                ? nof_symbols_per_slot
                : (prach_duration_info.start_symbol_pusch_scs + prach_duration_info.nof_symbols) %
                      nof_symbols_per_slot};
        cached_prach.grant_list.emplace_back(
            grant_info{cell_cfg.params.ul_cfg_common.init_ul_bwp.generic_params.scs, prach_symbols, crbs});
      }
    } else {
      const ofdm_symbol_range prach_symbols{prach_duration_info.start_symbol_pusch_scs,
                                            prach_duration_info.start_symbol_pusch_scs +
                                                prach_duration_info.nof_symbols};
      // If the burst of PRACH opportunities extends over 1 slot, then allocate a grant for each slot that include these
      // opportunities (1 or 2 slots).
      for (unsigned prach_slot_idx = 0; prach_slot_idx != td_mapper.prach_burst_length_slots(); ++prach_slot_idx) {
        // For the short PRACH formats, both grants (if more than 1) occupy the same symbols within the slot.
        cached_prach.grant_list.emplace_back(
            grant_info{cell_cfg.params.ul_cfg_common.init_ul_bwp.generic_params.scs, prach_symbols, crbs});
      }
    }

    // Pre-compute PRACH occasion parameters.
    cached_prach.occasion.pci    = cell_cfg.params.pci;
    cached_prach.occasion.format = prach_cfg.format;
    // Note: the Starting Symbol that is required for the PHY interface is actually the one given by Table 6.3.3.2-2
    // and 6.3.3.2-3, TS 38.211.
    cached_prach.occasion.start_symbol        = prach_cfg.starting_symbol;
    cached_prach.occasion.nof_prach_occasions = prach_cfg.nof_occasions_within_slot;
    cached_prach.occasion.nof_cs              = prach_cyclic_shifts_get(
        info.scs, rach_cfg_common().restricted_set, rach_cfg_common().rach_cfg_generic.zero_correlation_zone_config);
    cached_prach.occasion.index_fd_ra = id_fd_ra;
    cached_prach.occasion.nof_fd_ra   = rach_cfg_common().rach_cfg_generic.msg1_fdm;
    // We assume all the preambles from 0 to 64 are assigned to the same PRACH occasion.
    // TODO: adapt scheduler to difference preamble indices intervals.
    cached_prach.occasion.start_preamble_index = 0;
    cached_prach.occasion.nof_preamble_indexes =
        cell_cfg.params.ul_cfg_common.init_ul_bwp.rach_cfg_common->total_nof_ra_preambles;
  }
}

void prach_scheduler::run_slot(cell_resource_allocator& res_grid)
{
  if (OCUDU_UNLIKELY(first_slot_ind)) {
    // If called for the first time, pre-allocates the PRACH PDUs over the entire grid, until the last
    // (farthest in the future) usable slot.
    first_slot_ind = false;
    // NOTE: Min value of cell_resource_allocator.max_ul_slot_alloc_delay is 20, max value of prach_length_slots is 7.
    for (unsigned sl = 0; sl < (res_grid.max_ul_slot_alloc_delay - td_mapper.prach_burst_length_slots() + 1); ++sl) {
      allocate_slot_prach_pdus(res_grid, res_grid[sl].slot);
    }
    return;
  }

  // Pre-allocate PRACH PDU in the last slot.
  allocate_slot_prach_pdus(res_grid,
                           res_grid[res_grid.max_ul_slot_alloc_delay - td_mapper.prach_burst_length_slots()].slot);
}

void prach_scheduler::stop()
{
  first_slot_ind = true;
}

void prach_scheduler::allocate_slot_prach_pdus(cell_resource_allocator& res_grid, slot_point sl)
{
  // If any of the slots over which the PRACH preamble should be allocated isn't an UL slot, return.
  if (td_mapper.has_long_preamble()) {
    for (unsigned sl_idx = 0; sl_idx != td_mapper.prach_burst_length_slots(); ++sl_idx) {
      if (not cell_cfg.is_fully_ul_enabled(sl + sl_idx)) {
        return;
      }
    }
  } else {
    if (not cell_cfg.is_fully_ul_enabled(sl)) {
      return;
    }
  }

  // Check if the current slot is a valid PRACH occasion.
  if (not td_mapper.has_prach_occasion(sl)) {
    return;
  }

  for (const cached_prach_occasion& cached_prach : cached_prachs) {
    if (td_mapper.has_long_preamble()) {
      // Reserve RBs and symbols of the PRACH occasion in the resource grid for each grant (over multiple slots) of the
      // preamble.
      for (unsigned sl_idx = 0; sl_idx != td_mapper.prach_burst_length_slots(); ++sl_idx) {
        res_grid[sl + sl_idx].ul_res_grid.fill(cached_prach.grant_list[sl_idx]);
      }
      // Add PRACH occasion to scheduler slot output (one PRACH PDU per preamble).
      res_grid[sl].result.ul.prachs.push_back(cached_prach.occasion);
    } else {
      // Reserve RBs and symbols of the PRACH occasion in the resource grid for each grant (over 1 or 2 slots) of the
      // preamble and add the PRACH occasions to scheduler slot output (1 or 2 PRACH PDU per burst of PRACH
      // opportunities, depending on whether this burst repeats over 1 or 2 slots).
      res_grid[sl].ul_res_grid.fill(cached_prach.grant_list[sl.slot_index() % td_mapper.prach_burst_length_slots()]);
      res_grid[sl].result.ul.prachs.push_back(cached_prach.occasion);
    }
  }
}
