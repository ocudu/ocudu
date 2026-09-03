// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ssb_scheduler.h"
#include "ocudu/ocudulog/ocudulog.h"
#include "ocudu/ran/frame_types.h"
#include "ocudu/ran/ssb/ssb_mapping.h"
#include "ocudu/support/enum_utils.h"

using namespace ocudu;

ssb_scheduler::ssb_scheduler(const cell_configuration& cfg_) :
  ssb_case(band_helper::get_ssb_pattern(cfg_.params.dl_carrier.band, cfg_.params.ssb_cfg.scs)),
  cell_cfg(cfg_),
  logger(ocudulog::fetch_basic_logger("SCHED"))
{
  ssb_period = to_value(cell_cfg.params.ssb_cfg.ssb_period);
}

void ssb_scheduler::run_slot(cell_resource_allocator& res_alloc, slot_point sl_point)
{
  if (first_run_slot) {
    // First call to run_slot. Schedule SSBs when relevant across cell resource grid.
    for (unsigned i = 0; i != res_alloc.max_dl_slot_alloc_delay + 1; ++i) {
      schedule_ssb(res_alloc[i]);
    }
    first_run_slot = false;
  } else {
    // Schedule SSB in last scheduled slot + 1 slot if relevant.
    schedule_ssb(res_alloc[res_alloc.max_dl_slot_alloc_delay]);
  }
}

void ssb_scheduler::stop()
{
  first_run_slot = true;
}

void ssb_scheduler::schedule_ssb(cell_slot_resource_allocator& res_grid) const
{
  slot_point            sl_point = res_grid.slot;
  ssb_information_list& ssb_list = res_grid.result.dl.bc.ssb_info;

  if (ssb_list.full()) {
    logger.error("Failed to allocate SSB");
    return;
  }

  // Perform mod operation of slot index by ssb_periodicity;
  // "ssb_periodicity * nof_slots_per_subframe" gives the number of slots in 1 ssb_periodicity time interval.
  const slot_point sl_point_mod(sl_point.numerology(),
                                sl_point.count() % (ssb_period * sl_point.nof_slots_per_subframe()));

  // Select SSB case with reference to TS 38.213, Section 4.1.
  switch (ssb_case) {
    case ssb_pattern_case::A:
      ssb_alloc_case_A_C(ssb_list, CUTOFF_FREQ_ARFCN_CASE_A_B_C, sl_point_mod);
      break;
    case ssb_pattern_case::C: {
      const arfcn_t ssb_cut_off_freq =
          cell_cfg.paired_spectrum() ? CUTOFF_FREQ_ARFCN_CASE_A_B_C : CUTOFF_FREQ_ARFCN_CASE_C_UNPAIRED;
      ssb_alloc_case_A_C(ssb_list, ssb_cut_off_freq, sl_point_mod);
      break;
    }
    case ssb_pattern_case::B:
      ssb_alloc_case_B(ssb_list, sl_point_mod);
      break;
    case ssb_pattern_case::D:
      ssb_alloc_case_D(ssb_list, sl_point_mod);
      break;
    default:
      ocudu_assert(ssb_case < ssb_pattern_case::invalid, "Only SSB case A, B and C are currently supported");
  }

  // Update the used DL PRBs with those allocated to the SSBs.
  for (const auto& ssb : ssb_list) {
    // TODO: In case, SSB SCS != init DL BWP SCS, we should do an adaptation of symbols and CRBs to the numerology
    // of the latter.
    const grant_info grant{cell_cfg.params.dl_cfg_common.init_dl_bwp.generic_params.scs, ssb.symbols, ssb.crbs};
    res_grid.dl_res_grid.fill(grant);
  }
}

