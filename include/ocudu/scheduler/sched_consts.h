// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/support/units.h"
#include <chrono>
namespace ocudu {

/// SSB constants.
/// FR1 = [ 410 MHz – 7125 MHz] (TS 38.101, Section 5.1) and ARFCN corresponding to 7.125GHz is 875000.
const unsigned FR1_MAX_FREQUENCY_ARFCN = 875000;
const unsigned NOF_SSB_OFDM_SYMBOLS    = 4;

/// SIB1 constants.
/// SIB1 periodicity, see TS 38.331, Section 5.2.1.
constexpr unsigned SIB1_PERIODICITY = 160;
/// [Implementation defined] Max numbers of beams, to be used for SIB1 scheduler.
/// NOTE: This is temporary, and valid only for FR1.
constexpr unsigned MAX_NUM_BEAMS = 8;

/// [Implementation defined] Maximum allowed slot offset between DCI and its scheduled PDSCH. Values {0,..,32}.
constexpr unsigned SCHEDULER_MAX_K0 = 15;

/// [Implementation defined] Maximum allowed slot offset between PDSCH to the DL ACK/NACK. Values {0,..,15}.
constexpr unsigned SCHEDULER_MAX_K1 = 15;

/// [Implementation defined] Maximum allowed slot offset between DCI and its scheduled first PUSCH. Values {0,..,32}.
/// \remark As per TS 38.306, \c ul-SchedulingOffset field in \c ueCapabilityInformation indicates whether the UE
/// supports UL scheduling slot offset (K2) greater than 12.
/// \remark [Implementation defined] While testing with COTS UE, the CRC is KO whenever k2=12 is used to schedule PUSCH.
/// And, when maximum k2 is restricted to 11 the BLER is not seen. Hence, in order to support UEs not supporting k2 > 12
/// and to reduce BLER, we restrict maximum value of k2 to 11.
constexpr unsigned SCHEDULER_MAX_K2 = 11;

/// [Implementation defined] Maximum allowed delay in ms, between PDSCH and BSR injection in triggered UL grant feature.
constexpr std::chrono::milliseconds SCHEDULER_MAX_TRIG_UL_DELAY{16};

/// [Implementation defined] Maximum allowed size of BSR injection in triggered UL grant feature. 1500 bytes coresponds
/// to standard Ethernet packet size.
constexpr units::bytes SCHEDULER_MAX_TRIG_GRANT_SIZE{1500};

/// [Implementation defined] Hardcoded grant size for SRB1 triggered UL grant. The value base on maxiumum RCC processing
/// delay defined in TS 38.331.
constexpr units::bytes SRB1_TRIG_GRANT_SIZE{16};

/// Maximum value of NTN Cell-specific K-offset. The field is expressed in slots for a subcarrier spacing of 15 kHz.
/// See TS 38.331.
constexpr unsigned NTN_CELL_SPECIFIC_KOFFSET_MAX = 1023;

/// Maximum value of Msg delta. See table 6.1.2.1.1-5, in TS 38.214.
constexpr unsigned MAX_MSG3_DELTA = 6;

/// Maximum number of PDSCH time domain resource allocations. See TS 38.331, \c maxNrofDL-Allocations.
constexpr unsigned MAX_NOF_PDSCH_TD_RESOURCE_ALLOCATIONS = 16;

} // namespace ocudu
