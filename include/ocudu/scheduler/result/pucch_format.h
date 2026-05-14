// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/pucch/pucch_configuration.h"
#include "ocudu/ran/pucch/pucch_mapping.h"
#include "ocudu/ran/pucch/pucch_uci_bits.h"
#include "ocudu/ran/resource_allocation/ofdm_symbol_range.h"
#include "ocudu/ran/resource_allocation/rb_interval.h"

namespace ocudu {

/// PUCCH Format 4 spreading factor.
enum class pucch_format_4_sf { sf2 = 2, sf4 = 4 };

/// PRBs and symbols used for PUCCH resources.
struct pucch_resources {
  prb_interval      prbs;
  ofdm_symbol_range symbols;
  prb_interval      second_hop_prbs{0U, 0U};
};

/// Scheduler output for PUCCH Format 0.
struct pucch_format_0 {
  /// \c pucch-GroupHopping, as per TS 38.331.
  pucch_group_hopping group_hopping;
  /// \f$n_{ID}\f$ as per Section 6.3.2.2.1, TS 38.211.
  unsigned n_id_hopping;
  /// \c initialCyclicShift, as per TS 38.331, or Section 9.2.1, TS 38.211.
  uint8_t initial_cyclic_shift;
};

/// Scheduler output for PUCCH Format 1.
struct pucch_format_1 {
  /// \c pucch-GroupHopping, as per TS 38.331.
  pucch_group_hopping group_hopping;
  /// \f$n_{ID}\f$ as per Section 6.3.2.2.1, TS 38.211.
  unsigned n_id_hopping;
  /// \c initialCyclicShift, as per TS 38.331, or Section 9.2.1, TS 38.211.
  uint8_t initial_cyclic_shift;
  /// \c timeDomainOCC as per TS 38.331, or equivalent to index \f$n\f$ in Table 6.3.2.4.1-2, TS 38.211.
  uint8_t                  time_domain_occ;
  pucch_repetition_tx_slot slot_repetition;
};

/// Scheduler output for PUCCH Format 2.
struct pucch_format_2 {
  /// \f$n_{ID}\f$ as per Section 6.3.2.5.1 and 6.3.2.6.1, TS 38.211.
  uint16_t n_id_scrambling;
  /// \f$N_{ID}^0\f$ as per TS 38.211, Section 6.4.1.3.2.1.
  uint16_t n_id_0_scrambling;
};

/// Scheduler output for PUCCH Format 3.
struct pucch_format_3 {
  /// \c pucch-GroupHopping, as per TS 38.331
  pucch_group_hopping group_hopping;
  /// \f$n_{ID}\f$ as per Section 6.3.2.2.1, TS 38.211.
  unsigned                 n_id_hopping;
  pucch_repetition_tx_slot slot_repetition;
  uint16_t                 n_id_scrambling;
  bool                     pi_2_bpsk;
  /// DMRS parameters.
  bool     additional_dmrs;
  uint16_t n_id_0_scrambling;
};

/// Scheduler output for PUCCH Format 4.
struct pucch_format_4 {
  /// \c pucch-GroupHopping, as per TS 38.331
  pucch_group_hopping group_hopping;
  /// \f$n_{ID}\f$ as per Section 6.3.2.2.1, TS 38.211.
  unsigned                 n_id_hopping;
  pucch_repetition_tx_slot slot_repetition;
  uint16_t                 n_id_scrambling;
  bool                     pi_2_bpsk;
  /// \c occ-Index as per TS 38.331, or equivalent to index \f$n\f$ in Tables 6.3.2.6.3-1/2, TS 38.211. Only for PUCCH
  /// Format 4.
  uint8_t orthog_seq_idx;
  /// Spreading Factor \f$N_{SF}^{PUCCH,4}\f$, as per TS 38.211, Section 6.3.2.6.3. Only for PUCCH Format 4.
  /// TODO: check if this corresponds to \ref pucch_f4_occ_len.
  pucch_format_4_sf n_sf_pucch_f4;

  /// DMRS parameters.
  bool     additional_dmrs;
  uint16_t n_id_0_scrambling;
};

} // namespace ocudu
