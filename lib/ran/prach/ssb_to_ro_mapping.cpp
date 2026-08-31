// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/ran/prach/ssb_to_ro_mapping.h"
#include "ocudu/ran/band_helper.h"
#include "ocudu/ran/prach/prach_configuration.h"
#include "ocudu/ran/prach/prach_preamble_information.h"
#include "ocudu/ran/ssb/ssb_mapping.h"
#include "ocudu/support/error_handling.h"
#include "ocudu/support/math/bit_ops.h"
#include "ocudu/support/ocudu_assert.h"
#include <limits>

using namespace ocudu;

/// N_gap, as per TS 38.213, Table 8.1-2.
static unsigned get_n_gap(const prach_configuration& prach_cfg, subcarrier_spacing ul_scs)
{
  // As per TS 38.213, Section 8.1, N_gap is zero for preamble format B4.
  if (prach_cfg.format == prach_format_type::B4) {
    return 0;
  }
  // Long preamble formats use a 1.25kHz or 5kHz preamble subcarrier spacing.
  if (is_long_preamble(prach_cfg.format)) {
    return 0;
  }
  switch (ul_scs) {
    case subcarrier_spacing::kHz15:
    case subcarrier_spacing::kHz30:
    case subcarrier_spacing::kHz60:
    case subcarrier_spacing::kHz120:
      return 2;
    default:
      report_fatal_error("Unsupported PRACH subcarrier spacing {}", to_string(ul_scs));
  }
}

/// PRACH configuration derived from the PRACH configuration index of the cell.
static prach_configuration get_prach_config(const prach_helper::ssb_to_ro_mapping_config& config)
{
  return prach_configuration_get(band_helper::get_freq_range(config.band),
                                 band_helper::get_duplex_mode(config.band),
                                 config.rach_cfg.rach_cfg_generic.prach_config_index);
}

prach_helper::ssb_to_ro_mapping::ssb_to_ro_mapping(const ssb_to_ro_mapping_config& config) :
  paired_spectrum(band_helper::is_paired_spectrum(config.band)),
  cp(config.cp),
  tdd_cfg(config.tdd_cfg),
  n_gap(get_n_gap(get_prach_config(config), config.ul_scs)),
  ul_scs(config.ul_scs),
  nof_slots_per_frame(get_nof_slots_per_subframe(config.ul_scs) * NOF_SUBFRAMES_PER_FRAME),
  nof_td_occasions(std::max<unsigned>(1, get_prach_config(config).nof_occasions_within_slot)),
  nof_fd_occasions(std::max<unsigned>(1, config.rach_cfg.rach_cfg_generic.msg1_fdm)),
  preambles_per_ssb(ra_helper::get_preambles_per_ssb(config.rach_cfg)),
  nof_ssb_per_ro(get_nof_ssb_per_ro(config.rach_cfg.nof_ssb_per_ro)),
  nof_ro_per_ssb(get_nof_ro_per_ssb(config.rach_cfg.nof_ssb_per_ro)),
  first_td_occasion_start(get_prach_duration_info(get_prach_config(config), config.ul_scs).start_symbol_pusch_scs),
  td_occasion_duration(get_prach_duration_info(get_prach_config(config), config.ul_scs).nof_symbols / nof_td_occasions),
  nof_burst_slots(is_long_preamble(get_prach_config(config).format)
                      ? get_prach_duration_info(get_prach_config(config), config.ul_scs).prach_length_slots
                      : 1),
  td_mapping(config.band, config.ul_scs, config.rach_cfg.rach_cfg_generic.prach_config_index)
{
  report_error_if_not(not tdd_cfg.has_value() or tdd_cfg->ref_scs == config.ul_scs,
                      "The TDD reference subcarrier spacing must match the uplink subcarrier spacing");

  const ssb_bitmap_t& ssb_bitmap = config.ssb_cfg.ssb_bitmap;
  for (int idx = ssb_bitmap.find_lowest(); idx != -1; idx = ssb_bitmap.find_lowest(idx + 1, ssb_bitmap.size())) {
    ssb_indexes.push_back(static_cast<uint8_t>(idx));
  }
  report_error_if_not(not ssb_indexes.empty(), "No SS/PBCH block is active");

  build_ssb_symbol_table(config);

  if (ssb_indexes.size() == 1) {
    // Every PRACH occasion maps to the only active SS/PBCH block, so no occasion table is needed.
    single_ssb = ssb_indexes.front();
    return;
  }

  build_occasion_table();
}

