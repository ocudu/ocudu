// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

/// \file
/// \brief Positioning Reference Signals (PRS) parameter limits.

#pragma once

#include "ocudu/adt/to_array.h"
#include "ocudu/ran/time/radio_frame_constants.h"
#include <algorithm>
#include <cstdint>

namespace ocudu::prs_constants {

/// Maximum number of PRS resource sets per TRP, as per TS 38.455, Section 9.2.44.
inline constexpr unsigned MAX_NOF_RESOURCE_SETS = 8;

/// Maximum number of PRS resources per resource set, as per TS 38.455, Section 9.2.44.
inline constexpr unsigned MAX_NOF_RESOURCES_PER_SET = 64;

/// Minimum PRS bandwidth, in PRBs, as per TS 38.455, Section 9.2.44, "PRS Bandwidth".
inline constexpr unsigned MIN_PRBS = 24;

/// Maximum PRS bandwidth, in PRBs, as per TS 38.455, Section 9.2.44, "PRS Bandwidth".
inline constexpr unsigned MAX_PRBS = 272;

/// Granularity of the PRS bandwidth, in PRBs, as per TS 38.455, Section 9.2.44, "PRS Bandwidth".
inline constexpr unsigned PRB_GRANULARITY = 4;

/// Maximum start PRB of a PRS resource set, relative to Point A, as per TS 38.455, Section 9.2.44, "Start PRB".
inline constexpr unsigned MAX_START_PRB = 2176;

/// Valid comb size values, as per TS 38.455, Section 9.2.44, "Comb Size".
inline constexpr auto VALID_COMB_SIZES = to_array<uint8_t>({2, 4, 6, 12});

/// \brief Valid resource set periodicity values, in slots, for numerology 0 (i.e., \f$\mu=0\f$).
///
/// Periodicities valid for a numerology are these values scaled by \f$2^{\mu}\f$, as per TS 38.211, Section 7.4.1.7.4.
inline constexpr auto VALID_PERIODICITIES_NUMEROLOGY_0 =
    to_array<unsigned>({4, 5, 8, 10, 16, 20, 32, 40, 64, 80, 160, 320, 640, 1280, 2560, 5120, 10240});

/// \brief Valid resource set periodicity values, in slots, across all the supported numerologies.
inline constexpr auto VALID_PERIODICITIES = to_array<unsigned>(
    {4, 5, 8, 10, 16, 20, 32, 40, 64, 80, 160, 320, 640, 1280, 2560, 5120, 10240, 20480, 40960, 81920, 128, 256, 512});

/// Largest resource set periodicity, in slots, of \ref VALID_PERIODICITIES.
inline constexpr unsigned MAX_PERIODICITY_SLOTS =
    *std::max_element(VALID_PERIODICITIES.begin(), VALID_PERIODICITIES.end());

/// Valid resource repetition factor values, as per TS 38.455, Section 9.2.44, "Resource Repetition Factor".
inline constexpr auto VALID_REPETITION_FACTORS = to_array<uint8_t>({1, 2, 4, 6, 8, 16, 32});

/// Valid resource time gap values, in slots, as per TS 38.455, Section 9.2.44, "Resource Time Gap".
inline constexpr auto VALID_TIME_GAPS = to_array<uint8_t>({1, 2, 4, 8, 16, 32});

/// Valid time domain duration values, in OFDM symbols, as per TS 38.455, Section 9.2.44, "Resource Number of Symbols".
inline constexpr auto VALID_NOF_SYMBOLS = to_array<uint8_t>({2, 4, 6, 12});

/// Minimum PRS transmission power offset, in dB, as per TS 38.455, Section 9.2.44, "PRS Resource Transmit Power".
inline constexpr int MIN_POWER_OFFSET_DB = -60;

/// Maximum PRS transmission power offset, in dB, as per TS 38.455, Section 9.2.44, "PRS Resource Transmit Power".
inline constexpr int MAX_POWER_OFFSET_DB = 50;

/// Maximum sequence ID of a PRS resource, as per TS 38.455, Section 9.2.44, "Sequence ID".
inline constexpr unsigned MAX_SEQUENCE_ID = 4095;

/// Maximum slot offset of a PRS resource, as per TS 38.455, Section 9.2.44, "Resource Slot Offset".
inline constexpr unsigned MAX_RES_SLOT_OFFSET = 511;

/// Valid DL-PRS muting pattern bitmap sizes, as per TS 37.355, Section 6.4.3, "NR-MutingPattern-r16".
inline constexpr auto VALID_MUTING_PATTERN_SIZES = to_array<uint8_t>({2, 4, 6, 8, 16, 32});

/// \brief Valid muting bit repetition factor values of a DL-PRS Muting Option 1 pattern.
///
/// See TS 37.355, Section 6.4.3, "DL-PRS-MutingOption1-r16".
inline constexpr auto VALID_MUTING_BIT_REPETITION_FACTORS = to_array<uint8_t>({1, 2, 4, 8});

/// \brief Maximum value of the product of the periodicity, the muting bit repetition factor and the muting pattern
/// size of DL-PRS Muting Option 1, for numerology 0 (i.e., \f$\mu=0\f$).
///
/// Equal to the number of slots in one system frame cycle at numerology 0.
///
/// See TS 38.214, Section 5.1.6.5.
inline constexpr unsigned MAX_MUTING_OPTION1_PRODUCT_NUMEROLOGY_0 =
    radio_frame_constants::NOF_SFNS * radio_frame_constants::NOF_SUBFRAMES_PER_FRAME;

} // namespace ocudu::prs_constants
