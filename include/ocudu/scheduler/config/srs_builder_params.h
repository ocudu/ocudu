// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/bounded_integer.h"
#include "ocudu/ran/srs/srs_configuration.h"
#include <optional>

namespace ocudu {

/// \brief Options for the number of Initial Cyclic Shifts that can be set for PUCCH Format 1.
///
/// Defines the number of different Initial Cyclic Shifts that can be used for PUCCH Format 1, as per \c PUCCH-format1,
/// in \c PUCCH-Config, TS 38.331. We assume the CS are evenly distributed, which means we can only have a divisor of 12
/// possible cyclic shifts.
enum class nof_cyclic_shifts { no_cyclic_shift = 1, two = 2, three = 3, four = 4, six = 6, twelve = 12 };

enum class srs_type { disabled = 0, periodic, aperiodic };

struct srs_builder_params {
  /// Enables and sets the Sounding Reference Signals (SRS) type to be used.
  srs_type srs_type_enabled = srs_type::disabled;
  /// For periodic SRS, defines the SRS period for SRS periodic resources, in slots.
  /// For aperiodic SRS, defines the prohibit time for SRS, in slots (the SRS is not transmitted more often than the
  /// prohibit time).
  /// \remark For aperiodic SRS, \c srs_period_prohib_time is
  srs_periodicity srs_period_prohib_time = srs_periodicity::sl40;
  /// Defines the C_SRS parameter to be used for SRS, as per \c c-SRS, \c SRS-Resource \c SRS-Config, TS 38.331.
  /// If not set, the C_SRS is computed automatically and configured to use the maximum allowed SRS bandwidth.
  /// \remark The Frequency-Hopping SRS parameters assume b-SRS = 0.
  /// Values: {0,..,63}.
  std::optional<unsigned> c_srs = std::nullopt;
  /// Maximum number of symbols per UL slot dedicated for SRS resources.
  /// \remark In case of Sounding Reference Signals (SRS) being used, the number of symbols should be reduced so that
  /// the PUCCH resources do not overlap in symbols with the SRS resources.
  /// \remark The SRS resources are always placed at the end of the slot.
  /// \remark As per TS 38.211, Section 6.4.1.4.1, SRS resource can only be placed in the last 6 symbols of a slot.
  bounded_integer<unsigned, 1, 6> max_nof_symbols = 2U;
  /// \c Transmission comb number, as per TS 38.211, Section 6.4.1.4.2, or TS 38.331, "SRS-Resource".
  tx_comb_size tx_comb = tx_comb_size::n4;
  /// Defines the number of symbols per SRS resource.
  srs_nof_symbols nof_symbols = srs_nof_symbols::n1;
  /// Defines the lowest CRB index of the SRS CRBs interval, \c freqDomainShift. Only used if \ref c_srs is set.
  /// \remark Note this defines the CRB index, not the PRB.
  /// Values: {0,..,268}.
  bounded_integer<unsigned, 0, 268> freq_domain_shift = 0;
  /// Defines the CS reuse factor for the SRS resources.
  /// \remark With 2 or 4 antenna ports, different cyclic shifts are used by the different antennas. This parameter
  /// defines how many UEs can be multiplexed in the same symbols and RBs by exploiting different cyclic shifts.
  /// Values: {no_cyclic_shift, two, three, four, six} for 2 UL antenna ports.
  /// Values: {no_cyclic_shift, three} for 4 UL antenna ports.
  nof_cyclic_shifts cyclic_shift_reuse_factor = nof_cyclic_shifts::no_cyclic_shift;
  /// Defines the reuse of the SRS sequence ID for different UEs within the same cell.
  /// \remark The goal of the SRS sequence ID would be to reduce the inter-cell interference. However, if the cell is
  /// not in a dense multi-cell environment, we can reuse different sequence ID for different cell UEs.
  /// Values: {1, 2, 3, 5, 6, 10, 15, 30}.
  unsigned sequence_id_reuse_factor = 1;
  /// \c p0, TS 38.331. Value in dBm. Only even values allowed within {-202,...,24}.
  int p0 = -84;
};

} // namespace ocudu