void ssb_scheduler::ssb_alloc_case_A_C(ssb_information_list& ssb_list,
                                       arfcn_t               freq_arfcn_cut_off,
                                       slot_point            sl_point_mod) const
{
  const uint32_t slot_idx = sl_point_mod.count();

  // The OFDM symbols allocations for Case A and case C are identical; the only difference is the cutoff frequency,
  // which is 3GHz for case A and C paired, but 1.88GHz for case C unpaired.
  // For frequency lower than cutoff, SSB is allocated in slot 0 and 1 only.
  if (cell_cfg.params.dl_carrier.arfcn_f_ref <= freq_arfcn_cut_off and slot_idx > 1) {
    return;
  }

  // In case A and C, the candidate OFDM symbols (within the SSB burst) where to allocate the SSB within its period are
  // indexed as follows: n = 2, 8, 16, 22  for frequencies <= cutoff frequency; n = 2, 8, 16, 22, 30, 36, 44, 50  for
  // frequencies > cutoff frequency. Cutoff frequency is: 3GHz for Case A and Case C paired, 1.88GHz for Case C
  // unpaired. Slot 0 has OFDM symbols 2,8; Slot n has symbols {2,8} + 14 * n; TS 38.213 Section 4.1.
  if (slot_idx <= 3U) {
    constexpr std::array<uint8_t, 2U> ssb_burst_ofdm_symb = {2, 8};
    for (uint32_t n = 0, sz = ssb_burst_ofdm_symb.size(); n != sz; ++n) {
      const uint32_t ssb_idx = n + slot_idx * 2U;
      ocudu_assert(ssb_idx < cell_cfg.params.ssb_cfg.ssb_bitmap.size(), "SSB index exceeds SSB bitmap size");
      if (cell_cfg.params.ssb_cfg.ssb_bitmap.test(ssb_idx)) {
        fill_ssb_parameters(ssb_list,
                            cell_cfg.params.ssb_cfg.offset_to_point_A,
                            cell_cfg.params.ssb_cfg.k_ssb,
                            cell_cfg.params.ssb_cfg.scs,
                            cell_cfg.params.dl_cfg_common.init_dl_bwp.generic_params.scs,
                            ssb_burst_ofdm_symb[n] + slot_idx * NOF_OFDM_SYM_PER_SLOT_NORMAL_CP,
                            ssb_idx);
      }
    }
  }
}

void ssb_scheduler::ssb_alloc_case_B(ssb_information_list& ssb_list, slot_point sl_point_mod) const
{
  const uint32_t slot_idx = sl_point_mod.count();

  // For frequency lower than cutoff, SSB occasions are on slot 0 and 1 only, while for frequencies higher than the
  // cutoff, the SSB occasions are on slot 0, 1, 2, and 3.
  const arfcn_t max_slot_idx_case_B = cell_cfg.params.dl_carrier.arfcn_f_ref <= CUTOFF_FREQ_ARFCN_CASE_A_B_C ? 1U : 3U;
  if (slot_idx > max_slot_idx_case_B) {
    return;
  }

  // In case B, the candidate OFDM symbols  (within the SSB burst) where to allocate the SSB within its period are
  // indexed as follows: n = 4, 8, 16, 20  for frequencies <= cutoff frequency n = 4, 8, 16, 20, 32, 36, 44, 48  for
  // frequencies > cutoff frequency, Cutoff frequency is: 3GHz for Case B. TS 38.213 Section 4.1.

  // Slot 0 has OFDM symbols 4,8; Slot 2 has symbols 32, 36.
  if (slot_idx % 2U == 0U) {
    constexpr std::array<uint8_t, 2> ssb_burst_ofdm_symb = {4, 8};

    for (uint32_t n = 0, sz = ssb_burst_ofdm_symb.size(); n != sz; ++n) {
      const uint32_t ssb_idx = n + slot_idx * 2U;
      ocudu_assert(ssb_idx < cell_cfg.params.ssb_cfg.ssb_bitmap.size(), "SSB index exceeds SSB bitmap size");
      if (cell_cfg.params.ssb_cfg.ssb_bitmap.test(ssb_idx)) {
        fill_ssb_parameters(ssb_list,
                            cell_cfg.params.ssb_cfg.offset_to_point_A,
                            cell_cfg.params.ssb_cfg.k_ssb,
                            cell_cfg.params.ssb_cfg.scs,
                            cell_cfg.params.dl_cfg_common.init_dl_bwp.generic_params.scs,
                            ssb_burst_ofdm_symb[n] + slot_idx * NOF_OFDM_SYM_PER_SLOT_NORMAL_CP,
                            ssb_idx);
      }
    }
  }

  // Slot 1 has symbols 16, 20; Slot 3 has symbols 44, 48.
  if (slot_idx % 2U == 1U) {
    constexpr std::array<uint8_t, 2> ssb_burst_ofdm_symb = {16, 20};

    for (uint32_t n = 0, sz = ssb_burst_ofdm_symb.size(); n != sz; ++n) {
      const uint32_t ssb_idx = n + slot_idx * 2;
      ocudu_assert(ssb_idx < cell_cfg.params.ssb_cfg.ssb_bitmap.size(), "SSB index exceeds SSB bitmap size");
      if (cell_cfg.params.ssb_cfg.ssb_bitmap.test(ssb_idx)) {
        fill_ssb_parameters(ssb_list,
                            cell_cfg.params.ssb_cfg.offset_to_point_A,
                            cell_cfg.params.ssb_cfg.k_ssb,
                            cell_cfg.params.ssb_cfg.scs,
                            cell_cfg.params.dl_cfg_common.init_dl_bwp.generic_params.scs,
                            ssb_burst_ofdm_symb[n] + (slot_idx - 1) * NOF_OFDM_SYM_PER_SLOT_NORMAL_CP,
                            ssb_idx);
      }
    }
  }
}

