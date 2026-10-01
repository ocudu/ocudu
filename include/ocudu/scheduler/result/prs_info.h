// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/cyclic_prefix.h"
#include "ocudu/ran/prs/prs.h"
#include "ocudu/ran/resource_allocation/ofdm_symbol_range.h"
#include "ocudu/ran/resource_allocation/rb_interval.h"
#include "ocudu/ran/subcarrier_spacing.h"

namespace ocudu {

/// \brief Stores the information associated with a DL-PRS transmission occasion.
///
/// It corresponds to one repetition of one DL-PRS resource of a resource set, as per TS 38.211, Section 7.4.1.7.
struct prs_info {
  /// Subcarrier spacing of the DL-PRS transmission.
  subcarrier_spacing scs;
  /// Cyclic prefix of the DL-PRS transmission.
  cyclic_prefix cp;
  /// \brief Sequence ID of the resource, or \f$n_{ID,seq}^{PRS}\f$, as per TS 38.211, Section 7.4.1.7.2.
  ///
  /// Values: {0,...,\ref prs_constants::MAX_SEQUENCE_ID}.
  uint16_t n_id_prs;
  /// Comb size of the resource set, or \f$K_{comb}^{PRS}\f$, as per TS 38.211, Section 7.4.1.7.3.
  prs_comb_size comb_size;
  /// RE offset, or comb offset, of the resource, or \f$k_{offset}^{PRS}\f$. Values: {0,...,comb size - 1}.
  uint8_t comb_offset;
  /// Number of OFDM symbols of the resource, or \f$L_{PRS}\f$.
  prs_num_symbols nof_symbols;
  /// Symbols of the slot occupied by the resource, starting from \f$l_{start}^{PRS}\f$.
  ofdm_symbol_range symbols;
  /// Common RBs occupied by the resource set, where RB 0 is the RB that overlaps with Point A.
  crb_interval crbs;
  /// \brief Transmission power offset of the resource set, in dB.
  ///
  /// Values: {\ref prs_constants::MIN_POWER_OFF_DB,...,\ref prs_constants::MAX_POWER_OFF_DB}.
  int8_t power_offset_db;
};

} // namespace ocudu
