// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/bounded_integer.h"
#include "ocudu/ran/resource_allocation/ofdm_symbol_range.h"

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

} // namespace ocudu