ofdm_symbol_range prach_helper::ssb_to_ro_mapping::get_occasion_symbols(unsigned slot_offset,
                                                                        unsigned td_occasion_idx) const
{
  if (nof_burst_slots == 1) {
    const unsigned start = first_td_occasion_start + td_occasion_idx * td_occasion_duration;
    return {static_cast<uint8_t>(start), static_cast<uint8_t>(start + td_occasion_duration)};
  }

  // A long preamble runs from its starting symbol in the first slot of the burst to the end of the preamble in the
  // last one, filling every slot in between.
  const unsigned nsymb_per_slot = get_nsymb_per_slot(cp);
  const unsigned start          = slot_offset == 0 ? first_td_occasion_start : 0;
  const unsigned stop           = slot_offset + 1 < nof_burst_slots
                                      ? nsymb_per_slot
                                      : first_td_occasion_start + td_occasion_duration - slot_offset * nsymb_per_slot;
  return {static_cast<uint8_t>(start), static_cast<uint8_t>(stop)};
}

bool prach_helper::ssb_to_ro_mapping::is_valid_occasion(slot_point sl, unsigned td_occasion_idx) const
{
  for (unsigned slot_offset = 0; slot_offset != nof_burst_slots; ++slot_offset) {
    if (not is_valid_ro(sl + slot_offset, get_occasion_symbols(slot_offset, td_occasion_idx))) {
      return false;
    }
  }
  return true;
}

void prach_helper::ssb_to_ro_mapping::build_ssb_symbol_table(const ssb_to_ro_mapping_config& config)
{
  const ssb_configuration& ssb_cfg        = config.ssb_cfg;
  const ssb_pattern_case   ssb_case       = band_helper::get_ssb_pattern(config.band, ssb_cfg.scs);
  const unsigned           nsymb_per_slot = get_nsymb_per_slot(cp);
  const unsigned           ssb_period_slots =
      static_cast<unsigned>(ssb_cfg.ssb_period) * get_nof_slots_per_subframe(config.ul_scs);

  // The SS/PBCH block symbol indexes of TS 38.213, Section 4.1 are expressed in the SSB numerology, so they are
  // rescaled to the uplink one.
  const int scs_shift =
      static_cast<int>(to_numerology_value(config.ul_scs)) - static_cast<int>(to_numerology_value(ssb_cfg.scs));

  last_ssb_symbol.assign(ssb_period_slots, -1);
  for (ssb_id_t ssb_idx : ssb_indexes) {
    // First symbol of the SS/PBCH block within the half-frame.
    unsigned l_first  = ssb_get_l_first(ssb_case, ssb_idx);
    unsigned nof_symb = NOF_SSB_SYMB;
    if (scs_shift >= 0) {
      l_first <<= scs_shift;
      nof_symb <<= scs_shift;
    } else {
      l_first >>= -scs_shift;
      nof_symb = std::max(1U, nof_symb >> -scs_shift);
    }

    const unsigned ssb_slot  = l_first / nsymb_per_slot;
    const unsigned last_symb = std::min(nsymb_per_slot, (l_first % nsymb_per_slot) + nof_symb) - 1;
    ocudu_assert(ssb_slot < ssb_period_slots, "SS/PBCH block slot out of the SSB period");
    last_ssb_symbol[ssb_slot] = std::max<int8_t>(last_ssb_symbol[ssb_slot], static_cast<int8_t>(last_symb));
  }
}

