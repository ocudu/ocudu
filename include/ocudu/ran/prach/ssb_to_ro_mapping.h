// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/static_vector.h"
#include "ocudu/ran/prach/prach_occasion_mapping.h"
#include "ocudu/ran/slot_point.h"
#include "ocudu/ran/ssb/ssb_configuration.h"
#include <optional>
#include <vector>

namespace ocudu::prach_helper {

/// Association between SS/PBCH block indexes and PRACH occasions, as per TS 38.213, Section 8.1.
class ssb_to_ro_mapping
{
  /// PRACH occasions of a single PRACH slot of the association pattern period.
  struct prach_slot_entry {
    /// Position of the first valid PRACH occasion of this slot within its association period.
    uint16_t first_ro = 0;
    /// Bitmap of the time-domain occasions of this slot that are valid PRACH occasions.
    uint8_t valid_td_mask = 0;
  };

public:
  explicit ssb_to_ro_mapping(const prach_occasion_mapping_config& config);

  /// \brief SS/PBCH block index associated with a preamble detected in the PRACH occasion
  /// \c (prach_slot_rx, td_occasion_idx, fd_occasion_idx).
  /// \return Nullopt if the occasion is associated with no SS/PBCH block index.
  std::optional<ssb_id_t> get_ssb_index(slot_point prach_slot_rx,
                                        unsigned   td_occasion_idx,
                                        unsigned   fd_occasion_idx,
                                        unsigned   preamble_id) const;

  /// Position and validity of the PRACH occasions the association is built on.
  const prach_occasion_mapping& occasions() const { return occ_mapping; }

  /// Number of system frames spanned by the SS/PBCH block to PRACH occasion association period.
  unsigned association_period_frames() const { return assoc_period_frames; }

  /// Number of system frames after which the association between SS/PBCH blocks and PRACH occasions repeats.
  unsigned association_pattern_period_frames() const { return pattern_frames; }

private:
  /// Number of valid PRACH occasions of each of the first \c nof_frames system frames.
  std::vector<unsigned> count_ros_per_frame(unsigned nof_frames) const;

  /// Fills \c prach_slots and derives the number of PRACH occasions associated with an SS/PBCH block index.
  void build_occasion_table();

  // Position and validity of the PRACH occasions of the cell.
  const prach_occasion_mapping occ_mapping;

  // Subcarrier spacing of the initial uplink BWP.
  const subcarrier_spacing ul_scs;
  const unsigned           nof_slots_per_frame;

  // Active SS/PBCH block indexes, in increasing order.
  static_vector<ssb_id_t, NOF_SSB_BEAMS> ssb_indexes;
  // Set when a single SS/PBCH block is active, in which case every PRACH occasion maps to it.
  std::optional<ssb_id_t> single_ssb;

  const unsigned preambles_per_ssb;
  // Number of SS/PBCH block indexes associated with one PRACH occasion. Unity below one SSB per occasion.
  const unsigned nof_ssb_per_ro;
  // Number of consecutive PRACH occasions associated with one SS/PBCH block index. Unity above one SSB per occasion.
  const unsigned nof_ro_per_ssb;

  // Position of each slot of a system frame among the PRACH slots of that frame. -1 when not a PRACH slot.
  std::vector<int16_t> slot_positions;
  // Number of PRACH slots in a system frame.
  unsigned nof_prach_slots_per_frame = 0;

  unsigned assoc_period_frames = 1;
  unsigned pattern_frames      = 1;
  // Number of PRACH occasions associated with an SS/PBCH block index, one entry per association period of the
  // association pattern period.
  std::vector<uint16_t> nof_mapped_ros;
  // Number of PRACH occasions over which all the active SS/PBCH block indexes are mapped exactly once.
  unsigned ros_per_cycle = 0;

  // PRACH slots of the association pattern period, ordered as per TS 38.213, Section 8.1.
  std::vector<prach_slot_entry> prach_slots;
};

} // namespace ocudu::prach_helper
