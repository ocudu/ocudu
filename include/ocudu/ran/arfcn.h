// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/bounded_integer.h"

namespace ocudu {

/// Absolute Radio Frequency Channel Number (ARFCN), as per TS 38.104, Section 5.4.2.1.
using arfcn_t = bounded_integer<uint32_t, 0, 3279165>;

/// Max ARFCN valid value, as of TS 38.104 V17.8.0.
constexpr arfcn_t MAX_ARFCN = arfcn_t::max();

/// Min ARFCN value for range of frequencies 3GHz-24.25GHz, as per Table 5.4.2.1-1, TS 38.104.
constexpr arfcn_t MIN_ARFCN_3_GHZ_24_5_GHZ = 600000;
/// Min ARFCN value for range of frequencies above 24.25GHz, as per Table 5.4.2.1-1, TS 38.104.
constexpr arfcn_t MIN_ARFCN_24_5_GHZ_100_GHZ = 2016667;
/// The cutoff frequency for case A, B and C paired is 3GHz, corresponding to 600000 ARFCN (TS 38.213, Section 4.1).
constexpr arfcn_t CUTOFF_FREQ_ARFCN_CASE_A_B_C = MIN_ARFCN_3_GHZ_24_5_GHZ;
/// The cutoff frequency for case C unpaired is 1.88GHz, corresponding to 376000 ARFCN (TS 38.213, Section 4.1).
constexpr arfcn_t CUTOFF_FREQ_ARFCN_CASE_C_UNPAIRED = 376000;

} // namespace ocudu
