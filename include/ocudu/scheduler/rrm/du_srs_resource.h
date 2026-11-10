// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/bounded_integer.h"
#include "ocudu/ran/resource_allocation/ofdm_symbol_range.h"
#include "ocudu/ran/srs/srs_properties.h"
#include <algorithm>

namespace ocudu {

/// Contains the parameters for the SRS resources of a cell.
struct du_srs_resource {
  /// Id of the cell SRS resource.
  unsigned cell_res_id;
  /// Comb offset, as per \c transmissionComb, \c SRS-Resource, \c SRS-Config, TS 38.331.
  bounded_integer<uint8_t, 0, 4> tx_comb_offset;
  /// OFDM symbol range where the SRS resource is placed.
  ofdm_symbol_range symbols;
  /// \c freqDomainPosition, as per \c SRS-Resource, \c SRS-Config, TS 38.331.
  unsigned freq_dom_position = 0;
  /// \c sequenceId, as per \c SRS-Resource, \c SRS-Config, TS 38.331.
  unsigned sequence_id = 0;
  /// Cyclic shift, as per \c transmissionComb, \c SRS-Resource, \c SRS-Config, TS 38.331.
  unsigned cs = 0;
};

/// Returns the OFDM symbols that the SRS resources use in a partially-UL slot.
/// \param[in] ul_symbols UL symbols of the slot.
/// \param[in] nof_symbols Number of symbols of each SRS resource.
/// \return The symbols of the SRS resources, or an empty range if no SRS resource fits in the slot.
inline ofdm_symbol_range get_srs_symbols_in_partial_ul_slot(ofdm_symbol_range ul_symbols, srs_nof_symbols nof_symbols)
{
  // The SRS can use only the last 6 symbols of the slot, as per TS 38.211, Section 6.4.1.4.1.
  static constexpr unsigned max_srs_symbols = 6U;
  // The SRS resources are placed back to back from the end of the slot.
  const unsigned nof_res_symbols = static_cast<unsigned>(nof_symbols);
  const unsigned nof_srs_symbols = std::min(ul_symbols.length(), max_srs_symbols) / nof_res_symbols * nof_res_symbols;
  return {ul_symbols.stop() - nof_srs_symbols, ul_symbols.stop()};
}

} // namespace ocudu
