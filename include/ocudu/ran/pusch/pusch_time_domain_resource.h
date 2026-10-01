// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/resource_allocation/ofdm_symbol_range.h"
#include "ocudu/ran/sch/sch_mapping_type.h"
#include <optional>

namespace ocudu {

/// \brief Collects the PUSCH default time-domain allocation parameters.
///
/// The parameters are provided by TS38.214 Tables 5.1.2.1.1-2, 5.1.2.1.1-3, 5.1.2.1.1-4 and 5.1.2.1.1-5, and in
/// TS 38.331.
///
/// A configuration is invalid if the duration is zero.
struct pusch_time_domain_resource_allocation {
  /// PDCCH to PUSCH delay in slots, parameter \f$K_2\f$. Values: (0..32).
  uint8_t k2;
  /// PUSCH mapping.
  sch_mapping_type map_type;
  /// \brief Symbols used within the slot. Parameters \f$S\f$ and \f$L\f$. Values for \f$S\f$ are (0..10) and for
  /// \f$L\f$, (2..12).
  ofdm_symbol_range symbols;
  /// \brief Number of PUSCH repetitions, parameter \e numberOfRepetitions-r16, set only for an entry of a Rel-16
  /// dedicated TDRA list (see TS 38.331, \c PUSCH-TimeDomainResourceAllocation-r16. Unset for a legacy (Rel-15) entry.
  /// \remark The field \e numberOfRepetitions-r16 is mandatory present in every entry of
  /// \e pusch-TimeDomainAllocationListDCI-0-1-r16, as per TS 38.331.
  std::optional<uint8_t> nof_repetitions;

  bool operator==(const pusch_time_domain_resource_allocation& rhs) const
  {
    return k2 == rhs.k2 && map_type == rhs.map_type && symbols == rhs.symbols && nof_repetitions == rhs.nof_repetitions;
  }
  bool operator!=(const pusch_time_domain_resource_allocation& rhs) const { return !(rhs == *this); }
};

} // namespace ocudu
