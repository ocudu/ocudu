// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/nr_band.h"
#include "ocudu/ran/pdcch/search_space.h"
#include "ocudu/ran/sib/sib_configuration.h"
#include "ocudu/ran/ssb/ssb_configuration.h"
#include "ocudu/ran/subcarrier_spacing.h"
#include <vector>

namespace ocudu::sib_helper {

/// Slot offsets occupied by SIB1 PDSCHs together with the period over which they repeat.
struct sib1_sched_occations {
  /// Period of the Type0-CSS monitoring window in \c scs_common slots.
  unsigned window_period_slots;
  /// Unique slot offsets within [0, window_period_slots) where SIB1 PDSCH is scheduled.
  std::vector<unsigned> slot_offsets;
};

/// \brief Returns the slots occupied by SIB1 PDSCH within one Type0-CSS monitoring window, together with the window
/// period.
///
/// [Implementation-defined] Only k0 is used, so the slot of the PDCCH and PDSCH for SIB1 match.
///
/// [Implementation-defined] For multiplexing pattern 1 (FR1), SIB1 is scheduled in slot n0+1 of each active SSB beam's
/// Type0-CSS window (TS 38.213 Section 13).
///
/// For patterns 2 and 3 (FR2), CORESET0 is co-located with the SS/PBCH block, so the occupied slots are the SSB
/// slots themselves. The window period is the SSB period in \c scs_common slots.
///
/// \param ssb_cfg      SSB configuration carrying the active-SSB bitmap, SSB SCS, SSB period, and k_SSB.
/// \param band         NR band, used to determine FR1/FR2 and the CORESET#0 table (TS 38.213 Tables 13-1 to 13-10).
/// \param scs_common   Reference SCS used for slot counting (i.e., the common SCS).
/// \param ss0_idx      SearchSpace#0 index from PDCCH-ConfigSIB1 (parameter \c searchSpaceZero, TS 38.331).
/// \param coreset0_idx CORESET#0 index from PDCCH-ConfigSIB1 (parameter \c controlResourceSetZero, TS 38.331).
/// \param min_sib1_retx_period Minimum periodicity for SIB1 repetitions.
sib1_sched_occations get_occupied_slot_offsets(const ssb_configuration& ssb_cfg,
                                               nr_band                  band,
                                               subcarrier_spacing       scs_common,
                                               search_space0_index      ss0_idx,
                                               uint8_t                  coreset0_idx,
                                               sib1_rtx_periodicity min_sib1_retx_period = sib1_rtx_periodicity::ms160);

} // namespace ocudu::sib_helper