void ssb_scheduler::ssb_alloc_case_D(ssb_information_list& ssb_list, slot_point sl_point_mod) const
{
  // Number of slots within a 5ms burst for the SSB subcarrier spacing of 120kHz.
  static constexpr unsigned nof_slots_ssb_burst = 5 * pow2(to_numerology_value(subcarrier_spacing::kHz120));

  // In Case D, the candidate OFDM symbols (within the SSB burst) where to allocate the SSB are indexed as:
  // {4, 8, 16, 20} + 28 * n where n = 0, 1, 2, 3, 5, 6, 7, 8, 10, 11, 12, 13, 15, 16, 17, 18 as defined in TS 38.213
  // Section 4.1.
  static constexpr std::array<unsigned, 16> slot_pairs = {0, 1, 2, 3, 5, 6, 7, 8, 10, 11, 12, 13, 15, 16, 17, 18};

  const uint32_t slot_idx = sl_point_mod.count();

  // Skip if the slot index is out of the 5ms burst.
  if (slot_idx >= nof_slots_ssb_burst) {
    return;
  }

  // For case D, it supports up to 64 SSB positions. It takes the entire SSB bitmap.

  // Get the slot pair index if it is available.
  const auto slot_pair_it = std::find(slot_pairs.begin(), slot_pairs.end(), slot_idx / 2);
  if (slot_pair_it == slot_pairs.end()) {
    return;
  }

  // Calculate the first SSB index within the 5ms burst for the first SSB position in this slot. Each 'n' contains 4
  // positions and each slot 2 positions.
  const unsigned first_ssb_idx = 4 * std::distance(slot_pairs.begin(), slot_pair_it) + 2 * (slot_idx % 2);

  // The starting symbols for the first SSB position in the slot correspond to symbol 4 if the slot index is even,
  // otherwise to symbol 2.
  if (cell_cfg.params.ssb_cfg.ssb_bitmap.test(first_ssb_idx)) {
    const unsigned start_symbol_idx = (slot_idx % 2 == 0) ? 4 : 2;
    fill_ssb_parameters(ssb_list,
                        cell_cfg.params.ssb_cfg.offset_to_point_A,
                        cell_cfg.params.ssb_cfg.k_ssb,
                        cell_cfg.params.ssb_cfg.scs,
                        cell_cfg.params.dl_cfg_common.init_dl_bwp.generic_params.scs,
                        start_symbol_idx,
                        first_ssb_idx);
  }

  // The starting symbols for the first SSB position in the slot correspond to symbol 8 if the slot index is even,
  // otherwise to symbol 6.
  if (cell_cfg.params.ssb_cfg.ssb_bitmap.test(first_ssb_idx + 1)) {
    const unsigned start_symbol_idx = (slot_idx % 2 == 0) ? 8 : 6;
    fill_ssb_parameters(ssb_list,
                        cell_cfg.params.ssb_cfg.offset_to_point_A,
                        cell_cfg.params.ssb_cfg.k_ssb,
                        cell_cfg.params.ssb_cfg.scs,
                        cell_cfg.params.dl_cfg_common.init_dl_bwp.generic_params.scs,
                        start_symbol_idx,
                        first_ssb_idx + 1);
  }
}

void ssb_scheduler::fill_ssb_parameters(ssb_information_list& ssb_list,
                                        ssb_offset_to_pointA  offset_to_point_A,
                                        ssb_subcarrier_offset ssb_subc_offset,
                                        subcarrier_spacing    ssb_scs,
                                        subcarrier_spacing    scs_common,
                                        uint8_t               ssb_burst_symb_idx,
                                        uint8_t               ssb_idx)
{
  ssb_information ssb_msg = {};

  ssb_msg.ssb_index = ssb_idx;
  // As per TS38.213, Section 4.1, the symbols that are passed refer to SSB burst, and can range from 0 up until 56. We
  // need to convert these into slot symbols, that range within [0, 14).
  ssb_msg.symbols.set(ssb_burst_symb_idx % NOF_OFDM_SYM_PER_SLOT_NORMAL_CP,
                      (ssb_burst_symb_idx + NOF_SSB_OFDM_SYMBOLS) % NOF_OFDM_SYM_PER_SLOT_NORMAL_CP);
  ssb_msg.crbs = get_ssb_crbs(ssb_scs, scs_common, offset_to_point_A, ssb_subc_offset);
  ssb_list.push_back(ssb_msg);
}
