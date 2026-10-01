// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/nr_band.h"
#include "ocudu/ran/ssb/ssb_configuration.h"
#include "ocudu/ran/subcarrier_spacing.h"
#include <vector>

namespace ocudu {
namespace ssb_helper {

/// \brief Returns the slot offsets (within one SSB period) where at least one active SSB is transmitted.
///
/// Slot offsets are expressed in \c common_scs slot units. Duplicate slot values (when multiple SSBs share the
/// same common-SCS slot) are reported once. Output order is not guaranteed.
///
/// The mapping from SSB burst symbols to common-SCS slots follows TS 38.213 Section 4.1: for each active SSB index
/// \f$i\f$, the first symbol \f$l_\text{first}\f$ within the half-frame is converted to a slot offset as
/// \f$\lfloor l_\text{first} \cdot N^\text{common}_\text{slots/sf} / (14 \cdot N^\text{SSB}_\text{slots/sf})
/// \rfloor\f$.
///
/// \param ssb_cfg    SSB configuration carrying the active-SSB bitmap and SSB SCS.
/// \param band       NR band, used to derive the SSB pattern case (A–E) per TS 38.213 Table 4.1-1.
/// \param common_scs Reference subcarrier spacing used for slot counting (e.g., TDD ref SCS).
/// \return           List of unique common-SCS slot offsets occupied by active SSBs within one SSB period.
std::vector<unsigned>
get_occupied_slot_offsets(const ssb_configuration& ssb_cfg, nr_band band, subcarrier_spacing common_scs);

} // namespace ssb_helper
} // namespace ocudu
