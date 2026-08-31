// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/ran/prach/ssb_to_ro_mapping.h"
#include "ocudu/ran/prach/prach_constants.h"
#include "ocudu/support/error_handling.h"
#include "ocudu/support/math/bit_ops.h"
#include "ocudu/support/ocudu_assert.h"
#include <limits>

using namespace ocudu;

prach_helper::ssb_to_ro_mapping::ssb_to_ro_mapping(const prach_occasion_mapping_config& config) :
  occ_mapping(config),
  ul_scs(config.ul_scs),
  nof_slots_per_frame(get_nof_slots_per_subframe(config.ul_scs) * NOF_SUBFRAMES_PER_FRAME),
  preambles_per_ssb(ra_helper::get_preambles_per_ssb(config.rach_cfg)),
  nof_ssb_per_ro(get_nof_ssb_per_ro(config.rach_cfg.nof_ssb_per_ro)),
  nof_ro_per_ssb(get_nof_ro_per_ssb(config.rach_cfg.nof_ssb_per_ro))
{
  for (size_t idx : config.ssb_cfg.ssb_bitmap.get_bit_positions()) {
    ssb_indexes.push_back(static_cast<uint8_t>(idx));
  }
  report_error_if_not(not ssb_indexes.empty(), "No SS/PBCH block is active");

  if (ssb_indexes.size() == 1) {
    // Every PRACH occasion maps to the only active SS/PBCH block, so no occasion table is needed.
    single_ssb = ssb_indexes.front();
    return;
  }

  build_occasion_table();
}

std::vector<unsigned> prach_helper::ssb_to_ro_mapping::count_ros_per_frame(unsigned nof_frames) const
{
  const preamble_slot_mapping& td_mapping = occ_mapping.td_slot_mapping();

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
      for (unsigned td = 0; td != occ_mapping.get_nof_td_occasions(); ++td) {
        ros_per_frame[sfn] += occ_mapping.is_valid_occasion(sl, td) ? occ_mapping.get_nof_fd_occasions() : 0;
      }
    }
  }
  return ros_per_frame;
}

void prach_helper::ssb_to_ro_mapping::build_occasion_table()
{
  const preamble_slot_mapping& td_mapping       = occ_mapping.td_slot_mapping();
  const unsigned               nof_td_occasions = occ_mapping.get_nof_td_occasions();
  const unsigned               nof_fd_occasions = occ_mapping.get_nof_fd_occasions();

  slot_positions.assign(nof_slots_per_frame, -1);
  for (unsigned slot_idx = 0; slot_idx != nof_slots_per_frame; ++slot_idx) {
    if (td_mapping.has_slot_index_prach_occasion(slot_idx)) {
      slot_positions[slot_idx] = static_cast<int16_t>(nof_prach_slots_per_frame);
      ++nof_prach_slots_per_frame;
    }
  }

  // As per TS 38.213, Section 8.1, all the active SS/PBCH block indexes are mapped over this many PRACH occasions.
  ros_per_cycle = ssb_indexes.size() * nof_ro_per_ssb / nof_ssb_per_ro;

  // The PRACH occasion pattern only repeats once both the PRACH configuration period and the SS/PBCH block burst
  // realign. Both span a power of two number of system frames.
  const unsigned base_pattern = std::max(td_mapping.sfn_period(), occ_mapping.ssb_period_frames());

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
  unsigned ro_pos = 0;
  for (unsigned sfn = 0; sfn != pattern_frames; ++sfn) {
    if (sfn % assoc_period_frames == 0) {
      // Occasion positions restart at every association period, as per TS 38.213, Section 8.1.
      ro_pos = 0;
    }
    if (td_mapping.is_sfn_prach_occasion(sfn)) {
      for (unsigned slot_idx = 0; slot_idx != nof_slots_per_frame; ++slot_idx) {
        if (slot_positions[slot_idx] < 0) {
          continue;
        }
        const slot_point  sl{ul_scs, sfn, slot_idx};
        prach_slot_entry& entry = prach_slots[sfn * nof_prach_slots_per_frame + slot_positions[slot_idx]];
        entry.first_ro          = static_cast<uint16_t>(ro_pos);
        for (unsigned td = 0; td != nof_td_occasions; ++td) {
          if (not occ_mapping.is_valid_occasion(sl, td)) {
            continue;
          }
          entry.valid_td_mask |= 1U << td;
          ro_pos += nof_fd_occasions;
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
      nof_mapped_ros[sfn / assoc_period_frames] = static_cast<uint16_t>((ro_pos / ros_per_cycle) * ros_per_cycle);
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
  if (td_occasion_idx >= prach_occasion_mapping::max_nof_td_occasions or
      fd_occasion_idx >= occ_mapping.get_nof_fd_occasions()) {
    return std::nullopt;
  }
  const int16_t slot_pos = slot_positions[prach_slot_rx.slot_index()];
  if (slot_pos < 0) {
    return std::nullopt;
  }
  const prach_slot_entry& entry =
      prach_slots[(prach_slot_rx.sfn() % pattern_frames) * nof_prach_slots_per_frame + slot_pos];
  if (((entry.valid_td_mask >> td_occasion_idx) & 1U) == 0) {
    return std::nullopt;
  }

  // Occasions are ordered by frequency index first, then by time index within the PRACH slot, as per TS 38.213,
  // Section 8.1.
  const unsigned nof_earlier_td = count_ones(entry.valid_td_mask & ((1U << td_occasion_idx) - 1U));
  const unsigned ro_pos = entry.first_ro + nof_earlier_td * occ_mapping.get_nof_fd_occasions() + fd_occasion_idx;
  if (ro_pos >= nof_mapped_ros[(prach_slot_rx.sfn() % pattern_frames) / assoc_period_frames]) {
    // The occasions left over after the last complete mapping cycle carry no SS/PBCH block index.
    return std::nullopt;
  }

  const unsigned ssb_pos =
      ((ro_pos % ros_per_cycle) / nof_ro_per_ssb) * nof_ssb_per_ro + preamble_id / preambles_per_ssb;
  if (ssb_pos >= ssb_indexes.size()) {
    return std::nullopt;
  }
  return ssb_indexes[ssb_pos];
}
