// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/resource_allocation/ofdm_symbol_range.h"
#include "ocudu/ran/sch/sch_mapping_type.h"
#include <optional>

namespace ocudu {

/// \brief Collects the PDSCH default time-domain allocation parameters.
///
/// The parameters are provided by TS38.214 Tables 5.1.2.1.1-2, 5.1.2.1.1-3, 5.1.2.1.1-4 and 5.1.2.1.1-5, and in
/// TS 38.331.
/// A configuration is invalid if the duration is zero.
struct pdsch_time_domain_resource_allocation {
  /// PDCCH to PDSCH delay in slots, parameter \f$K_0\f$. Values: (0..32).
  uint8_t k0;
  /// PDSCH mapping.
  sch_mapping_type map_type;
  /// \brief Symbols used within the slot. Parameters \f$S\f$ and \f$L\f$. Values for \f$S\f$ are (0..10) and for
  /// \f$L\f$, (2..12).
  ofdm_symbol_range symbols;
  /// \brief Number of PDSCH repetitions, parameter \e repetitionNumber-r16, set only for an entry of a Rel-16
  /// dedicated TDRA list (see TS 38.331, \c PDSCH-TimeDomainResourceAllocation-r16). Values: {2,...,8,16}.
  /// Unset for a legacy (Rel-15) entry, or a Rel-16 entry with no repetitions configured.
  std::optional<uint8_t> rep_number;

  bool operator==(const pdsch_time_domain_resource_allocation& rhs) const
  {
    return k0 == rhs.k0 && map_type == rhs.map_type && symbols == rhs.symbols && rep_number == rhs.rep_number;
  }
  bool operator!=(const pdsch_time_domain_resource_allocation& rhs) const { return !(rhs == *this); }
};

} // namespace ocudu