bool prach_helper::ssb_to_ro_mapping::is_valid_ro(slot_point sl, ofdm_symbol_range prach_symbols) const
{
  ocudu_assert(sl.scs() == ul_scs, "Slot subcarrier spacing does not match the uplink one");

  // As per TS 38.213, Section 8.1, all the PRACH occasions of a paired spectrum or supplementary uplink band are
  // valid.
  if (paired_spectrum or not tdd_cfg.has_value()) {
    return true;
  }

  const unsigned slot_idx = sl.sfn() * nof_slots_per_frame + sl.slot_index();

  if (get_active_tdd_ul_symbols(*tdd_cfg, slot_idx, cp).contains(prach_symbols)) {
    return true;
  }

  const ofdm_symbol_range dl_symbols = get_active_tdd_dl_symbols(*tdd_cfg, slot_idx, cp);
  int                     last_busy  = dl_symbols.empty() ? -1 : static_cast<int>(dl_symbols.stop()) - 1;
  if (not last_ssb_symbol.empty()) {
    last_busy = std::max(last_busy, static_cast<int>(last_ssb_symbol[slot_idx % last_ssb_symbol.size()]));
  }

  return static_cast<int>(prach_symbols.start()) >= last_busy + 1 + static_cast<int>(n_gap);
}

bool prach_helper::ssb_to_ro_mapping::is_valid_prach_slot(slot_point sl) const
{
  for (unsigned td = 0; td != nof_td_occasions; ++td) {
    if (not is_valid_occasion(sl, td)) {
      return false;
    }
  }
  return true;
}

std::vector<unsigned> prach_helper::ssb_to_ro_mapping::count_ros_per_frame(unsigned nof_frames) const
{
  std::vector<unsigned> ros_per_frame(nof_frames, 0);
  for (unsigned sfn = 0; sfn != nof_frames; ++sfn) {
    if (not td_mapping.is_sfn_prach_occasion(sfn)) {
      continue;
    }
    for (unsigned slot_idx = 0; slot_idx != nof_slots_per_frame; ++slot_idx) {
      if (not td_mapping.has_slot_index_prach_occasion(slot_idx)) {
        continue;
      }
      const slot_point sl{ul_scs, sfn, slot_idx};
      for (unsigned td = 0; td != nof_td_occasions; ++td) {
        ros_per_frame[sfn] += is_valid_occasion(sl, td) ? nof_fd_occasions : 0;
      }
    }
  }
  return ros_per_frame;
}

void prach_helper::ssb_to_ro_mapping::build_occasion_table()
{
  slot_ordinals.assign(nof_slots_per_frame, -1);
  for (unsigned slot_idx = 0; slot_idx != nof_slots_per_frame; ++slot_idx) {
    if (td_mapping.has_slot_index_prach_occasion(slot_idx)) {
      slot_ordinals[slot_idx] = static_cast<int16_t>(nof_prach_slots_per_frame++);
    }
  }

  // As per TS 38.213, Section 8.1, all the active SS/PBCH block indexes are mapped over this many PRACH occasions.
  ros_per_cycle = ssb_indexes.size() * nof_ro_per_ssb / nof_ssb_per_ro;

  // The PRACH occasion pattern only repeats once both the PRACH configuration period and the SS/PBCH block burst
  // realign. Both span a power of two number of system frames.
  const unsigned ssb_period_frames = std::max<unsigned>(1, last_ssb_symbol.size() / nof_slots_per_frame);
  const unsigned base_pattern      = std::max(td_mapping.sfn_period(), ssb_period_frames);

  const std::vector<unsigned> ros_per_frame = count_ros_per_frame(base_pattern);

  // As per TS 38.213, Table 8.1-1, the association period is a power of two number of PRACH configuration periods
  // that keeps the association period below or equal to 160msec. Pick the smallest one that maps every active
  // SS/PBCH block index at least once, in every association period of the association pattern period.
  const unsigned max_assoc_period_frames = prach_constants::MAX_PRACH_SFN_PERIOD;
  for (assoc_period_frames = td_mapping.sfn_period();; assoc_period_frames *= 2) {
    pattern_frames = std::max(assoc_period_frames, base_pattern);

    unsigned min_ros = std::numeric_limits<unsigned>::max();
    for (unsigned first = 0; first != pattern_frames; first += assoc_period_frames) {
      unsigned ros = 0;
      for (unsigned sfn = first; sfn != first + assoc_period_frames; ++sfn) {
        ros += ros_per_frame[sfn % base_pattern];
      }
      min_ros = std::min(min_ros, ros);
    }
    if (min_ros >= ros_per_cycle) {
      break;
    }
    report_error_if_not(assoc_period_frames < max_assoc_period_frames,
                        "The PRACH configuration cannot map the {} active SS/PBCH blocks within an association period "
                        "of {} system frames",
                        ssb_indexes.size(),
                        assoc_period_frames);
  }

  nof_mapped_ros.assign(pattern_frames / assoc_period_frames, 0);
  prach_slots.assign(pattern_frames * nof_prach_slots_per_frame, prach_slot_entry{});
  unsigned ro_ordinal = 0;
  for (unsigned sfn = 0; sfn != pattern_frames; ++sfn) {
    if (sfn % assoc_period_frames == 0) {
      // Occasion ordinals restart at every association period, as per TS 38.213, Section 8.1.
      ro_ordinal = 0;
    }
    if (td_mapping.is_sfn_prach_occasion(sfn)) {
      for (unsigned slot_idx = 0; slot_idx != nof_slots_per_frame; ++slot_idx) {
        if (slot_ordinals[slot_idx] < 0) {
          continue;
        }
        const slot_point  sl{ul_scs, sfn, slot_idx};
        prach_slot_entry& entry = prach_slots[sfn * nof_prach_slots_per_frame + slot_ordinals[slot_idx]];
        entry.first_ro          = static_cast<uint16_t>(ro_ordinal);
        for (unsigned td = 0; td != nof_td_occasions; ++td) {
          if (not is_valid_occasion(sl, td)) {
            continue;
          }
          entry.valid_td_mask |= 1U << td;
          ro_ordinal += nof_fd_occasions;
        }
        // A PRACH occasion is scheduled for the whole PRACH slot, so a slot whose occasions are only partly valid
        // would make the gNB and the UE disagree on the occasion ordering.
        report_error_if_not(entry.valid_td_mask == 0 or
                                entry.valid_td_mask == static_cast<uint8_t>((1U << nof_td_occasions) - 1U),
                            "The PRACH occasions of slot {} are only partly valid",
                            slot_idx);
      }
    }
    if ((sfn + 1) % assoc_period_frames == 0) {
      // The occasions left over after the last complete mapping cycle carry no SS/PBCH block index.
      nof_mapped_ros[sfn / assoc_period_frames] = static_cast<uint16_t>((ro_ordinal / ros_per_cycle) * ros_per_cycle);
    }
  }
}

