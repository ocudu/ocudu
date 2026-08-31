// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/static_vector.h"
#include "ocudu/ran/cyclic_prefix.h"
#include "ocudu/ran/nr_band.h"
#include "ocudu/ran/prach/prach_time_mapping.h"
#include "ocudu/ran/prach/rach_config_common.h"
#include "ocudu/ran/resource_allocation/ofdm_symbol_range.h"
#include "ocudu/ran/slot_point.h"
#include "ocudu/ran/ssb/ssb_configuration.h"
#include "ocudu/ran/tdd/tdd_ul_dl_config.h"
#include <optional>
#include <vector>

namespace ocudu::prach_helper {

/// Cell parameters that determine the association between SS/PBCH block indexes and PRACH occasions.
struct ssb_to_ro_mapping_config {
  nr_band band;
  /// Subcarrier spacing of the initial uplink BWP.
  subcarrier_spacing        ul_scs;
  cyclic_prefix             cp;
  const rach_config_common& rach_cfg;
  const ssb_configuration&  ssb_cfg;
  /// Absent for paired spectrum.
  const std::optional<tdd_ul_dl_config_common>& tdd_cfg;
};

/// Association between SS/PBCH block indexes and PRACH occasions, as per TS 38.213, Section 8.1.
class ssb_to_ro_mapping
{
  /// Maximum number of time-domain PRACH occasions within a PRACH slot, as per TS 38.211, Tables 6.3.3.2-2 to
  /// 6.3.3.2-4.
  static constexpr unsigned max_nof_td_occasions = 8;

  /// PRACH occasions of a single PRACH slot of the association pattern period.
  struct prach_slot_entry {
    /// Ordinal of the first valid PRACH occasion of this slot within its association period.
    uint16_t first_ro = 0;
    /// Bitmap of the time-domain occasions of this slot that are valid PRACH occasions.
    uint8_t valid_td_mask = 0;
  };

public:
  explicit ssb_to_ro_mapping(const ssb_to_ro_mapping_config& config);

  /// Whether a PRACH occasion spanning \c prach_symbols in slot \c sl is valid, as per TS 38.213, Section 8.1.
  bool is_valid_ro(slot_point sl, ofdm_symbol_range prach_symbols) const;

  /// \brief Whether the burst of PRACH occasions starting at slot \c sl can be used, i.e. the slot starts a PRACH
  /// burst and every occasion of that burst is valid, as per TS 38.213, Section 8.1.
  bool is_valid_prach_slot(slot_point sl) const;

  /// \brief SS/PBCH block index associated with a preamble detected in the PRACH occasion
  /// \c (prach_slot_rx, td_occasion_idx, fd_occasion_idx).
  /// \return Nullopt if the occasion is associated with no SS/PBCH block index.
  std::optional<ssb_id_t> get_ssb_index(slot_point prach_slot_rx,
                                        unsigned   td_occasion_idx,
                                        unsigned   fd_occasion_idx,
                                        unsigned   preamble_id) const;

  /// Time-domain positioning of the PRACH preambles of the cell.
  const preamble_slot_mapping& td_slot_mapping() const { return td_mapping; }

  /// Number of system frames spanned by the SS/PBCH block to PRACH occasion association period.
  unsigned association_period_frames() const { return assoc_period_frames; }

  /// Number of system frames after which the association between SS/PBCH blocks and PRACH occasions repeats.
  unsigned association_pattern_period_frames() const { return pattern_frames; }

private:
  /// Symbols that the time-domain occasion \c td_occasion_idx spans in the slot \c slot_offset of a PRACH burst.
  ofdm_symbol_range get_occasion_symbols(unsigned slot_offset, unsigned td_occasion_idx) const;

  /// Whether the time-domain occasion \c td_occasion_idx of the burst starting at slot \c sl is a valid occasion.
  bool is_valid_occasion(slot_point sl, unsigned td_occasion_idx) const;

  /// Fills \c last_ssb_symbol from the SS/PBCH block burst positions of TS 38.213, Section 4.1.
  void build_ssb_symbol_table(const ssb_to_ro_mapping_config& config);

  /// Number of valid PRACH occasions of each of the first \c nof_frames system frames.
  std::vector<unsigned> count_ros_per_frame(unsigned nof_frames) const;

  /// Fills \c prach_slots and derives the number of PRACH occasions associated with an SS/PBCH block index.
  void build_occasion_table();

  // Active SS/PBCH block indexes, in increasing order.
  static_vector<ssb_id_t, NOF_SSB_BEAMS> ssb_indexes;
  // Set when a single SS/PBCH block is active, in which case every PRACH occasion maps to it.
  std::optional<ssb_id_t> single_ssb;

  const bool          paired_spectrum;
  const cyclic_prefix cp;
  // Absent for paired spectrum.
  const std::optional<tdd_ul_dl_config_common> tdd_cfg;
  // N_gap, as per TS 38.213, Table 8.1-2.
  const unsigned n_gap;
  // Subcarrier spacing of the initial uplink BWP.
  const subcarrier_spacing ul_scs;
  const unsigned           nof_slots_per_frame;
  const unsigned           nof_td_occasions;
  const unsigned           nof_fd_occasions;
  const unsigned           preambles_per_ssb;
  // Number of SS/PBCH block indexes associated with one PRACH occasion. Unity below one SSB per occasion.
  const unsigned nof_ssb_per_ro;
  // Number of consecutive PRACH occasions associated with one SS/PBCH block index. Unity above one SSB per occasion.
  const unsigned nof_ro_per_ssb;
  // Starting symbol of the first time-domain PRACH occasion of a PRACH slot.
  const unsigned first_td_occasion_start;
  // Duration of a single time-domain PRACH occasion in symbols. A long preamble spans more than one slot.
  const unsigned td_occasion_duration;
  // Number of slots spanned by a long preamble. Unity for short preamble formats.
  const unsigned nof_burst_slots;
  // Slots of a system frame that start a PRACH occasion, and system frames that hold PRACH occasions.
  preamble_slot_mapping td_mapping;

  // Last symbol occupied by an SS/PBCH block, indexed by slot within the SSB period. -1 when the slot holds no SSB.
  std::vector<int8_t> last_ssb_symbol;
  // Ordinal of each slot of a system frame among the PRACH slots of that frame. -1 when not a PRACH slot.
  std::vector<int16_t> slot_ordinals;
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
