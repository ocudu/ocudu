// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

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

/// Cell parameters that determine the position and the validity of the PRACH occasions.
struct prach_occasion_mapping_config {
  nr_band band;
  /// Subcarrier spacing of the initial uplink BWP.
  subcarrier_spacing        ul_scs;
  cyclic_prefix             cp;
  const rach_config_common& rach_cfg;
  const ssb_configuration&  ssb_cfg;
  /// Absent for paired spectrum.
  const std::optional<tdd_ul_dl_config_common>& tdd_cfg;
};

/// Position and validity of the PRACH occasions of a cell, as per TS 38.211 Section 6.3.3.2 and TS 38.213 Section 8.1.
class prach_occasion_mapping
{
public:
  /// Maximum number of time-domain PRACH occasions within a PRACH slot, as per TS 38.211, Tables 6.3.3.2-2 to
  /// 6.3.3.2-4.
  static constexpr unsigned max_nof_td_occasions = 8;

  explicit prach_occasion_mapping(const prach_occasion_mapping_config& config);

  /// Whether a PRACH occasion spanning \c prach_symbols in slot \c sl is valid, as per TS 38.213, Section 8.1.
  bool is_valid_ro(slot_point sl, ofdm_symbol_range prach_symbols) const;

  /// Whether the time-domain occasion \c td_occasion_idx of the burst starting at slot \c sl is a valid occasion.
  bool is_valid_occasion(slot_point sl, unsigned td_occasion_idx) const;

  /// \brief Whether the burst of PRACH occasions starting at slot \c sl can be used, i.e. the slot starts a PRACH
  /// burst and every occasion of that burst is valid, as per TS 38.213, Section 8.1.
  bool is_valid_prach_slot(slot_point sl) const;

  /// Number of time-domain PRACH occasions within a PRACH slot.
  unsigned get_nof_td_occasions() const { return nof_td_occasions; }

  /// Number of frequency multiplexed PRACH occasions.
  unsigned get_nof_fd_occasions() const { return nof_fd_occasions; }

  /// Number of system frames after which the SS/PBCH block burst realigns with the PRACH occasions.
  unsigned ssb_period_frames() const;

  /// Time-domain positioning of the PRACH preambles of the cell.
  const preamble_slot_mapping& td_slot_mapping() const { return td_mapping; }

private:
  /// Symbols that the time-domain occasion \c td_occasion_idx spans in the slot \c slot_offset of a PRACH burst.
  ofdm_symbol_range get_occasion_symbols(unsigned slot_offset, unsigned td_occasion_idx) const;

  // Slots of a system frame that start a PRACH occasion, and system frames that hold PRACH occasions.
  const preamble_slot_mapping td_mapping;

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
  // Starting symbol of the first time-domain PRACH occasion of a PRACH slot.
  const unsigned first_td_occasion_start;
  // Duration of a single time-domain PRACH occasion in symbols. A long preamble spans more than one slot.
  const unsigned td_occasion_duration;
  // Number of slots spanned by a long preamble. Unity for short preamble formats.
  const unsigned nof_burst_slots;

  // Last symbol occupied by an SS/PBCH block, indexed by slot within the SSB period. -1 when the slot holds no SSB.
  const std::vector<int8_t> last_ssb_symbol;
};

} // namespace ocudu::prach_helper