std::optional<ssb_id_t> prach_helper::ssb_to_ro_mapping::get_ssb_index(slot_point prach_slot_rx,
                                                                       unsigned   td_occasion_idx,
                                                                       unsigned   fd_occasion_idx,
                                                                       unsigned   preamble_id) const
{
  ocudu_assert(prach_slot_rx.scs() == ul_scs, "Slot subcarrier spacing does not match the uplink one");

  if (single_ssb.has_value()) {
    return single_ssb;
  }
  if (td_occasion_idx >= max_nof_td_occasions or fd_occasion_idx >= nof_fd_occasions) {
    return std::nullopt;
  }
  const int16_t slot_ordinal = slot_ordinals[prach_slot_rx.slot_index()];
  if (slot_ordinal < 0) {
    return std::nullopt;
  }
  const prach_slot_entry& entry =
      prach_slots[(prach_slot_rx.sfn() % pattern_frames) * nof_prach_slots_per_frame + slot_ordinal];
  if (((entry.valid_td_mask >> td_occasion_idx) & 1U) == 0) {
    return std::nullopt;
  }

  // Occasions are ordered by frequency index first, then by time index within the PRACH slot, as per TS 38.213,
  // Section 8.1.
  const unsigned nof_earlier_td = count_ones(entry.valid_td_mask & ((1U << td_occasion_idx) - 1U));
  const unsigned ro_ordinal     = entry.first_ro + nof_earlier_td * nof_fd_occasions + fd_occasion_idx;
  if (ro_ordinal >= nof_mapped_ros[(prach_slot_rx.sfn() % pattern_frames) / assoc_period_frames]) {
    // The occasions left over after the last complete mapping cycle carry no SS/PBCH block index.
    return std::nullopt;
  }

  const unsigned ssb_ordinal =
      ((ro_ordinal % ros_per_cycle) / nof_ro_per_ssb) * nof_ssb_per_ro + preamble_id / preambles_per_ssb;
  if (ssb_ordinal >= ssb_indexes.size()) {
    return std::nullopt;
  }
  return ssb_indexes[ssb_ordinal];
}
