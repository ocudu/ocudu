// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

///\file
///\brief pusch_constants - namespace containing constants related to PUSCH transmissions.

#pragma once

#include "ocudu/ran/resource_block.h"
#include "ocudu/support/units.h"
#include <array>

namespace ocudu {

namespace pusch_constants {

/// \brief Maximum number of RE per resource block in a PUSCH transmission.
///
/// As per TS38.214 Section 6.1.4.2.
constexpr unsigned MAX_NRE_PER_RB = 156;

/// \brief Maximum modulation order supported on PUSCH transmissions.
///
/// As per TS38.214 Section 6.1.4.1 with \c mcs-Table set to \c qam256.
constexpr unsigned MAX_MODULATION_ORDER = 8;

/// \brief Maximum number of PUSCH transmission layers.
///
/// As per TS38.211 Section 6.3.1.3.
constexpr unsigned MAX_NOF_LAYERS = 4;

/// \brief Maximum number of receive ports for receiving PUSCH.
constexpr unsigned MAX_NOF_RX_PORTS = 8;

/// Returns number of REs per codeword in a single transmission.
constexpr unsigned get_codeword_max_symbols(unsigned nof_prb, unsigned nof_layers)
{
  return nof_prb * pusch_constants::MAX_NRE_PER_RB * nof_layers;
}

/// Returns number of encoded bits per codeword in a single transmission.
constexpr units::bits get_max_codeword_size(unsigned nof_prb, unsigned nof_layers)
{
  return units::bits{get_codeword_max_symbols(nof_prb, nof_layers) * pusch_constants::MAX_MODULATION_ORDER};
}

/// Maximum number of bits per OFDM symbol.
constexpr unsigned MAX_NOF_BITS_PER_OFDM_SYMBOL =
    MAX_NOF_LAYERS * MAX_NOF_SUBCARRIERS * pusch_constants::MAX_MODULATION_ORDER;

/// Maximum number of OFDM symbols carrying DM-RS in a slot is at most \f$4 \times 2\f$, being 4 the maximum
/// number of positions \f$\bar{l}\f$ and 2 the maximum number of indices \f$l'\f$, as per TS38.211 Section 6.4.1.1.
constexpr unsigned MAX_NOF_DMRS_SYMBOLS = 4 * 2;

/// Maximum number of subcarriers carrying DM-RS in a symbol. It is at most half of the total number of subcarriers
/// (i.e., <tt>MAX_NOF_SUBCARRIERS / 2</tt>) is assigned a DM-RS symbol.
constexpr unsigned MAX_NOF_DMRS_SUBC = MAX_NOF_SUBCARRIERS / 2;

/// Maximum number of PUSCH time domain resource allocations. See TS 38.331, \c maxNrofUL-Allocations.
constexpr unsigned MAX_NOF_PUSCH_TD_RES_ALLOCS = 16;

/// \brief Valid \e numberOfRepetitions-r16 values for an entry of the PUSCH Rel-16 dedicated TDRA list.
/// See TS 38.331, \c PUSCH-Allocation-r16.
constexpr std::array<uint8_t, 8> VALID_NOF_REPETITIONS = {1, 2, 3, 4, 7, 8, 12, 16};

/// \brief Maximum number of PUSCH transmissions of a transport block that the PHY soft-combines. A repetition counts
/// the same as a retransmission.
/// \remark Must match \c rx_buffer_codeblock_pool::max_nof_repetitions.
/// TODO: use a common constant for SCHED and PHY, to follow the limit from one source of truth.
constexpr unsigned MAX_NOF_SOFT_COMBINED_PUSCH_TXS = 32;

} // namespace pusch_constants

} // namespace ocudu
